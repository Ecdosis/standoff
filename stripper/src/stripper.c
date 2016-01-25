/**
 * Strip XML tags from a file and write them out in AESE format
 * Created on 22/10/2010
 * (c) Desmond Schmidt 2010
 */
/* This file is part of stripper.
 *
 *  stripper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <syslog.h>
#include <tidy/tidy.h>
#include <tidy/buffio.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>
#ifdef JNI
#include <jni.h>
#include "calliope_AeseStripper.h"
#include "ramfile.h"
#include "log.h"
#endif
#include "milestone.h"
#include "layer.h"
#include "range.h"
#include "dest_file.h"
#include "format.h"
#include "expat.h"
#include "stack.h"
#include "STIL.h"
#include "hashset.h"
#include "error.h"
#include "attribute.h"
#include "simplification.h"
#include "recipe.h"
#include "hashmap.h"
#include "hh_exceptions.h"
#include "userdata.h"
#include "utils.h"
#include "encoding.h"
#include "memwatch.h"

#define FILE_NAME_LEN 128
#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif
static UChar *U_ENGB;
static UChar *U_TEI;
static UChar *U_AMP;
static UChar *U_STIL;
/* this has to be public so we can initialise it in main */
struct format_struct
{
	const UChar *name;
	format_write_header hfunc;
	format_write_tail tfunc;
	format_write_range rfunc;
	const char *text_suffix;
	const char *markup_suffix;
	const char *middle_name;
};
typedef struct 
{
    userdata *user_data;
    /** source file */
    char src[FILE_NAME_LEN];
    /** source file minus suffix */
    char barefile[FILE_NAME_LEN];
    /** name of style */
    UChar *style;
    /** name of recipe file */
    char *recipe_file;
    /** if doing help or version info don't process anything */
    int doing_help;
    /** language code */
    UChar *language;
    /** hard-hyphen object */
    hh_exceptions *hh_except;
    /** copy of commandline arg */
    char *hh_except_string;
    /** the parser */
    XML_Parser parser;
    format *f;
} stripper;
/**
 * Copy an array of attributes as returned by expat
 * @param atts the attributes
 * @return a NULL terminated array, user must free
 */
static UChar **copy_atts( char **atts )
{
	int i = 0;
	UChar **new_atts;
	while ( atts[i] != NULL )
    {
        i += 2;
    }
    new_atts = calloc( i+2, sizeof(UChar*) );
    if ( new_atts != NULL )
    {
        i = 0;
        while ( atts[i] != NULL )
        {
            new_atts[i] = utf8toutf16(atts[i]);
            if ( new_atts[i] == NULL )
                fprintf( stderr,
                    "stripper: failed to allocate store for attribute key" );
            new_atts[i+1] = utf8toutf16( atts[i+1] );
            if ( new_atts[i+1] == NULL )
                fprintf( stderr,
                    "stripper: failed to allocate store for attribute value" );
            i += 2;
        }
    }
    return new_atts;
}
/**
 * Start element handler for XML file stripping.
 * @param userData the user data (optional)
 * @param name the name of the element
 * @param atts an array of attributes terminated by a NULL
 * pointer
 */
static void XMLCALL start_element_scan( void *userData,
	const char *name, const char **atts )
{
    range *r;
    userdata *u = userData;
    UChar **new_atts;
    int pushed = 0;
    UChar *u_name = utf8toutf16((char*)name);
    if ( u_name != NULL )
    {
        UChar *simple_name = u_name;
        if ( recipe_has_removal(userdata_rules(u),u_name) )
        {
            stack_push(userdata_ignoring(u),u_name);
            pushed = 1;
            // save on stack, free when popped
        }
        new_atts = copy_atts( (char**)atts );
        if ( stack_empty(userdata_ignoring(u)) 
            && recipe_has_rule(userdata_rules(u),u_name,new_atts) )
            simple_name = recipe_simplify( userdata_rules(u), u_name, new_atts );
        r = range_new( stack_empty(userdata_ignoring(u))?0:1,
            simple_name, new_atts, userdata_toffset(u) );
        if ( r != NULL )
        {
            // stack has to set length when we get to the range end
            stack_push( userdata_range_stack(u), r );
            // queue preserves the order of the start elements
            dest_file *df = userdata_get_markup_dest( u, range_get_name(r) );
            dest_file_enqueue( df, r );
        }
        if ( !pushed )
            free( u_name );
    }
}
/**
 * End element handler for XML split
 * @param userData (optional)
 * @param name name of element
 */
