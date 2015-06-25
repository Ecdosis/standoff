/*
 * STIL.c
 *
 *  Created on: 13/11/2011
 * (c) Desmond Schmidt 2011
 */
/* This file is part of stripper.
 *
 *  stripper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  stripper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with stripper.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include "ramfile.h"
#include "milestone.h"
#include "layer.h"
#include "range.h"
#include "dest_file.h"
#include "format.h"
#include "STIL.h"
#include "error.h"
#include "encoding.h"
#include "stack.h"
#include "utils.h"
#include "memwatch.h"
static UChar *U_COMMA;
static UChar *U_COMMANL;
static UChar *U_LF;
static UChar *U_HEAD;
static UChar *U_TAIL;
static UChar *U_STYLE;
static UChar *U_RANGES_START;
static UChar *U_RANGE_START;
static UChar *U_RANGE_END;
static UChar *U_NAME;
static UChar *U_RELOFF;
static UChar *U_REMOVED;
static UChar *U_LEN;
static UChar *U_CONTENT;
static UChar *U_ANNOTATIONS;
static UChar *U_ANNOTATIONS_END;
static UChar *U_ANNOTATION;
static UChar *U_ANNOTATION_END;
static UChar *U_TRUE;
static UChar *U_COLON;
static int write_utf16_array( UChar **array, int len, dest_file *df )
{
    int i;
    int res = 1;
    for ( i=0;i<len;i++ )
    {
        if ( array[i] != NULL )
        {
            int u_len = u_strlen(array[i]);
            int n = dest_file_write( df, array[i], u_len );
            if ( n != u_len )
            {
                break;
                res = 0;
            }
        }
        else
        {
            break;
            res = 0;
        }
    }
    return res;
}
/**
 * Escape and wrap a string in double quotes
 * @param src the string that needs escaping
 * @param len the length of the string
 * @return a reallocated string with escaped quotes
 */
static UChar *quote_string( UChar *src, int len )
{
    int i,j;
    int n_quotes = 0;
    for ( i=0;i<len;i++ )
        if ( src[i] == '"' )
            n_quotes++;
    UChar *dst = calloc( len+n_quotes+3,sizeof(UChar) );
    if ( dst != NULL )
    {
        dst[0] = '"';
        for ( j=1,i=0;i<len;i++ )
        {
            if ( src[i] == (UChar)'"' )
            {
                dst[j++] = (UChar)'\\';
                dst[j++] = (UChar)'"';
            }
            else
                dst[j++] = src[i];
        }
        dst[j++] = '"';
        dst[j] = 0;
    }
    else
        dst = src;
    return dst;
}
/**
 * Write the header information. 
 * @param arg ignored optional user param
 * @param style the name of the format style (for transformation)
 * @param dst the destination markup file handle
 * @return 1 if successful, 0 otherwise
 */
int STIL_write_header( void *arg, dest_file *dst, UChar *style )
{
    int res = 1;
    U_COMMA = utf8toutf16(",");
    U_COMMANL = utf8toutf16(",\n");
    U_LF = utf8toutf16("\n");
    U_HEAD = utf8toutf16( "{\n");
    U_TAIL = utf8toutf16( "\n  ]\n}\n");
    U_STYLE = utf8toutf16("  \"style\": ");
    U_RANGES_START = utf8toutf16("  \"ranges\": [\n");
	U_RANGE_START = utf8toutf16("  {\n");
    U_RANGE_END = utf8toutf16("  }");
    U_NAME = utf8toutf16("    \"name\": ");
    U_RELOFF = utf8toutf16("    \"reloff\": ");
    U_LEN = utf8toutf16("    \"len\": ");
    U_REMOVED = utf8toutf16("    \"removed\": ");
    U_CONTENT = utf8toutf16("    \"content\": ");
    U_ANNOTATIONS = utf8toutf16("    \"annotations\": [ ");
    U_ANNOTATIONS_END = utf8toutf16(" ]\n");
    U_ANNOTATION = utf8toutf16("{ ");
    U_ANNOTATION_END = utf8toutf16(" }");
    U_TRUE = utf8toutf16("true");
    U_COLON = utf8toutf16(": ");
    // write header
    UChar **items = calloc(5,sizeof(UChar*));
    if ( items != NULL )
    {
        items[0] = U_HEAD;
        items[1] = U_STYLE;
        items[2] = quote_string(style,u_strlen(style));
        items[3] = U_COMMANL;
        items[4] = U_RANGES_START;
        res = write_utf16_array( items, 5, dst );
        if ( items[2] != NULL )
            free( items[2] );
        free( items );
    }
    else
        res = 0;
	return res;
}
/**
 * Free and clear a UChar memory location
 * @param u_ptr a pointer to a UChar string
 */
static void forget_uchar( UChar **u_ptr )
{
    free( *u_ptr );
    *u_ptr = NULL;
}
/**
 * Write the tail
 */
