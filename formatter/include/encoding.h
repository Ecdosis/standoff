/* 
 * File:   encoding.h
 * Author: desmond
 *
 * Created on February 15, 2013, 2:07 PM
 */

#ifndef ENCODING_H
#define	ENCODING_H

#ifdef	__cplusplus
extern "C" {
#endif
char *utf16toutf8Len( UChar *str, int *u_len );
char *utf16toutf8( UChar *str );
UChar *utf8toutf16( char *str );
UChar *utf8toutf16Len( char *str, int s_len );
int convert_from_encoding( char *src, int srclen, UChar *dst, 
    int dstlen, char *charset );
int convert_to_encoding( UChar *src, int srclen, char *dst, 
    int dstlen, char *charset );
int measure_to_encoding( UChar *src, size_t srclen, char *encoding );
int measure_from_encoding( char *src, size_t srclen, char *encoding );
#ifdef MVD_TEST
void test_encoding( int *passed, int *failed );
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* ENCODING_H */

