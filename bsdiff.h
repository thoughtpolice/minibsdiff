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
#ifndef _MINIBSDIFF_H_
#define _MINIBSDIFF_H_

#include "minibsdiff-config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- */
/* -- Public API ----------------------------------------------------------- */

/*-
 * Determine the maximum size of a patch between two files. This function
 * should be used to allocate a buffer big enough for `bsdiff` to store
 * its output in.
 */
off_t bsdiff_patchsize_max(off_t oldsize, off_t newsize);

/*-
 * Create a binary patch from the buffers pointed to by oldp and newp (with
 * respective sizes,) and store the result in the buffer pointed to by 'patch'.
 *
 * The input pointer 'patch' must not be NULL, and the size of the buffer must
 * be at least 'bsdiff_patchsize_max(new,old)' in length.
 *
 * Returns -1 if `patch` is NULL, the 'patch' buffer is not large enough, or if
 * memory cannot be allocated.
 * Otherwise, the return value is the size of the patch that was put in the
 * 'patch' buffer.
 *
 * This function is memory-intensive, and requires max(17*n,9*n+m)+O(1) bytes
 * of memory, where n is the size of the new file and m is the size of the old
 * file. It runs in O((n+m) log n) time.
 */
int bsdiff(u_char* oldp, off_t oldsize,
           u_char* newp, off_t newsize,
           u_char* patch, off_t patchsize);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MINIBSDIFF_H_ */