static void XMLCALL end_element_scan(void *userData, const char *name)
{
	userdata *u = userData;
	range *r = stack_pop( userdata_range_stack(u) );
    //printf("end: %s\n",name);
    int rlen = userdata_toffset(u)-range_get_start(r);
    range_set_len( r, rlen );
	UChar *u_name = utf8toutf16((char*)name);
    if ( u_name != NULL )
    {
        if ( !stack_empty(userdata_ignoring(u)) 
            && u_strcmp(stack_peek(userdata_ignoring(u)),u_name)==0 )
        {
            UChar *popped = stack_pop( userdata_ignoring(u) );
            if ( popped != NULL )
                free( popped );
        }
        free( u_name );
    }
}
/**
 * Is the given string just whitespace?
 * @param s the string (not null-terminated
 * @param len its length
 * @return 1 is only contains whitespace, else 0
 */
static int is_whitespace( const char *s, int len )
{
    int res = 1;
    int i;
    for ( i=0;i<len;i++ )
        if ( s[i]!='\t' && s[i]!='\n' && s[i]!=' ' )
            return 0;
    return res;
}
/**
 * Compute the length of the string in UTF-8
 * @param text the UTF-8 string
 * @param len its length in BYTES
 * @return its length in CHARS
static int utf8_len( XML_Char *text, int len )
{
     int i;
     int nchars = 0;
     for ( i=0;i<len;i++ )
     {
         unsigned char uc = (unsigned char) text[i];
         // these are just continuation bytes not real "characters"
         if ( uc < 0x80 || uc > 0xBF )
             nchars++;
     }
     return nchars;
}
 */
/**
 * trim leading and trailing white space down to 1 char
 * @param u the userdata struct
 * @param cptr VAR pointer to the string
 * @param len VAR pointer to its length
 */
