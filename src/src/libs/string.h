#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>
 
void* memset( void* s, int c, size_t n ) {
    char* _src;

    _src = ( char* )s;

    while ( n-- ) {
        *_src++ = c;
    }

    return s;
}

void* memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

char* strcpy( char* d, const char* s ) {
    char* tmp = d;

    while ( ( *d++ = *s++ ) != 0 ) ;

    return tmp;
}
 
#endif