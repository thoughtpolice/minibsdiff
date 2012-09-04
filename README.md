# minibsdiff: a miniature, portable version of bsdiff.

Colin Percival's [bsdiff][] is a popular tool for creating and applying binary
patches to software. This is a stripped down copy of `bsdiff` that's designed
to be portable and reusable as a library in your own software (if you wanted to
say, create your own update system.) Many people end up reusing bsdiff (it's
stable, well-known, small, and has a good license,) but I haven't found a
standalone copy of the library somewhere that I could easily reuse, so I wrote
one.

This code is based on bsdiff v4.3.

The main differences:

  * Control/data/extra blocks in the patch output are not `bzip2` compressed.
    You'll have to apply your own compression method. **This means that the
    patches produced by this library are incompatible with those produced by
    the classic bsdiff tool!**

  * The code has been refactored into a reusable API, consisting of a few
    simple functions in `bsdiff.h` and `bspatch.h`.

  * It works everywhere (even under MSVC.)

[travis-ci.org](http://travis-ci.org) results: [![Build Status](https://secure.travis-ci.org/thoughtpolice/minibsdiff.png?branch=master)](http://travis-ci.org/thoughtpolice/minibsdiff)

# Usage

Just include `bsdiff.{c,h}` and `bspatch.{c,h}` in your source tree and you're
ready to go.

This is the entire API:

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

For an full example of using the API, see `minibsdiff.c`, which roughly
reimplements the standard `bsdiff/bspatch` in a single tool. You can
build it by:

  * Running `make` on Linux/OS X.
  * Running `make` on Windows will build with MinGW, assuming it is
    in `C:\MinGW`
  * MSVC: TODO FIXME
  * Finally, there's a `CMakeLists.txt` file for portable cross-platform
    building. You can use it to build on all systems with lots of
    configurations (Ninja/makefiles on Windows/Lin/OSX, MSVC, etc.)

# Join in

File bugs in the GitHub [issue tracker][].

Master [git repository][gh]:

* `git clone https://github.com/thoughtpolice/minibsdiff.git`

There's also a [BitBucket mirror][bb]:

* `git clone https://bitbucket.org/thoughtpolice/minibsdiff.git`

# Authors

See [AUTHORS.txt](https://raw.github.com/thoughtpolice/minibsdiff/master/AUTHORS.txt).

# License

2-clause BSD. See `LICENSE.txt` for terms of copyright and redistribution.

[bsdiff]: http://www.daemonology.net/bsdiff/
[issue tracker]: http://github.com/thoughtpolice/minibsdiff/issues
[gh]: http://github.com/thoughtpolice/minibsdiff
[bb]: http://bitbucket.org/thoughtpolice/minibsdiff
