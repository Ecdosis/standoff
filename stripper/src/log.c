#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>
#include "encoding.h"
#include "memwatch.h"
void initlog()
{
    FILE *fp = fopen("/tmp/stripper.log","w");
    if ( fp != NULL )
        fclose( fp );
}
void tmplog( const char *fmt, ... )
{
    UChar message[256];
    va_list l;
    va_start(l,fmt);
    u_vsnprintf( message, 255, fmt, l );
    va_end(l);
    FILE *fp = fopen("/tmp/stripper.log","a+");
    if ( fp != NULL )
    {
        int mlen = measure_to_encoding( message, u_strlen(message), "UTF-8");
        char *buf = malloc( mlen );
        convert_to_encoding( message, u_strlen(message), buf, mlen, "UTF-8" );
        fwrite(buf,1,strlen(buf),fp);
        fclose(fp);
    }
}

