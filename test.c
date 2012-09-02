/*-
 * This is just a simple example of using
 * the portable bsdiff API. Parts of it are derived
 * from the original bsdiff/bspatch.
 *
 * Compile with:
 *
 *   $ cc -Wall -std=gnu99 -O2 test.c bsdiff.c bspatch.c
 *
 * Usage:
 *
 *   $ ./a.out gen <v1> <v2> <patch>
 *   $ ./a.out app <v1> <patch> <v2>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <err.h>

#include <unistd.h>
#include <fcntl.h>

#include "bspatch.h"
#include "bsdiff.h"

#define bail() (err(1, "%s", progname))
#define barf(msg) (errx(1, "%s", msg))

#define max(a,b) \
       ({ typeof (a) _a = (a); \
           typeof (b) _b = (b); \
         _a > _b ? _a : _b; })

#define zmalloc(sz) \
    ({ void* _ptr = malloc(sz); \
       if (_ptr == NULL) bail(); \
       memset(_ptr, 0xAA, sz); \
       _ptr; })

static char* progname;

static void
usage(void)
{
  errx(1, "usage:\n\n"
          "Generate patch:"
          "\t$ %s gen <v1> <v2> <patch>\n"
          "Apply patch:"
          "\t$ %s app <v1> <patch> <v2>\n", progname, progname);
}

long
read_file(const char* f, u_char** buf)
{
  FILE* fp;
  long fsz;

  if ( ((fp = fopen(f, "r"))    == NULL) ||
       (fseek(fp, 0, SEEK_END)  != 0)    ||
       ((fsz = ftell(fp))       == -1)   ||
       ((*buf = zmalloc(fsz+1)) == NULL) ||
       (fseek(fp, 0, SEEK_SET)  != 0)    ||
       (fread(*buf, 1, fsz, fp) != fsz)  ||
       (fclose(fp)              != 0)
     ) bail();

  return fsz;
}

void
write_file(const char* f, u_char* buf, long sz)
{
  FILE* fp;

  if ( ((fp = fopen(f, "w")) == NULL) ||
       (fwrite(buf, 1, sz, fp) != sz) ||
       (fclose(fp)             != 0)
     ) bail();

  return;
}

void
diff(const char* oldf, const char* newf, const char* patchf)
{
  u_char* old;
  u_char* new;
  u_char* patch;
  long oldsz, newsz, patchsz;
  int res;

  printf("Generating binary patch between %s and %s\n", oldf, newf);

  /* Read old and new files */
  oldsz = read_file(oldf, &old);
  newsz = read_file(newf, &new);
  printf("Old file = %lu bytes\nNew file = %lu bytes\n", oldsz, newsz);

  /* Debugging */
  /* fwrite(old, 1, oldsz, stdout); */
  /* fwrite(new, 1, newsz, stdout); */

  /* Allocate for size of patch. Worst case, the files are totally different,
   * so we'll require new+old amount of space.
   */
  patchsz = oldsz + newsz + 1024; /* Just to be safe */
  patch = zmalloc(patchsz);

  /* Compute delta */
  printf("Computing binary delta...\n");
  res = bsdiff(old, oldsz, new, newsz, patch);
  if (res <= 0) barf("bsdiff() failed!");
  patchsz = res;
  printf("sizeof(delta('%s', '%s')) = %lu bytes\n", oldf, newf, patchsz);

  /* Write patch */
  write_file(patchf, patch, patchsz);

  free(old);
  free(new);
  free(patch);

  printf("Created patch file %s\n", patchf);
  exit(EXIT_SUCCESS);
}

void
patch(const char* inf, const char* patchf, const char* outf)
{
  u_char* inp;
  u_char* patchp;
  u_char* newp;
  long insz, patchsz;
  ssize_t newsz;
  int res;

  printf("Applying binary patch %s to %s\n", patchf, inf);

  /* Read old file and patch file */
  insz    = read_file(inf, &inp);
  patchsz = read_file(patchf, &patchp);
  printf("Old file = %lu bytes\nPatch file = %lu bytes\n", insz, patchsz);

  /* Figure out new file size */
  newsz = bspatch_newsize(patchp);
  if (newsz <= 0) barf("Corrupt patch\n");
  newp = zmalloc(newsz);
  printf("New file size = %lu bytes\n", newsz);

  /* Apply delta */
  res = bspatch(inp, insz, patchp, patchsz, newp);
  if (res != 0) barf("bspatch() failed!");

  /* Write new file */
  write_file(outf, newp, newsz);

  free(inp);
  free(patchp);
  free(newp);

  printf("Successfully applied patch; new file is %s\n", outf);
  exit(EXIT_SUCCESS);
}

int
main(int ac, char* av[])
{
  progname = av[0];

  if (ac != 5) usage();

  if (memcmp(av[1], "gen", 3) == 0)
    diff(av[2], av[3], av[4]);

  if (memcmp(av[1], "app", 3) == 0)
    patch(av[2], av[3], av[4]);

  usage(); /* patch()/diff() don't return */
  return 0;
}