static void trim( userdata *u, char **cptr, int *len )
{
    char *text = *cptr;
    int length = *len;
    int i;
    int state = userdata_last_char_type(u);
    // trim front of string
    for ( i=0;i<length;i++ )
    {
        switch ( state )
        {
            case 0: // last char was non-space/non-CR 
                if ( text[i] == ' ' || text[i] == '\t' )
                    state = 1;
                else if ( text[i] == '\r' )
                    state = 2;
                else if ( text[i] == '\n' )
                    state = 3;
                else
                    state = -1;
                break;
            case 1: // last char was a space
                if ( isspace(text[i]) )
                {
                    (*cptr)++;
                    (*len)--;
                    if ( text[i] == '\n' )
                        state = 3;
                    else if ( text[i] == '\r' )
                        state = 2;
                }
                else
                    state = -1;
                break;
            case 2: // last char was a CR
                if ( text[i] == '\n' )
                    state = 3;
                else if ( isspace(text[i]) )
                {
                    (*cptr)++;
                    (*len)--;
                }
                else
                    state = -1;
                break;
            case 3: // last char was a LF
                if ( isspace(text[i]) )
                {
                    (*cptr)++;
                    (*len)--;
                }
                else
                    state = -1;
                break;
            
        }
        if ( state < 0 )
            break;
    }
    // trim rear of string
    length = *len;
    text = *cptr;
    state = 0;
    for ( i=length-1;i>=0;i-- )
    {
        switch ( state )
        {
            case 0: // initial state or TEXT
                if ( text[i] == ' ' || text[i] == '\t' )
                    state = 1;
                else if ( text[i] == '\r' )
                    state = 2;
                else if ( text[i] == '\n' )
                    state = 3;
                else
                {
                    if ( text[i] == '-' )
                    {
                        int ulen = length;
                        userdata_update_last_word(u,text,length);
                        if ( strlen(userdata_last_word(u))>0 )
                        {
                            userdata_set_hoffset(u,userdata_toffset(u)+ulen-1);
                            userdata_set_hyphen_state(u,HYPHEN_ONLY);
                        }
                    }
                    else
                    {
                        userdata_clear_last_word(u);
                        userdata_set_hyphen_state(u,HYPHEN_NONE);
                    }
                    state = -1;
                    userdata_set_last_char_type(u,CHAR_TYPE_TEXT);
                }
                break;
            case 1: // last char was space
                if ( text[i] == ' ' || text[i] == '\t' )
                    (*len)--;
                else if ( text[i] == '\n' )
                {
                    (*len)--;
                    state = 3;
                }
                else if ( text[i] == '\r' )
                {
                    (*len)--;
                    state = 2;
                }
                else
                {
                    state = -1;
                    userdata_set_last_char_type(u,CHAR_TYPE_SPACE);
                }
                break;
            case 2: // last char was CR
                if ( isspace(text[i]) )
                {
                    (*len)--;
                }
                else
                {
                    if ( text[i] == '-' )
                    {
                        // remove trailing LF
                        (*len)--;
                        userdata_set_hyphen_state(u,HYPHEN_LF);
                        userdata_set_hoffset(u,userdata_toffset(u)
                            +length/*utf8_len(text,length)*/-2);
                        userdata_update_last_word(u,text,length);
                        userdata_set_last_char_type(u,CHAR_TYPE_TEXT);
                    }
                    else
                    {
                        userdata_clear_last_word(u);
                        userdata_set_last_char_type(u,CHAR_TYPE_CR);
                    }
                    state = -1;
                }
                break;
            case 3: // last char was LF
                if ( isspace(text[i]) )
                {
                    (*len)--;
                }
                else
                {
                    if ( text[i] == '-' )
                    {
                        // remove trailing LF
                        (*len)--;
                        userdata_set_hyphen_state(u,HYPHEN_LF);
                        userdata_set_hoffset(u,userdata_toffset(u)
                            +length/*utf8_len(text,length)*/-2);
                        userdata_update_last_word(u,text,length);
                        userdata_set_last_char_type(u,CHAR_TYPE_TEXT);
                    }
                    else
                    {
                        userdata_clear_last_word(u);
                        userdata_set_last_char_type(u,CHAR_TYPE_LF);
                    }
                    state = -1;
                }
                break;
        }
        if ( state == -1 )
            break;
    }
    if ( state != -1 && (*len)>0 )
        userdata_set_last_char_type(u,state);
}
/**
 * Duplicate the first word of a text fragment
 * @param text the text to pop the word off of
 * @param len its length
 * @return an allocated word in UTF8 or NULL
 */
static XML_Char *first_word( XML_Char *text, int len )
{
    int i;
    // point to first non-space
    for ( i=0;i<len;i++ )
    {
        if ( !isspace(text[i]) )
            break;
    }
    int j = i;
    for ( ;i<len;i++ )
    {
        if ( !isalpha(text[i])||text[i]=='-' )
            break;
    }
    char *word = calloc( 1+i-j, sizeof(XML_Char) );
    if ( word != NULL )
    {
        int nbytes = (i-j)*sizeof(XML_Char);
        memcpy( word, &text[j], nbytes );
    }
    return word;
}
/**
 * Combine two words by returning an allocated concatenation
 * @param w1 the first word
 * @param w2 the second word
 * @return the combined word, caller to dispose
 */
