/*-
 * Copyright 2012-2013 Austin Seipp
 * Copyright 2003-2005 Colin Percival
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if 0
__FBSDID("$FreeBSD: src/usr.bin/bsdiff/bsdiff/bsdiff.c,v 1.1 2005/08/06 01:59:05 cperciva Exp $");
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "bsdiff.h"

#define MIN(x,y) (((x)<(y)) ? (x) : (y))

/* Header is
   0  8       BSDIFF_CONFIG_MAGIC (see minibsdiff-config.h)
   8  8       length of bzip2ed ctrl block
   16 8       length of bzip2ed diff block
   24 8       length of new file */
/* File is
   0  32      Header
   32 ??      ctrl block
   ?? ??      diff block
   ?? ??      extra block */


static void
split(off_t *I,off_t *V,off_t start,off_t len,off_t h)
{
  off_t i,j,k,x,tmp,jj,kk;

  if(len<16) {
    for(k=start;k<start+len;k+=j) {
      j=1;x=V[I[k]+h];
      for(i=1;k+i<start+len;i++) {
        if(V[I[k+i]+h]<x) {
          x=V[I[k+i]+h];
          j=0;
        };
        if(V[I[k+i]+h]==x) {
          tmp=I[k+j];I[k+j]=I[k+i];I[k+i]=tmp;
          j++;
        };
      };
      for(i=0;i<j;i++) V[I[k+i]]=k+j-1;
      if(j==1) I[k]=-1;
    };
    return;
  };

  x=V[I[start+len/2]+h];
  jj=0;kk=0;
  for(i=start;i<start+len;i++) {
    if(V[I[i]+h]<x) jj++;
    if(V[I[i]+h]==x) kk++;
  };
  jj+=start;kk+=jj;

  i=start;j=0;k=0;
  while(i<jj) {
    if(V[I[i]+h]<x) {
      i++;
    } else if(V[I[i]+h]==x) {
      tmp=I[i];I[i]=I[jj+j];I[jj+j]=tmp;
      j++;
    } else {
      tmp=I[i];I[i]=I[kk+k];I[kk+k]=tmp;
      k++;
    };
  };

  while(jj+j<kk) {
    if(V[I[jj+j]+h]==x) {
      j++;
    } else {
      tmp=I[jj+j];I[jj+j]=I[kk+k];I[kk+k]=tmp;
      k++;
    };
  };

  if(jj>start) split(I,V,start,jj-start,h);

  for(i=0;i<kk-jj;i++) V[I[jj+i]]=kk-1;
  if(jj==kk-1) I[jj]=-1;

  if(start+len>kk) split(I,V,kk,start+len-kk,h);
}

static void
qsufsort(off_t *I,off_t *V,u_char *old,off_t oldsize)
{
  off_t buckets[256];
  off_t i,h,len;

  for(i=0;i<256;i++) buckets[i]=0;
  for(i=0;i<oldsize;i++) buckets[old[i]]++;
  for(i=1;i<256;i++) buckets[i]+=buckets[i-1];
  for(i=255;i>0;i--) buckets[i]=buckets[i-1];
  buckets[0]=0;

  for(i=0;i<oldsize;i++) I[++buckets[old[i]]]=i;
  I[0]=oldsize;
  for(i=0;i<oldsize;i++) V[i]=buckets[old[i]];
  V[oldsize]=0;
  for(i=1;i<256;i++) if(buckets[i]==buckets[i-1]+1) I[buckets[i]]=-1;
  I[0]=-1;

  for(h=1;I[0]!=-(oldsize+1);h+=h) {
    len=0;
    for(i=0;i<oldsize+1;) {
      if(I[i]<0) {
        len-=I[i];
        i-=I[i];
      } else {
        if(len) I[i-len]=-len;
        len=V[I[i]]+1-i;
        split(I,V,i,len,h);
        i+=len;
        len=0;
      };
    };
    if(len) I[i-len]=-len;
  };

  for(i=0;i<oldsize+1;i++) I[V[i]]=i;
}

static off_t
matchlen(u_char *oldp,off_t oldsize,u_char *newp,off_t newsize)
{
  off_t i;

  for(i=0;(i<oldsize)&&(i<newsize);i++)
    if(oldp[i]!=newp[i]) break;

  return i;
}

static off_t
search(off_t *I,u_char *oldp,off_t oldsize,
       u_char *newp,off_t newsize,off_t st,off_t en,off_t *pos)
{
  off_t x,y;

  if(en-st<2) {
    x=matchlen(oldp+I[st],oldsize-I[st],newp,newsize);
    y=matchlen(oldp+I[en],oldsize-I[en],newp,newsize);

    if(x>y) {
      *pos=I[st];
      return x;
    } else {
      *pos=I[en];
      return y;
    }
  };

  x=st+(en-st)/2;
  if(memcmp(oldp+I[x],newp,MIN(oldsize-I[x],newsize))<0) {
    return search(I,oldp,oldsize,newp,newsize,x,en,pos);
  } else {
    return search(I,oldp,oldsize,newp,newsize,st,x,pos);
  };
}

static void
offtout(off_t x,u_char *buf)
{
  off_t y;

  if(x<0) y=-x; else y=x;

  buf[0]=y%256;y-=buf[0];
  y=y/256;buf[1]=y%256;y-=buf[1];
  y=y/256;buf[2]=y%256;y-=buf[2];
  y=y/256;buf[3]=y%256;y-=buf[3];
  y=y/256;buf[4]=y%256;y-=buf[4];
  y=y/256;buf[5]=y%256;y-=buf[5];
  y=y/256;buf[6]=y%256;y-=buf[6];
  y=y/256;buf[7]=y%256;

  if(x<0) buf[7]|=0x80;
}

