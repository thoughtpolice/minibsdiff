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
__FBSDID("$FreeBSD: src/usr.bin/bsdiff/bspatch/bspatch.c,v 1.1 2005/08/06 01:59:06 cperciva Exp $");
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "bspatch.h"

/*
  Patch file format:
  0        8       BSDIFF_CONFIG_MAGIC (see minibsdiff-config.h)
  8        8       X
  16       8       Y
  24       8       sizeof(newfile)
  32       X       control block
  32+X     Y       diff block
  32+X+Y   ???     extra block
  with control block a set of triples (x,y,z) meaning "add x bytes
  from oldfile to x bytes from the diff block; copy y bytes from the
  extra block; seek forwards in oldfile by z bytes".
*/

static off_t
offtin(u_char *buf)
{
  off_t y;

  y=buf[7]&0x7F;
  y=y*256;y+=buf[6];
  y=y*256;y+=buf[5];
  y=y*256;y+=buf[4];
  y=y*256;y+=buf[3];
  y=y*256;y+=buf[2];
  y=y*256;y+=buf[1];
  y=y*256;y+=buf[0];

  if(buf[7]&0x80) y=-y;

  return y;
}

bool
bspatch_valid_header(u_char* patch, ssize_t patchsz)
{
  ssize_t newsize, ctrllen, datalen;

  if (patchsz < 32) return false;

  /* Make sure magic and header fields are valid */
  if(memcmp(patch, BSDIFF_CONFIG_MAGIC, 8) != 0) return false;

  ctrllen=offtin(patch+8);
  datalen=offtin(patch+16);
  newsize=offtin(patch+24);
  if((ctrllen<0) || (datalen<0) || (newsize<0))
    return false;

  return true;
}

ssize_t
bspatch_newsize(u_char* patch, ssize_t patchsz)
{
  if (!bspatch_valid_header(patch, patchsz)) return -1;
  return offtin(patch+24);
}

int
bspatch(u_char* oldp,  ssize_t oldsz,
        u_char* patch, ssize_t patchsz,
        u_char* newp,  ssize_t newsz)
{
  ssize_t newsize,ctrllen,datalen;
  u_char *ctrlblock, *diffblock, *extrablock;
  off_t oldpos,newpos;
  off_t ctrl[3];
  off_t i;

  /* Sanity checks */
  if (oldp == NULL || patch == NULL || newp == NULL) return -1;
  if (oldsz < 0    || patchsz < 0   || newsz < 0)    return -1;
  if (!bspatch_valid_header(patch, patchsz)) return -2;

  /* Read lengths from patch header */
  ctrllen=offtin(patch+8);
  datalen=offtin(patch+16);
  newsize=offtin(patch+24);
  if (newsize > newsz) return -1;

  /* Get pointers into the header metadata */
  ctrlblock  = patch+32;
  diffblock  = patch+32+ctrllen;
  extrablock = patch+32+ctrllen+datalen;

  /* Apply patch */
  oldpos=0;newpos=0;
  while(newpos<newsize) {
    /* Read control block */
    ctrl[0] = offtin(ctrlblock);
    ctrl[1] = offtin(ctrlblock+8);
    ctrl[2] = offtin(ctrlblock+16);
    ctrlblock += 24;

    /* Sanity check */
    if(newpos+ctrl[0]>newsize)
      return -3; /* Corrupt patch */

    /* Read diff string */
    memcpy(newp + newpos, diffblock, ctrl[0]);
    diffblock += ctrl[0];

    /* Add old data to diff string */
    for(i=0;i<ctrl[0];i++)
      if((oldpos+i>=0) && (oldpos+i<oldsz))
        newp[newpos+i]+=oldp[oldpos+i];

    /* Adjust pointers */
    newpos+=ctrl[0];
    oldpos+=ctrl[0];

    /* Sanity check */
    if(newpos+ctrl[1]>newsize)
      return -3; /* Corrupt patch */

    /* Read extra string */
    memcpy(newp + newpos, extrablock, ctrl[1]);
    extrablock += ctrl[1];

    /* Adjust pointers */
    newpos+=ctrl[1];
    oldpos+=ctrl[2];
  };

  return 0;
}