static XML_Char *combine_words( XML_Char *w1, XML_Char *w2 )
{
    XML_Char *w3 = calloc( strlen(w1)+strlen(w2)+1,1 );
    if ( w3 != NULL )
    {
        strcpy( w3, w1 );
        strcat( w3, w2 );
    }
    return w3;
}
/**
 * Add markup for the detected hyphen 
 * @param u the userdata
 * @param text the current text after the hyphen
 * @param len its length
 */
static void process_hyphen( userdata *u, XML_Char *text, int len )
{
    XML_Char *next = first_word(text,len);
    if ( next != NULL && strlen(next)>0 )
    {
        char *force = "weak";
        XML_Char *last = userdata_last_word(u);
        XML_Char *combined = combine_words(last,next);
        if ( (userdata_has_word(u,last)
            && userdata_has_word(u,next)
            && combined!=NULL
            && (!userdata_has_word(u,combined)
                ||userdata_has_hh_exception(u,combined)))
            || (strlen(next)>0&&isupper(next[0])) )
        {
            force = "strong";
            //printf("strong: %s-%s\n",last,next);
        }
        //else
        //    printf("weak: %s\n",combined);
        // create a range to describe a hard hyphen
        UChar **atts = calloc(1,sizeof(UChar*));
        if ( atts != NULL )
        {
            
            UChar *u_force = utf8toutf16(force);
            if ( u_force != NULL )
            {
               range *r = range_new( 0, u_force, atts, userdata_hoffset(u) );
                if ( r != NULL )
                { 
                    dest_file *df = userdata_get_markup_dest( u, u_force );
                    userdata_set_hyphen_state(u,HYPHEN_NONE);
                    range_set_len( r, 2 );  // hyphen+LF
                    dest_file_enqueue( df, r );
                }
                free( u_force );
            }
        }
        else
            fprintf(stderr,"stripper: failed to create hyphen range\n");
        if ( combined != NULL )
            free( combined );
    }
    if ( next != NULL )
        free( next );
}
/**
 * Handle characters during stripping. We basically write
 * everything to all the files currently identified by
 * current_bitset.
 * @param userData a userdata object or NULL
 * @param s the character data
 * @param its length
 */
static void XMLCALL charhndl( void *userData, const XML_Char *s, int len )
{
	userdata *u = userData;
	if ( stack_empty(userdata_ignoring(u)) )
	{
		size_t n;
        if ( len == 1 && s[0] == '&' )
        {
            n = dest_file_write( userdata_text_dest(u), U_AMP, u_strlen(U_AMP) );
            userdata_inc_toffset( u, u_strlen(U_AMP) );
            userdata_set_last_char_type(u,CHAR_TYPE_TEXT);
        }
        else 
	    {
            char *text = (char*)s;
            if ( userdata_hyphen_state(u) == HYPHEN_LF 
                && !is_whitespace(text,len) )
                process_hyphen(u,text,len);
            trim( u, &text, &len );
            if ( len == 1 && (text[0]=='\n'||text[0]=='\r') 
                && userdata_hyphen_state(u)==HYPHEN_ONLY )
            {
                userdata_set_hyphen_state(u,HYPHEN_LF);
                len=1;
            }
            if ( len > 0 )
            {
                //int ulen = utf8_len(text,len);
                UChar *u_text = utf8toutf16Len( text, len );
                if ( u_text != NULL )
                {
                    n = dest_file_write( userdata_text_dest(u), u_text, 
                        u_strlen(u_text) );
                    userdata_inc_toffset( u, u_strlen(u_text) );
                    //if ( ulen != len )
                    //    printf("ulen!=len\n");
                    if ( n <= 0 )
                        error( "stripper: write error on text file" );
                    free( u_text );
                }
            }
        }
    }
    else if ( !is_whitespace(s,len) )
    {
        range *r = stack_peek( userdata_range_stack(u) );
        if ( len == 1 && s[0] == '&' )
        {
			range_add_content( r, U_AMP, u_strlen(U_AMP) );
        }
        else
        {
            UChar *u_s = utf8toutf16Len((char*)s,len);
            if ( u_s != NULL )
            {
                range_add_content( r, u_s, u_strlen(u_s) );
                free( u_s );
            }
        }
    }
    // else it's inter-element white space
}
/**
 * Scan the source file, looking for tags to send to
 * the tags file and text to the text file.
 * @param buf the data to parse
 * @param len its length
 * @param s the stripper object
 * @return 1 if it succeeded, 0 otherwise
 */
