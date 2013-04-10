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
#ifndef _MINIBSPATCH_H_
#define _MINIBSPATCH_H_

#include "minibsdiff-config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- */
/* -- Public API ----------------------------------------------------------- */

/*-
 * Determine if the buffer pointed to by `patch` of a given `size` is
 * a valid patch.
 */
bool bspatch_valid_header(u_char* patch, ssize_t patchsz);

/*-
 * Determine the size of the new file that will result from applying
 * a patch. Returns -1 if the patch header is invalid, otherwise returns
 * the size of the new file.
 */
ssize_t bspatch_newsize(u_char* patch, ssize_t patchsize);

/*-
 * Apply a patch stored in 'patch' to 'oldp', result in 'newp', and store the
 * result in 'newp'.
 *
 * The input pointers must not be NULL.
 *
 * The size of 'newp', represented by 'newsz', must be at least
 * 'bspatch_newsize(oldsz,patchsz)' bytes in length.
 *
 * Returns -1 if memory can't be allocated, or the input pointers are NULL.
 * Returns -2 if the patch header is invalid. Returns -3 if the patch itself is
 * corrupt.
 * Otherwise, returns 0.
 *
 * This function requires n+m+O(1) bytes of memory, where n is the size of the
 * old file and m is the size of the new file. It does no allocations.
 * It runs in O(n+m) time.
 */
int bspatch(u_char* oldp,  ssize_t oldsz,
            u_char* patch, ssize_t patchsz,
            u_char* newp,  ssize_t newsz);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MINIBSPATCH_H_ */
