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
#ifndef _MINIBSDIFF_CONFIG_H_
#define _MINIBSDIFF_CONFIG_H_

#ifdef _MSC_VER
#include <Windows.h>
#include "stdint-msvc.h"
#include "stdbool-msvc.h"
#else
#include <stdint.h>
#include <stdbool.h>
#endif /* _MSC_VER */

/* ------------------------------------------------------------------------- */
/* -- Patch file magic number ---------------------------------------------- */

/** MUST be 8 bytes long! */
/** TODO FIXME: we should static_assert this */
#define BSDIFF_CONFIG_MAGIC "MBSDIF43"

/* ------------------------------------------------------------------------- */
/* -- Slop size for temporary patch buffer --------------------------------- */

#define BSDIFF_PATCH_SLOP_SIZE 102400

/* ------------------------------------------------------------------------- */
/* -- Type definitions ----------------------------------------------------- */

/* Duplicated to keep code small. Keep in sync with bspatch.h! */
#ifndef _MINIBSDIFF_U_CHAR_T_
#define _MINIBSDIFF_U_CHAR_T_
typedef uint8_t u_char;
#endif /* _MINIBSDIFF_U_CHAR_T_ */

#ifdef _MSC_VER
typedef SSIZE_T ssize_t;
#endif /* _MSC_VER */

#endif /* _MINIBSDIFF_CONFIG_H_ */