static int scan_source( const char *buf, int len, stripper *s )
{
	int res = 1;
	userdata_set_last_char_type(s->user_data, CHAR_TYPE_LF);
    s->parser = XML_ParserCreate( NULL );
    if ( s->parser != NULL )
    {
        XML_SetElementHandler( s->parser, start_element_scan,
            end_element_scan );
        XML_SetCharacterDataHandler( s->parser, charhndl );
        XML_SetUserData( s->parser, s->user_data );
        if ( XML_Parse(s->parser,buf,len,1) == XML_STATUS_ERROR )
        {
            warning(
                "stripper: %s at line %" XML_FMT_INT_MOD "u\n",
                XML_ErrorString(XML_GetErrorCode(s->parser)),
                XML_GetCurrentLineNumber(s->parser));
            res = 0;
        }
        XML_ParserFree( s->parser );
    }
    else
    {
        fprintf(stderr,"stripper: failed to create parser\n");
        res = 0;
    }
	return res;
}
/**
 * Dispose of a stripper
 * @param s the stripper perhaps partly completed
 */
void stripper_dispose( stripper *s )
{
    if ( s != NULL )
    {
        if ( s->user_data != NULL )
            userdata_dispose( s->user_data );
        if ( s->hh_except_string != NULL )
            free( s->hh_except_string );
        if ( s->hh_except != NULL )
            hh_exceptions_dispose( s->hh_except );
        if ( s->style != NULL && s->style != U_TEI )
            free( s->style );
        if ( s->language != U_ENGB && s->language != NULL )
            free( s->language );
        if ( s->f != NULL )
            free( s->f );
        free( s );
    }
    if ( U_TEI != NULL )
        free( U_TEI );
    if ( U_AMP != NULL )
        free( U_AMP );
    if ( U_ENGB != NULL )
        free( U_ENGB );
    if ( U_STIL != NULL )
        free( U_STIL );
}
/**
 * Create a stripper object. We need this for thread safety
 * @return  a finished stripper object
 */
stripper *stripper_create()
{
    stripper *s = calloc( 1, sizeof(stripper) );
    if ( s != NULL )
    {
        U_ENGB = utf8toutf16("en_GB");
        U_TEI = utf8toutf16("TEI");
        U_AMP = utf8toutf16("&amp;");
        U_STIL = utf8toutf16("STIL");
        /** array of available formats - add more here */
        format *f = calloc( 1, sizeof(format) );
        s->f = f;
        f->name = U_STIL;
        f->hfunc = STIL_write_header;
        f->tfunc=STIL_write_tail;
        f->rfunc=STIL_write_range;
        f->text_suffix=".txt";
        f->markup_suffix=".json";
        f->middle_name="-stil";
        s->language = U_ENGB;
        s->style = U_TEI;
    }
    else
        fprintf(stderr,"stripper: failed to allocate object\n");
    return s;
}
/**
 * Convert html to XML
 * @param input the HTML input
 * @param len VAR param update with length of xhtml string
 * @return the allocated XML output
 */
