#ifndef _MINIBSDIFF_H_
#define _MINIBSDIFF_H_

#ifndef _MINIBSDIFF_U_CHAR_T_
#define _MINIBSDIFF_U_CHAR_T_
typedef uint8_t u_char;
#endif

/* NOTA BENE: patch must be at least be 'newsize' in length! */
int bsdiff(u_char* old, off_t oldsize,
           u_char* new, off_t newsize,
           u_char* patch);

#endif /* _MINIBSDIFF_H_ */
