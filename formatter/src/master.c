#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "attribute.h"
#include "hashmap.h"
#include "annotation.h"
#include "range.h"
#include "range_array.h"
#include "hashset.h"
#include "master.h"
#include "formatter.h"
#include "AESE/AESE.h"
#include "STIL/STIL.h"
#include "error.h"

#include "memwatch.h"
#define ERROR_LEN 128
#define HTML_PRELIM "<html><body><p>"
#define HTML_TAIL "</p></body></html>"
#define XML_PRELIM "<TEI><text><body><p>"
#define XML_TAIL "</p></body></text></TEI>"
#define MARKDOWN_PRELIM "#"
#define MARKDOWN_TAIL ""
#define MARKDOWN_ERROR "#Error: conversion failed"
static format formats[]={{"AESE",load_aese_markup},{"STIL",load_stil_markup}};
static int num_formats = sizeof(formats)/sizeof(format);
struct master_struct
{
    char *text;
    int tlen;
    output_fmt_type output_format;
    int output_len;
    int has_css;
    int has_markup;
    int has_text;
    int selected_format;
    formatter *f;
    char error_string[ERROR_LEN];
};
/**
 * Create a aese formatter
 * @param text the text to format
 * @param len the length of the text
 * @param output_type the output format type (HTML etc)
 * @return an initialised master instance
 */
master *master_create( char *text, int len, output_fmt_type output_format )
{
    master *hf = calloc( 1, sizeof(master) );
    if ( hf != NULL )
    {
        hf->has_text = 0;
        hf->has_css = 0;
        hf->has_markup = 0;
        hf->output_format = output_format;
        if ( text != NULL )
        {
            hf->tlen = len;
            if ( hf->tlen > 0 )
            {
                hf->f = formatter_create( hf->tlen, output_format );
                hf->text = text;
                hf->has_text = 1;
            }
        }
        
    }
    else
        error("master: failed to allocate instance\n");
    return hf;
}
/**
 * Dispose of a aese formatter
 */
void master_dispose( master *hf )
{
    if ( hf->f != NULL )
        formatter_dispose( hf->f );
    free( hf );
}
/**
 * Look up a markup format in our list.
 * @param fmt_name the markup format's name
 * @return its index in the table or 0
 */
static int master_lookup_format( const char *fmt_name )
{
    int i;
    for ( i=0;i<num_formats;i++ )
    {
        if ( strcmp(formats[i].name,fmt_name)==0 )
            return i;
    }
    return 0;
}
/**
 * Load the markup file (possibly one of several)
 * @param hf the master in question
 * @param markup a markup string
 * @param mlen the length of the markup
 * @param fmt the format
 * return 1 if successful, else 0
 */
int master_load_markup( master *hf, const char *markup, int mlen, 
    const char *fmt )
{
    int res = 0;
    //fprintf(stderr,"mlen=%d markup=%s\n",mlen,markup);
    hf->selected_format = master_lookup_format( fmt );
    if ( hf->selected_format >= 0 )
    {
        res = formatter_load_markup( hf->f, 
            formats[hf->selected_format].lm, markup, mlen );
        if ( res && !hf->has_markup )
            hf->has_markup = 1;
    }
    return res;
}
/**
 * Load a css file
 * @param hf the master in question
 * @param css the css data
 * @param len its length
 * return 1 if successful, else 0
 */
int master_load_css( master *hf, const char *css, int len )
{
    int res = formatter_css_parse( hf->f, css, len );
    if ( res && !hf->has_css )
        hf->has_css = 1;
    return res;
}
/**
 * Format an error message for the target output format
 * @param hf the master object
 * @param prelim the leading codes
 * @param tail the trailing codes
 * @param error the string to write to
 * @param message the central message
 * @param len the length of error
 */
static void master_format_error( master *hf, char *message )
{
    switch ( hf->output_format )
    {
        case XML:
            snprintf(hf->error_string,ERROR_LEN, "Error: %s%s%s\n", 
                XML_PRELIM,message,XML_TAIL);
            break;
        case Markdown:
            snprintf(hf->error_string,ERROR_LEN, "Error: %s%s%s\n", 
                MARKDOWN_PRELIM,message, MARKDOWN_TAIL);
            break;
        default:
        case HTML:
            snprintf(hf->error_string,ERROR_LEN, "Error: %s%s%s\n", 
                HTML_PRELIM,message,HTML_TAIL);
            break;
    }
}
/**
 * Convert the specified text to the output format
 * @param hf the master in question
 * @return a string in the output format
 */
char *master_convert( master *hf )
{
    char *str = hf->error_string;
    if ( hf->has_text && hf->has_css && hf->has_markup )
    {
        if ( formatter_cull_ranges(hf->f,hf->text,&hf->tlen) )
        {
            int res = formatter_make_output( hf->f, hf->text, hf->tlen );
            if ( res )
                str = formatter_get_output( hf->f, &hf->output_len );
            else
            {
                master_format_error(hf,"Conversion failed");
                hf->output_len = strlen(hf->error_string);
            }
        }
        else
        {
            master_format_error(hf,"failed to remove ranges" );
        }
    }
    else
    {
        const char *hntext = (hf->has_text)?"":"no text ";
        const char *hnmarkup = (hf->has_markup)?"":"no markup ";
        const char *hncss = (hf->has_css)?"":"no css ";
        char error[ERROR_LEN];
        snprintf(error,ERROR_LEN, "%s%s%s", hntext,hnmarkup,hncss );
        master_format_error( hf, error );
    }
    return str;
}
/**
 * Get the length of the just processed html
 * @param hf the master in question
 * @return the html text length
 */
int master_get_output_len( master *hf )
{
    return hf->output_len;
}
/**
 * List the formats registered with the main program. If the user
 * defines another format he/she must call the register routine to
 * register it. Then this command returns a list of dynamically
 * registered formats.
 * @return the available format names
*/
char *master_list( char *buf, int len )
{
	int i;
    int left = len-1;
    buf[0] = 0;
	for ( i=0;i<num_formats;i++ )
	{
        strncat( buf, formats[i].name, left );
        left -= strlen(formats[i].name);
        strncat( buf, "\n", left );
        left--;
	}
}
