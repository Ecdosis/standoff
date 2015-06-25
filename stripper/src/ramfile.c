#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>
#include "ramfile.h"
#include "error.h"
#include "utils.h"
#include "memwatch.h"
#define BLOCK_SIZE 8096
#define PRINT_LIMIT 32024
static UChar buf[PRINT_LIMIT];
struct ramfile_struct
{
    int allocated;
    UChar *buf;
    char *name;
    int used;
};
/**
 * Create a ramfile - a file-like holder of a string buffer in memory
 * @param name the name of the buffer
 * @return an initialised ramfile object
 */
ramfile *ramfile_create( const char *name )
{
    ramfile *rf = calloc( 1, sizeof(ramfile) );
    if ( rf != NULL )
    {
        rf->buf = calloc( BLOCK_SIZE, sizeof(UChar) );
        if ( rf->buf == NULL )
        {
            free( rf );
            rf = NULL;
        }
        else
        {
            char *hyphen_pos = strrchr(name,'-');
            rf->allocated = BLOCK_SIZE;
            rf->used = 0;
            if ( hyphen_pos != NULL && strlen(hyphen_pos+1) > 0 )
                name = hyphen_pos+1;
            rf->name = strdup((char*)name);
        }
    }
	return rf;
}
/**
 * Dispose of a ramfile
 */
void ramfile_dispose( ramfile *rf )
{
    if ( rf->buf != NULL )
        free( rf->buf );
    if ( rf->name != NULL )
        free( rf->name );
    free( rf );
}
/**
 * Write some data to a ramfile
 * @param rf the ramfile in question
 * @param data the data to write
 * @param len the length of the data 
 * @return nchars if it worked, else 0
 */
int ramfile_write( ramfile *rf, const UChar *data, int len )
{
    if ( len+rf->used >= rf->allocated )
    {
        int new_size = len+rf->used+BLOCK_SIZE;
        UChar *tmp = calloc( new_size, sizeof(UChar) );
        if ( tmp != NULL )
        {
            u_strncpy( tmp, rf->buf, rf->used );
            rf->allocated = new_size;
            tmp[rf->used] = 0;
            free( rf->buf );
            rf->buf = tmp;
        }
        else
        {
            error("ramfile: failed to reallocate ramfile buffer\n");
            return 0;
        }  
    }
    u_strncpy( &rf->buf[rf->used], data, len );
    rf->used += len;
    rf->buf[rf->used] = 0;
    return len;
}
/**
 * Equivalent of printf. Works up to PRINT_LIMIT bytes
 * @param rf the ramfile in question
 * @param fmt the printf format string
 * ... the other args
 * @return 1 if it worked, else 0
 */
int ramfile_print( ramfile *rf, const char *fmt, ... )
{
    int slen,res = 1;
    va_list ap;
    va_start( ap, fmt );
    u_vsnprintf( buf, PRINT_LIMIT, fmt, ap );
    slen = u_strlen(buf);
    res = ramfile_write( rf, buf, slen );
    va_end( ap );
    return res;
}
/**
 * Get the NULL-terminated string buffer
 * @param rf the ramfile in question
 * @return a C-string
 */
UChar *ramfile_get_buf( ramfile *rf )
{
    return rf->buf;
}
/**
 * Get the length of the string stored here.
 * @param rf the ramfile in question
 * @return the length of the used part of the buffer
 */
int ramfile_get_len( ramfile *rf )
{
    return rf->used;
}
char *ramfile_get_name( ramfile *rf )
{
    return rf->name;
}