off_t
bsdiff_patchsize_max(off_t newsize, off_t oldsize)
{
  return newsize+oldsize+BSDIFF_PATCH_SLOP_SIZE;
}

int bsdiff(u_char* oldp, off_t oldsize,
           u_char* newp, off_t newsize,
           u_char* patch, off_t patchsz)
{
  off_t *I,*V;
  off_t scan,pos,len;
  off_t lastscan,lastpos,lastoffset;
  off_t oldscore,scsc;
  off_t s,Sf,lenf,Sb,lenb;
  off_t overlap,Ss,lens;
  off_t i;
  off_t dblen,eblen;
  u_char *db,*eb;
  u_char buf[8];
  u_char header[32];
  u_char *fileblock;

  off_t ctrllen;

  /* Sanity checks */
  if (oldp == NULL || newp == NULL || patch == NULL) return -1;
  if (oldsize < 0 || newsize < 0 || patchsz < 0)     return -1;
  if (bsdiff_patchsize_max(oldsize, newsize) > patchsz) return -1;

  /* Allocate oldsize+1 bytes instead of oldsize bytes to ensure
     that we never try to malloc(0) and get a NULL pointer */
  if(((I=malloc((oldsize+1)*sizeof(off_t)))==NULL) ||
     ((V=malloc((oldsize+1)*sizeof(off_t)))==NULL)) return -1;

  qsufsort(I,V,oldp,oldsize);

  free(V);

  /* Allocate newsize+1 bytes instead of newsize bytes to ensure
     that we never try to malloc(0) and get a NULL pointer */
  if(((db=malloc(newsize+1))==NULL) ||
     ((eb=malloc(newsize+1))==NULL)) {
    free(I);
    return -1;
  }
  dblen=0;
  eblen=0;

  /* Write initial header */
  memcpy(header,BSDIFF_CONFIG_MAGIC,8);
  offtout(0, header + 8);
  offtout(0, header + 16);
  offtout(newsize, header + 24);
  memcpy(patch, header, 32);

  /* Set up initial pointers */
  fileblock = patch + 32;
  ctrllen = 0;

  /* Compute the differences, writing ctrl as we go */
  scan=0;len=0;pos=0;
  lastscan=0;lastpos=0;lastoffset=0;
  while(scan<newsize) {
    oldscore=0;

    for(scsc=scan+=len;scan<newsize;scan++) {
      len=search(I,oldp,oldsize,newp+scan,newsize-scan,
                 0,oldsize,&pos);

      for(;scsc<scan+len;scsc++)
        if((scsc+lastoffset<oldsize) &&
           (oldp[scsc+lastoffset] == newp[scsc]))
          oldscore++;

      if(((len==oldscore) && (len!=0)) ||
         (len>oldscore+8)) break;

      if((scan+lastoffset<oldsize) &&
         (oldp[scan+lastoffset] == newp[scan]))
        oldscore--;
    };

    if((len!=oldscore) || (scan==newsize)) {
      s=0;Sf=0;lenf=0;
      for(i=0;(lastscan+i<scan)&&(lastpos+i<oldsize);) {
        if(oldp[lastpos+i]==newp[lastscan+i]) s++;
        i++;
        if(s*2-i>Sf*2-lenf) { Sf=s; lenf=i; };
      };

      lenb=0;
      if(scan<newsize) {
        s=0;Sb=0;
        for(i=1;(scan>=lastscan+i)&&(pos>=i);i++) {
          if(oldp[pos-i]==newp[scan-i]) s++;
          if(s*2-i>Sb*2-lenb) { Sb=s; lenb=i; };
        };
      };

      if(lastscan+lenf>scan-lenb) {
        overlap=(lastscan+lenf)-(scan-lenb);
        s=0;Ss=0;lens=0;
        for(i=0;i<overlap;i++) {
          if(newp[lastscan+lenf-overlap+i]==
             oldp[lastpos+lenf-overlap+i]) s++;
          if(newp[scan-lenb+i]==
             oldp[pos-lenb+i]) s--;
          if(s>Ss) { Ss=s; lens=i+1; };
        };

        lenf+=lens-overlap;
        lenb-=lens;
      };

      for(i=0;i<lenf;i++)
        db[dblen+i]=newp[lastscan+i]-oldp[lastpos+i];
      for(i=0;i<(scan-lenb)-(lastscan+lenf);i++)
        eb[eblen+i]=newp[lastscan+lenf+i];

      dblen+=lenf;
      eblen+=(scan-lenb)-(lastscan+lenf);

      offtout(lenf,buf);
      memcpy(fileblock, buf, 8);
      fileblock += 8; ctrllen += 8;

      offtout((scan-lenb)-(lastscan+lenf),buf);
      memcpy(fileblock, buf, 8);
      fileblock += 8; ctrllen += 8;

      offtout((pos-lenb)-(lastpos+lenf),buf);
      memcpy(fileblock, buf, 8);
      fileblock += 8; ctrllen += 8;

      lastscan=scan-lenb;
      lastpos=pos-lenb;
      lastoffset=pos-scan;
    };
  };

  /* Write size of ctrl data */
  offtout(ctrllen, header + 8);

  /* Write diff data */
  memcpy(fileblock, db, dblen);
  fileblock += dblen;
  /* Write size of diff data */
  offtout(dblen, header + 16);

  /* Write extra data */
  memcpy(fileblock, eb, eblen);

  /* Write the final header */
  memcpy(patch, header, 32);

  /* Free the memory we used */
  free(db);
  free(eb);
  free(I);

  return (32+ctrllen+dblen+eblen);
}
