#ifndef _BSPATCH_H_
#define _BSPATCH_H_

typedef uint8_t u_char;

bool valid_header(u_char* patchf);
ssize_t bspatch_newsize(u_char* patch);
int bspatch(u_char* oldf,   ssize_t   oldf_sz,
            u_char* patchf, ssize_t patchf_sz,
            u_char* newf);

#endif /* _BSPATCH_H_ */