int STIL_write_tail( void *arg, dest_file *df )
{
    int res = dest_file_write( df, U_TAIL, u_strlen(U_TAIL) );
    if ( U_COMMA != NULL ) forget_uchar(&U_COMMA);
    if ( U_COMMANL != NULL ) forget_uchar(&U_COMMANL);
    if ( U_LF != NULL ) forget_uchar(&U_LF);
    if ( U_HEAD != NULL ) forget_uchar(&U_HEAD);
    if ( U_TAIL != NULL ) forget_uchar(&U_TAIL);
    if ( U_STYLE != NULL ) forget_uchar(&U_STYLE);
    if ( U_RANGES_START != NULL ) forget_uchar(&U_RANGES_START);
    if ( U_RANGE_START != NULL ) forget_uchar(&U_RANGE_START);
    if ( U_RANGE_END != NULL ) forget_uchar(&U_RANGE_END);
    if ( U_NAME != NULL ) forget_uchar(&U_NAME);
    if ( U_RELOFF != NULL ) forget_uchar(&U_RELOFF);
    if ( U_LEN != NULL ) forget_uchar(&U_LEN);
    if ( U_CONTENT != NULL ) forget_uchar(&U_CONTENT);
    if ( U_ANNOTATIONS != NULL ) forget_uchar(&U_ANNOTATIONS);
    if ( U_ANNOTATIONS_END != NULL ) forget_uchar(&U_ANNOTATIONS_END);
    if ( U_ANNOTATION != NULL ) forget_uchar(&U_ANNOTATION);
    if ( U_ANNOTATION_END != NULL ) forget_uchar(&U_ANNOTATION_END);
    if ( U_REMOVED != NULL ) forget_uchar(&U_REMOVED);
    if ( U_TRUE != NULL ) forget_uchar(&U_TRUE);
    if ( U_COLON != NULL ) forget_uchar(&U_COLON);
    return res > 0;
}
/**
 * Work out the required size of the write array for a range
 * @param removed 1 if this range is removed
 * @param content_len the length of the content (or 0)
 * @param first 1 if this is the first range
 * @param atts the array of attributes (annotations on the range
 */
static int calc_range_size(int removed, int content_len, int first, UChar **atts)
{
    int size = 0;
    int i = 0;
    if ( !first )
        size++;
    size += 11;
    if ( content_len > 0 )
        size += 3;
    if ( removed )
        size += 3;
    if ( atts[0] != NULL )
        size += 2;
    while ( atts[i] != NULL )
    {
        size += 5;
        if ( i!= 0 )
            size++;
        i += 2;
    }
    return size;
}
/**
 * This will be called repeatedly
 * @param name the name of the range
 * @param atts a NULL-terminated array of XML attributes.
 * These get turned into STIL annotations
 * @param reloff relative offset for this range
 * @param len length of the range
 * @param dst the output file handle
 * @param contents the contents of an empty range
 * @param content_len length of the content
 * @param first 1 if this is the first range
 * @param dst the open file descriptor to write to
 */
int STIL_write_range( UChar *name, UChar **atts, int removed,
	int reloff, int len, UChar *contents, int content_len, int first,
    dest_file *dst )
{
	int res=1;
    stack *tofree = stack_create();
    if ( tofree != NULL )
    {
        int size = calc_range_size(removed,content_len,first,atts);
        UChar **write_array = calloc(size,sizeof(UChar*));
        if ( write_array != NULL )
        {
            int i = 0;
            UChar u_reloff[16];
            u_itoa(reloff,u_reloff,16);
            UChar u_len[16];
            UChar *q_name = quote_string(name,u_strlen(name));
            UChar *q_contents = quote_string(contents,content_len);
            stack_push(tofree,write_array);
            stack_push(tofree,q_name);
            stack_push(tofree,q_contents);
            u_itoa(len,u_len,16);
            if ( !first )
                write_array[i++] = U_COMMANL;
            write_array[i++] = U_RANGE_START;
            write_array[i++] = U_NAME;
            write_array[i++] = q_name;
            write_array[i++] = U_COMMANL;
            write_array[i++] = U_RELOFF;
            write_array[i++] = u_reloff;
            write_array[i++] = U_COMMANL;
            write_array[i++] = U_LEN;
            write_array[i++] = u_len;
            write_array[i++] = U_COMMANL;
            if ( content_len > 0 )
            {
                write_array[i++] = U_CONTENT;
                write_array[i++] = q_contents;
                write_array[i++] = U_COMMANL;
            }
            if ( removed )
            {
                write_array[i++] = U_REMOVED;
                write_array[i++] = U_TRUE;
                write_array[i++] = U_COMMANL;
            }
            if ( atts[0] != NULL )
            {
                int k = 0;
                write_array[i++] = U_ANNOTATIONS;
                while ( atts[k] != NULL )
                {
                    UChar *q_name = quote_string( atts[k], u_strlen(atts[k]) );
                    stack_push(tofree,q_name);
                    UChar *q_value = quote_string( atts[k+1], u_strlen(atts[k+1]) );
                    stack_push(tofree,q_value);
                    if ( k > 0 )
                        write_array[i++] = U_COMMA;
                    write_array[i++] = U_ANNOTATION;
                    write_array[i++] = q_name;
                    write_array[i++] = U_COLON;
                    write_array[i++] = q_value;
                    write_array[i++] = U_ANNOTATION_END;
                    k += 2;
                }
                write_array[i++] = U_ANNOTATIONS_END;
            }
            write_array[i++] = U_RANGE_END;
            write_utf16_array( write_array, size, dst );
            while ( !stack_empty(tofree) )
                free( stack_pop(tofree));
        }
        stack_delete(tofree);
    }
    else
    {
        fprintf(stderr,"STIL: Failed to allocate stack\n");
        res = 0;
    }
	return res;
}

