#ifndef _MINIBSPATCH_H_
#define _MINIBSPATCH_H_

#ifndef _MINIBSDIFF_U_CHAR_T_
#define _MINIBSDIFF_U_CHAR_T_
typedef uint8_t u_char;
#endif

bool valid_header(u_char* patchf);
ssize_t bspatch_newsize(u_char* patch);
int bspatch(u_char* oldf,   ssize_t   oldf_sz,
            u_char* patchf, ssize_t patchf_sz,
            u_char* newf);

#endif /* _MINIBSPATCH_H_ */