static char *html2xhtml( const char *input, int *len )
{
  TidyBuffer output = {0};
  int rc = -1;
  int ok;
  char *out = NULL;

  TidyDoc tdoc = tidyCreate();
  ok = tidyOptSetBool( tdoc, TidyXhtmlOut, yes ); 
  if ( ok )
    rc = tidyParseString( tdoc, input );
  if ( rc >= 0 )
    rc = tidyCleanAndRepair( tdoc );
  if ( rc > 1 )   
    rc = ( tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1 );
  if ( rc >= 0 )
    rc = tidySaveBuffer( tdoc, &output );  
  if ( rc >= 0 )
  {
     out = malloc( output.size+1 );
     if ( out != NULL )
     {
         memcpy( out, output.bp, output.size );
         out[output.size] = 0;
         //printf("%s\n",&out[output.size-100]);
         *len = output.size;
     }
  }
  else
      fprintf(stderr,"tidy failed\n");
  tidyBufFree( &output );
  if ( tdoc != NULL )
      tidyRelease( tdoc );
  return out;
}
format_write_range format_rfunc(format *f)
{
    return f->rfunc;
}
format_write_tail format_tfunc(format *f)
{
    return f->tfunc;
}
format_write_header format_hfunc(format *f)
{
    return f->hfunc;
}
const char *format_text_suffix(format *f)
{
    return f->text_suffix;
}
const char *format_markup_suffix(format *f)
{
    return f->markup_suffix;
}
const char *format_middle_name(format *f)
{
    return f->middle_name;
}
#ifdef JNI
static void unload_string( JNIEnv *env, jstring jstr, const jchar *ustr, 
    jboolean copied )
{
    if ( copied )
        (*env)->ReleaseStringChars( env, jstr, ustr );
}
static void unload_string_c( JNIEnv *env, jstring jstr, const char *cstr, 
    jboolean copied )
{
    if ( copied )
        (*env)->ReleaseStringUTFChars( env, jstr, cstr );
}
static const jchar *load_string( JNIEnv *env, jstring jstr, jboolean *copied )
{
    return (*env)->GetStringChars(env, jstr, copied);  
}
static const char *load_string_c( JNIEnv *env, jstring jstr, jboolean *copied )
{
    return (*env)->GetStringUTFChars(env, jstr, copied);  
}
/*
 * Class:     calliope_AeseStripper
 * Method:    strip
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZLcalliope/json/JSONResponse;Lcalliope/json/JSONResponse;)I
 */
JNIEXPORT jint JNICALL Java_calliope_AeseStripper_strip
  (JNIEnv *env, jobject obj, jstring xml, jstring rules, 
    jstring style, jstring language, jstring hexcepts, jboolean is_html,
    jobject text, jobject markup)
{
	int res = 1;
    jboolean x_copied,r_copied=JNI_FALSE,s_copied,h_copied,l_copied=JNI_FALSE;
    const char *x_str = load_string_c( env, xml, &x_copied );
    //fprintf(stderr,"x_str=%s r_str\n",x_str);
    const char *r_str = (rules!=NULL)?load_string_c(env,rules,&r_copied):NULL;
    //fprintf(stderr,"f_str=%s\n",f_str);
    const jchar *s_str = load_string( env, style, &s_copied );
    //fprintf(stderr,"s_str=%s\n",s_str);
    const jchar *l_str = (language==NULL)?U_ENGB
        :load_string( env, language, &l_copied );
    //fprintf(stderr,"l_str=%s\n",l_str);
    const char *h_str = (hexcepts==NULL)?NULL
        :load_string_c( env, hexcepts, &h_copied );
    //fprintf(stderr,"h_str=%s\n",h_str);
    char *xhtml = NULL;
    stripper *s = stripper_create();
    if ( s != NULL )
    {
        recipe *ruleset;
        s->hh_except_string = (char*)h_str;
        // load or initialise rule set
        if ( rules == NULL )
            ruleset = recipe_new();
        else
            ruleset = recipe_load(r_str,strlen(r_str));
        hh_exceptions *hhe = hh_exceptions_create( s->hh_except_string );
        if ( hhe != NULL )
        {
            s->user_data = userdata_create( s->language, s->barefile, 
                ruleset, s->f, hhe );
            if ( s->user_data != NULL )
            {
                // write header
                int i=0;
                while ( res && userdata_markup_dest(s->user_data,i)!= NULL )
                {
                    res = s->f->hfunc( NULL, 
                        userdata_markup_dest(s->user_data,i), 
                        (jchar*)s_str );
                    i++;
                }
                // parse XML
                if ( res )
                {
                    int xlen = strlen( x_str );
                    if ( is_html )
                    {
                        xhtml = html2xhtml( x_str, &xlen );
                        if ( xhtml != NULL )
                        {
                            unload_string_c( env, xml, x_str, x_copied );
                            x_copied = JNI_FALSE;
                            x_str = xhtml;
                        }
                    }
                    res = scan_source( x_str, xlen, s );
                    if ( res )
                        userdata_write_files( env, s->user_data, text, markup );
                }
                else
                    tmplog("write header failed\n");
            }
        }
        stripper_dispose( s );
        if ( xhtml != NULL )
            free( xhtml );
        unload_string_c( env, xml, x_str, x_copied );
        unload_string_c( env, rules, r_str, r_copied );
        unload_string( env, style, s_str, s_copied );
        unload_string( env, language, l_str, l_copied );
        if ( h_str != NULL )
            unload_string_c( env, hexcepts, h_str, h_copied );
    }
    return res;
}
#elif COMMANDLINE
/**
 * Get the file length of the src file
 * @return its length as an int
 */
