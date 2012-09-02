#ifndef _BSDIFF_H_
#define _BSDIFF_H_

typedef uint8_t u_char;

/* NOTA BENE: patch must be at least be 'newsize' in length! */
int bsdiff(u_char* old, off_t oldsize,
           u_char* new, off_t newsize,
           u_char* patch);

#endif /* _BSDIFF_H_ */
