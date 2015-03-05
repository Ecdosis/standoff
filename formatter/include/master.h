/* 
 * File:   master.h
 * Author: desmond
 *
 * Created on 19 November 2011, 9:20 AM
 */
#ifndef MASTER_H
#define	MASTER
#ifdef	__cplusplus
extern "C" {
#endif
typedef struct master_struct master;
typedef enum { UNKNOWN, HTML, XML, Markdown } output_fmt_type; 

master *master_create( char *text, int len, output_fmt_type output_format );
void master_dispose( master *hf );
int master_load_markup( master *hf, const char *markup, int mlen, 
    const char *fmt );
int master_load_css( master *hf, const char *css, int len );
char *master_convert( master *hf );
int master_get_output_len( master *hf );
char *master_list( char *buf, int len );

#ifdef	__cplusplus
}
#endif
#endif	/* MASTER_H */