static int file_size( const char *file_name )
{
    FILE *fp = fopen( file_name, "r" );
    long sz = -1;
    if ( fp != NULL )
    {
        fseek(fp, 0L, SEEK_END);
        sz = ftell(fp);
        fclose( fp );
    }
    return (int) sz;
}
/**
 * Read a file contents 
 * @param file_name the name of the file to read
 * @param len its length once entirely read
 * @return an allocated buffer. caller MUST dispose
 */
static const char *read_file( const char *file_name, int *len )
{
    char *buf = NULL;
    *len = file_size(file_name);
    if ( *len > 0 )
    {
        FILE *src_file = fopen( file_name, "r" );
        int flen;
        if ( src_file != NULL )
        {
            buf = calloc( 1, (*len)+1 );
            if ( buf != NULL )
            {
                flen = (int)fread( buf, 1, *len, src_file );
                if ( flen != *len )
                    fprintf(stderr,"couldn't read %s\n",file_name);
            }
            else
                fprintf(stderr,"couldn't allocate buf\n");
            fclose( src_file );
            
        }
        else
            fprintf(stderr,"failed to open %s\n",file_name );
    }
    return buf;
}
/**
 * Print a simple help message. If we get time we can
 * make a man page later.
 */
static void print_help()
{
	printf(
		"usage: stripper [-h] [-v] [-s style] [-l] [-f format] "
        "[-r recipe] XML-file\n"
		"stripper removes tags from an XML file and saves "
			"them to a separate file\n"
		"in a standoff markup format. The original text is "
			"also preserved and is\n"
		"written to another file. Options are: \n"
		"-h print this help message\n"
		"-v print the version information\n"
        "-s style Specify a style name (default \"TEI\")\n"
		"-l language a two-char language code or variant like en_GB\n"
		"-e hh_exceptions ensure these space-delimited compound words ARE "
        "hyphenated\nIF both halves are words and the compound is also, e.g. safeguard\n"
		"-r recipe-file specifying removals and simplifications in XML or JSON\n"
		"XML-file the only real argument is the name of an XML "
			"file to split.\n");
}
/**
 * Check whether a file exists
 * @param file the file to test
 * @return 1 if it does, 0 otherwise
 */
static int file_exists( const char *file )
{
	FILE *EXISTS = fopen( file,"r" );
	if ( EXISTS )
	{
		fclose( EXISTS );
		return 1;
	}
	return 0;
}
/**
 * Check the commandline arguments
 * @param argc number of commandline args+1
 * @param argv array of arguments, first is program name
 * @param s the stripper object containing local vars
 * @return 1 if they were OK, 0 otherwise
 */
