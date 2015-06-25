/* 
 * File:   dest_file.h
 * Author: desmond
 *
 * Created on August 11, 2012, 1:06 PM
 */

#ifndef DEST_FILE_H
#define	DEST_FILE_H

#ifdef	__cplusplus
extern "C" {
#endif
#ifdef JNI
#define DST_FILE ramfile
#define DST_WRITE(p,n,f) ramfile_write( f, p, n )
#define DST_PRINT(s,f,...) ramfile_print( s, f, __VA_ARGS__ )
#else
#define DST_FILE FILE
#define DST_WRITE(p,n,f) fwrite( p, 1, n, f )
#define DST_PRINT(s,f,...) fprintf( s, f, __VA_ARGS__ )
#endif

typedef struct dest_file_struct dest_file;
typedef enum {markup_kind,text_kind} dest_kind;
typedef struct format_struct format;
dest_file *dest_file_create( dest_kind kind, layer *l, char *midname, 
        char *name, format *f );
dest_file *dest_file_dispose( dest_file *df );
layer *dest_file_layer( dest_file *df );
DST_FILE *dest_file_dst( dest_file *df );
int dest_file_first( dest_file *df );
void dest_file_set_first( dest_file *df, int value );
int dest_file_reloff( dest_file *df, int absolute_off );
void dest_file_enqueue( dest_file *df, range *r );
int dest_file_open( dest_file *df );
int dest_file_close( dest_file *df, int tlen );
int dest_file_len( dest_file *df );
int dest_file_write( dest_file *df, UChar *data, int len );


#ifdef	__cplusplus
}
#endif

#endif	/* DEST_FILE_H */

