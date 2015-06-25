/* 
 * File:   ramfile.h
 * Author: desmond
 *
 * Created on 3 December 2011, 3:31 PM
 */

#ifndef RAMFILE_H
#define	RAMFILE_H

#ifdef	__cplusplus
extern "C" {
#endif
typedef struct ramfile_struct ramfile;
ramfile *ramfile_create( const char *name);
void ramfile_dispose( ramfile *rf );
int ramfile_write( ramfile *rf, const UChar *data, int len );
int ramfile_print( ramfile *rf, const char *fmt, ... );
UChar *ramfile_get_buf( ramfile *rf );
int ramfile_get_len( ramfile *rf );
char *ramfile_get_name( ramfile *rf );

#ifdef	__cplusplus
}
#endif

#endif	/* RAMFILE_H */