static int check_args( int argc, char **argv, stripper *s )
{
	char *dot_pos;
	int sane = 1;
	if ( argc < 2 )
		sane = 0;
	else
	{
		int i;
        for ( i=1;i<argc;i++ )
		{
			if ( strlen(argv[i])==2 && argv[i][0]=='-' )
			{
				switch ( argv[i][1] )
				{
					case 'v':
						printf( "stripper version 2.0 (c) "
								"Desmond Schmidt 2015\n");
						s->doing_help = 1;
						break;
					case 'h':
						print_help();
						s->doing_help = 1;
						break;
					case 'r':
                        s->recipe_file = argv[i+1];
                        break;
                    case 's':
                        s->style = utf8toutf16(argv[i+1]);
                        break;
                    case 'e':
                        s->hh_except_string = strdup(argv[i+1]);
                        break;
                    case 'l':
                        s->language = utf8toutf16(argv[i+1]);
                        break;
				}
			}
			if ( !sane )
				break;
		}
		if ( !s->doing_help )
		{
			strncpy( s->src, argv[argc-1], FILE_NAME_LEN );
			sane = file_exists( s->src );
			if ( !sane )
				fprintf(stderr,"stripper: can't find file %s\n",s->src );
            else
            {
                strncpy(s->barefile, s->src, FILE_NAME_LEN );
                dot_pos = strrchr( s->barefile, '.' );
                if ( dot_pos != NULL )
                    dot_pos[0] = 0;
            }
		}
        if ( s->language == NULL )
            sane = 0;
	}
	return sane;
}
/**
 * Tell them how to use the program.
 */
static void usage()
{
	printf( "usage: stripper [-h] [-v] [-l lang] [-s style] "
        "[-r recipe] [-e hh_exceptions] XML-file\n" );
}
/**
 * The main entry point
 * @param argc number of commandline args+1
 * @param argv array of arguments, first is program name
 * @return 0 to the system
 */
int main( int argc, char **argv )
{
    stripper *s = stripper_create();
    if ( s != NULL )
    {
        int res = 1;
        if ( check_args(argc,argv,s) )
		{
            recipe *rules;
            if ( s->recipe_file == NULL )
                rules = recipe_new();
            else
            {
                int rlen;
                const char *rdata = read_file( s->recipe_file, &rlen );
                if ( rdata != NULL )
                {
                    rules = recipe_load(rdata, rlen);
                    free( (char*)rdata );
                }
            }
            if ( rules != NULL )
            {
                s->hh_except = hh_exceptions_create( s->hh_except_string );
                s->user_data = userdata_create( s->language, s->barefile, 
                    rules, s->f, s->hh_except );
                if ( s->user_data == NULL )
                {
                    fprintf(stderr,"stripper: failed to initialise userdata\n");
                    res = 0;
                }
                if ( res && !s->doing_help )
                {
                    int i=0;
                    userdata *u = s->user_data;
                    while ( userdata_markup_dest(u,i) )
                    {
                        res = s->f->hfunc( NULL, 
                            userdata_markup_dest(u,i), s->style );
                        i++;
                    }
                    // parse XML, prepare body for writing
                    if ( res )
                    {
                        int len;
                        const char *data = read_file( s->src, &len );
                        if ( data != NULL )
                        {
                            if ( strstr(s->src,".html") )
                            {
                                char *xhtml = html2xhtml( data, &len );
                                if ( xhtml != NULL )
                                {
                                    free( (char*)data );
                                    data = xhtml;
                                }
                            }
                            res = scan_source( data, len, s );
                            if ( !res )
                                fprintf(stderr,"scan failed\n");
                            free( (char*)data );
                        }
                    }
                }
                // save the files in a separate step
                userdata_write_files( s->user_data );
            }
        }
        else
            usage();
        stripper_dispose( s );
    }
	return 0;
}
#endif
