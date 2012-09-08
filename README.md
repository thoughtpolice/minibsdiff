# minibsdiff: a miniature, portable version of bsdiff.

Colin Percival's [bsdiff][] is a popular tool for creating and applying patches
to binary software. This is a stripped down copy of `bsdiff` that's designed to
be portable and reusable as a library in your own software (if you wanted to
say, create your own update system.) Many people end up reusing bsdiff (it's
stable, well-known, works great, and has a good license,) but I haven't found a
standalone copy of the library somewhere that I could easily reuse, so I wrote
one.

This code is based on [bsdiff v4.3](http://www.daemonology.net/bsdiff/bsdiff-4.3.tar.gz).

The main differences:

  * Control and data blocks in the patch output are not `bzip2` compressed.
    You'll have to apply your own compression method. **This is a very
    important part of bsdiff's design, and if you don't apply a compression
    method at some level when using this library, it won't buy you anything**.
    Please see the 'Usage' section below.

  * Patches produced by this library are incompatible with those produced by
    the classic bsdiff tool. (The header format has changed appropriately so
    they are both incompatible with each other.) You're encouraged to change
    this when using the library yourself - see 'Usage' below.

  * The code has been refactored into a reusable API (documented below)
    consisting of a few simple functions in `bsdiff.h` and `bspatch.h`. It
    should be easily usable from any programming language. It has zero external
    dependencies.

  * It works everywhere (even under MSVC.)

  * There's a simple example included that should show you how to get started.

  * Because there are no external dependencies and it's so small, **minibsdiff
    is great place to start if you need to customize bsdiff yourself**! You'll
    inevitably want to do this as time goes on, and most of the work is done
    for you.

[travis-ci.org](http://travis-ci.org) results: [![Build Status](https://secure.travis-ci.org/thoughtpolice/minibsdiff.png?branch=master)](http://travis-ci.org/thoughtpolice/minibsdiff)

# Usage

## Building

Copy `bsdiff.{c,h}`, `bspatch.{c,h}`, `minibsdiff-config.h` and
`{stdbool,stdint}-msvc.h` in your source tree and you're ready to go. You
shouldn't need any special build settings for it to Just Work(TM).

## API

```c
/*-
 * Create a patch from two byte arrays.
 *
 * The input pointer 'patch' must be NULL before usage.
 *
 * Returns -1 if memory can't be allocated, or `*patch` is
 * not NULL. Otherwise, returns the size of the patch stored
 * in `patch`.
 *
 * The `patch` variable must be explicitly free()'d
 * if the function is successful after it's used.
 */
int bsdiff(u_char* oldp, off_t oldsize,
           u_char* newp, off_t newsize,
           u_char** patch);

/** Check if a patch header is valid */
bool bspatch_valid_header(u_char* patch, ssize_t patchsz);

/*-
 * Apply a patch to a byte array.
 *
 * The input pointer 'newp' must be NULL before usage.
 *
 * Returns -1 if memory can't be allocated, or the input
 * pointer `*inp` is not NULL. Returns -2 if the patch
 * header is invalid. Returns -3 if the patch itself is
 * corrupt. Otherwise, returns 0.
 *
 * The output pointer 'newp' must be free()'d after usage
 * if this function is successful.
 */
int bspatch(u_char* oldp,  ssize_t oldsz,
            u_char* patch, ssize_t patchsz,
            u_char** newp, ssize_t* newsz);
```

## Building the example program.

For an full example of using the API, see `minibsdiff.c`, which roughly
reimplements the standard `bsdiff/bspatch` in a single tool (without
compression.) To build it:

  * Running `make` on Linux or OS X. If you have MinGW installed and on
    your `PATH` then you can do 'make MinGW=YES' which will build an
    `.exe` on Windows.

  * There is a `CMakeLists.txt` file you can use to generate Ninja, MSVC or
    MinGW makefile projects for Windows as well. You can of course use `cmake`
    on Linux/OS X as well.

## Customization notes.

You can change the patch file's magic number by modifying `BSDIFF_CONFIG_MAGIC`
in `minibsdiff-config.h`. It must be 8 bytes long (anything beyond that will be
ignored.) This library by default has the magic number `MBSDIF43`.

---

**You should really, really, really compress the output in some way**. Whether
or not you do that directly in the diff/patch routines or on the result you get
from calling them is irrelevant. If you don't do this, **bsdiff will buy you
nothing**.

Briefly, bsdiff is based on the concept of finding approximate matches between
two executable files, and calculating and storing their bytewise differences in
the patch file. The patch format is roughly composed of a control block
specifying how to add and insert changes from the new executable into the old
one, and a difference block actually composed of the differences.

Binary updates to software packages tend to have disproportionate amounts of
binary-level differences from just a few source code changes. The key
observation however is that most code is still the same, but *relocated* in
such a way that things like internal pointers are always offset in a
predictable manner.  For example, if you have a single translation unit with 5
functions, and you fix a small bug in this code and ship it to users, the
*symbolic representation* has not changed all that much, but the change will
result in *executable* differences affecting all 5 functions, such that e.g.
relative pointers must all be adjusted properly, across all of them.

But even then, many of these 'relocations' will be small (a byte or two,) and
more than that, they will often be very regular, meaning the differences are
highly redundant, and thus compressible.

As a result, an uncompressed patch from bsdiff is roughly on par with the new
file in size, but compression can reduce it's size dramatically due to repeated
data in the differences (by a factor of 10x or 20x.) In fact, without some sort
of compression, it practically defeats the purpose of using it in the first
place!

Not having compression by default is still a feature, though - it keeps the
library simple and portable, and you can layer it in however you want because
the source is small and easy to hack. But realistically, you'll **always** want
to compress it at one point or another in the Real World.

Here are some good compression libraries you might be interested in:

  * [zlib](http://www.zlib.net)
  * [gzip](http://www.gzip.org)
  * [lz4](http://code.google.com/p/lz4)
  * [snappy](http://code.google.com/p/snappy)
  * [quicklz](http://www.quicklz.com)

In my non-scientific experiments, **bzip at compression level 9 gives the best
output size** out of all the ones listed above. It's obviously worth
sacrificing compression time/speed for smaller updates that decompress quickly.

# Join in

File bugs in the GitHub [issue tracker][].

Master [git repository][gh]:

* `git clone https://github.com/thoughtpolice/minibsdiff.git`

There's also a [BitBucket mirror][bb]:

* `git clone https://bitbucket.org/thoughtpolice/minibsdiff.git`

If you're going to submit a pull request or send a patch, **sign off on your
changes** by using `git commit -s`. I manage the `Signed-off by` field like
git: by signing off, you acknowledge that the code you are submitting for
inclusion bides by the license of the project. An `Acked-by` field states that
someone has reviewed this code, and at the very least it is not completely
insane.

# Authors

See [AUTHORS.txt](https://raw.github.com/thoughtpolice/minibsdiff/master/AUTHORS.txt).

# License

2-clause BSD. See `LICENSE.txt` for terms of copyright and redistribution.

[bsdiff]: http://www.daemonology.net/bsdiff/
[issue tracker]: http://github.com/thoughtpolice/minibsdiff/issues
[gh]: http://github.com/thoughtpolice/minibsdiff
[bb]: http://bitbucket.org/thoughtpolice/minibsdiff
