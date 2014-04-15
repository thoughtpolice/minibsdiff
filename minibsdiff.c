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

/*-
 * This is just a simple example of using
 * the portable bsdiff API. Parts of it are derived
 * from the original bsdiff/bspatch.
 *
 * Compile with:
 *
 *   $ cc -Wall -std=c99 -O2 minibsdiff.c bsdiff.c bspatch.c
 *
 * Usage:
 *
 *   $ ./a.out gen <v1> <v2> <patch>
 *   $ ./a.out app <v1> <patch> <v2>
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif /* _MSC_VER */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

/* Create one large compilation unit */
#include "bspatch.c"
#include "bsdiff.c"

/* ------------------------------------------------------------------------- */
/* -- Utilities ------------------------------------------------------------ */

static char* progname;

static void
barf(const char* msg)
{
  printf("%s: ERROR: %s", progname, msg);
  exit(EXIT_FAILURE);
}

static void
usage(void)
{
  printf("usage:\n\n"
         "Generate patch:"
         "\t$ %s gen <v1> <v2> <patch>\n"
         "Apply patch:"
         "\t$ %s app <v1> <patch> <v2>\n", progname, progname);
  exit(EXIT_FAILURE);
}

static long
read_file(const char* f, u_char** buf)
{
  FILE* fp;
  long fsz;

  fsz = 0;
  if ( ((fp = fopen(f, "rb"))  == NULL)         ||
       (fseek(fp, 0, SEEK_END)  != 0)           ||
       ((fsz = ftell(fp))       == -1)          ||
       ((*buf = malloc(fsz+1))  == NULL)        ||
       (fseek(fp, 0, SEEK_SET)  != 0)           ||
       (fread(*buf, 1, fsz, fp) != (size_t)fsz) ||
       (fclose(fp)              != 0)
     ) barf("Couldn't open file for reading!\n");

  return fsz;
}

static void
write_file(const char* f, u_char* buf, long sz)
{
  FILE* fp;

  if ( ((fp = fopen(f, "w+b")) == NULL)       ||
       (fwrite(buf, 1, sz, fp) != (size_t)sz) ||
       (fclose(fp)             != 0)
     ) barf("Couldn't open file for writing!\n");

  return;
}

/* ------------------------------------------------------------------------- */
/* -- Main routines -------------------------------------------------------- */

static void
diff(const char* oldf, const char* newf, const char* patchf)
{
  u_char* old;
  u_char* new;
  u_char* patch;
  long oldsz, newsz;
  off_t patchsz;
  int res;

#ifndef NDEBUG
  printf("Generating binary patch between %s and %s\n", oldf, newf);
#endif /* NDEBUG */

  /* Read old and new files */
  oldsz = read_file(oldf, &old);
  newsz = read_file(newf, &new);

#ifndef NDEBUG
  printf("Old file = %lu bytes\nNew file = %lu bytes\n", oldsz, newsz);
#endif /* NDEBUG */

  /* Compute delta */
#ifndef NDEBUG
  printf("Computing binary delta...\n");
#endif /* NDEBUG */

  patchsz = bsdiff_patchsize_max(oldsz, newsz);
  patch = malloc(patchsz+1); /* Never malloc(0) */
  res = bsdiff(old, oldsz, new, newsz, patch, patchsz);
  if (res <= 0) barf("bsdiff() failed!");
  patchsz = res;

#ifndef NDEBUG
  printf("sizeof(delta('%s', '%s')) = %ld bytes\n", oldf, newf, patchsz);
#endif /* NDEBUG */

  /* Write patch */
  write_file(patchf, patch, patchsz);

  free(old);
  free(new);
  free(patch);

#ifndef NDEBUG
  printf("Created patch file %s\n", patchf);
#endif /* NDEBUG */
  exit(EXIT_SUCCESS);
}

static void
patch(const char* inf, const char* patchf, const char* outf)
{
  u_char* inp;
  u_char* patchp;
  u_char* newp;
  long insz, patchsz;
  ssize_t newsz;
  int res;

#ifndef NDEBUG
  printf("Applying binary patch %s to %s\n", patchf, inf);
#endif /* NDEBUG */

  /* Read old file and patch file */
  insz    = read_file(inf, &inp);
  patchsz = read_file(patchf, &patchp);
#ifndef NDEBUG
  printf("Old file = %lu bytes\nPatch file = %lu bytes\n", insz, patchsz);
#endif /* NDEBUG */

  /* Apply delta */
  newsz = bspatch_newsize(patchp, patchsz);
  if (newsz <= 0) barf("Couldn't determine new file size; patch corrupt!");

  newp = malloc(newsz+1); /* Never malloc(0) */
  res = bspatch(inp, insz, patchp, patchsz, newp, newsz);
  if (res != 0) barf("bspatch() failed!");

  /* Write new file */
  write_file(outf, newp, newsz);

  free(inp);
  free(patchp);
  free(newp);

#ifndef NDEBUG
  printf("Successfully applied patch; new file is %s\n", outf);
#endif /* NDEBUG */
  exit(EXIT_SUCCESS);
}

/* ------------------------------------------------------------------------- */
/* -- Driver --------------------------------------------------------------- */

int
main(int ac, char* av[])
{
  /* WIN32 FIXME: av[0] becomes the full path to minibsdiff */
  progname = av[0];
  if (ac != 5) usage();

  if (memcmp(av[1], "gen", 3) == 0)
    diff(av[2], av[3], av[4]);
  if (memcmp(av[1], "app", 3) == 0)
    patch(av[2], av[3], av[4]);

  usage(); /* patch()/diff() don't return */
  return 0;
}
