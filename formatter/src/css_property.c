/*
 * This file is part of formatter.
 *
 *  formatter is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  formatter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with formatter.  If not, see <http://www.gnu.org/licenses/>.
 *  (c) copyright Desmond Schmidt 2011
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "css_property.h"
#include "error.h"
#include "memwatch.h"

#define AESE_PREFIX "-aese-"
#define AESE_PREFIX_LEN 6
#define MD_ATTR "-md-attr-"
#define MD_ATTR_LEN 9
#define XML_ATTR "-xml-attr-"
#define XML_ATTR_LEN 10
#define MD_TAG "-md-tag-"
#define MD_TAG_LEN 8
#define XML_TAG "-xml-tag-"
#define XML_TAG_LEN 9

/**
 * A css property is a property like font-weight: bold. Except that we
 * extend it by prefixing "-aese-markup_name: output_name. This is for copying 
 * over attributes from markup properties to the output format.
 */
struct css_property_struct
{
	/* name of the markup-attribute */
	char *markup_name;
	/* if NULL then we only match not replace */
	char *output_name;
    /* property value to be set */
    char *output_value;
    CssPropertyType type;
};

/**
 * Delete a property and free its memory
 * @param p the property in question
 */
void css_property_dispose( css_property *p )
{
    if ( p->markup_name != NULL )
    {
        free( p->markup_name );
        p->markup_name = NULL;
    }
    if ( p->output_name != NULL )
    {
        free( p->output_name );
        p->output_name = NULL;
    }
    if ( p->output_value != NULL )
    {
        free( p->output_value );
        p->output_name = NULL;
    }
    free( p );
    p = NULL;
}
/**
 * Clone a property deeply
 * @param p the original one to copy
 * @return the clone
 */
css_property *css_property_clone( css_property *p )
{
    css_property *copy = calloc( 1, sizeof(css_property) );
    if ( copy == NULL )
        error( "css_property: failed to duplicate property struct\n");
    else
    {
        if ( p->markup_name != NULL )
        {
            copy->markup_name = strdup(p->markup_name);
            if ( copy->markup_name == NULL )
                error("css_property: failed to duplicate markup_name "
                    "field during clone\n");
        }
        if ( p->output_name != NULL )
        {
            copy->output_name = strdup(p->output_name);
            if ( copy->output_name == NULL )
                error("css_property: failed to duplicate markup_name "
                    "field during clone\n");
        }
        if ( p->output_value != NULL )
        {
            copy->output_value = strdup(p->output_value);
            if ( copy->output_value == NULL )
                error("css_property: failed to duplicate output_value "
                    "field during clone\n");
        }
        copy->type = p->type;
    }
    return copy;
}
/**
 * Get the property's output name
 * @param p the property in question
 * @return a string being the output attribute name
 */
char *css_property_get_output_name( css_property *p )
{
    return p->output_name;
}
/**
 * Get the property's markup name
 * @param p the property in question
 * @return a string being the markup attribute name
 */
char *css_property_get_markup_name( css_property *p )
{
    return p->markup_name;
}/**
 * Get the output property value
 * @param p the property in question
 * @return a string being the property value
 */
char *css_property_get_output_value( css_property *p )
{
    return p->output_value;
}
/**
 * Remove all instances of the given char from the string in situ
 * @param str the string to remove it from
 * @param c the char to remove
 */
static void strip_char( char *str, char c )
{
    int i = 0;
    int j = 0;
    while ( str[i] != 0 )
    {
        if ( str[i] != c )
        {
            if ( i > j )
                str[j] = str[i];
            j++;
        }
        i++;
    }
    str[j] = 0;
}
static css_property *parse_custom_property( char *data, int start, int end,
    CssPropertyType type )
{
    // the property name had an escaped ":"
	int escaped = 0;
    int i = start;
    css_property *prop_temp = NULL;
    while ( i < end )
    {
        if ( data[i]=='\\' )
        {
            escaped = 1;
            i+=2;
        }
        else if ( data[i] == ':' )
        {
            // parse left hand side
            prop_temp = calloc( 1, sizeof(css_property) );
            if ( prop_temp != NULL )
            {
                int lhs_len = i-start;
                prop_temp->type = type;
                prop_temp->markup_name = calloc( 1, lhs_len+1 );
                if ( prop_temp->markup_name != NULL )
                {
                    strncpy( prop_temp->markup_name, &data[start], lhs_len );
                    if ( escaped )
                        strip_char(prop_temp->markup_name,'\\');
                    // parse right hand side
                    i++;
                    while ( i<end && isspace(data[i]) )
                        i++;
                    while ( end>i && isspace(data[end]) )
                        end--;
                    if ( i < end )
                    {
                        int rhs_len = (end-i)+1;
                        prop_temp->output_name = calloc( 1, rhs_len+1 );
                        if ( prop_temp->output_name != NULL )
                        {
                            strncpy( prop_temp->output_name, &data[i], 
                                rhs_len );
                            break;
                        }
                        else
                        {
                            css_property_dispose( prop_temp );
                            warning("css_property: failed to allocate"
                                "output name\n");
                            prop_temp = NULL;
                        }
                    }
                    else
                    {
                        warning("css_property: missing output name\n");
                        css_property_dispose( prop_temp );
                        prop_temp = NULL;
                    }
                }
                else
                {
                    css_property_dispose( prop_temp );
                    warning( "css_property: failed to allocate markup_name\n");
                    prop_temp = NULL;
                }
            }
            else
            {
                warning("css_property: failed to allocate css_property\n");
            }
        }
        i++;
    }
    return prop_temp;
}
/**
 * Parse a single property from the raw CSS data. Ignore any property
 * not beginning with "-aese-, -md-, -html-, or -xml-". Such properties specify
 * extensions for the translation phase. Such properties are supposed to be
 * ignored by browser-based css parsers
 * @param data the raw CSS data read from the file
 * @param len the length of the property in data
 * @return an allocated css_property (caller must eventually dispose it)
 */
css_property *css_property_parse( const char *data, int len )
{
    // format: 
    // -aese-markup_name: output_name;
    // [copy the attribute value over from the markup unchanged (not here)]
	// -md-attr-name: value
    // -xml-attr-name: value
    // -xml-tag: name
    // -md-tag: name
    int start=0, end=len;
    css_property *prop_temp = NULL;
    while ( start<end && isspace(data[start]) )
    {
		start++;
        end--;
    }
	if ( strncmp(&data[start],AESE_PREFIX,AESE_PREFIX_LEN)==0 )
        return parse_custom_property( data, start+AESE_PREFIX_LEN, end, Aese );
    else if ( strncmp(&data[start],MD_ATTR,MD_ATTR_LEN)==0 )
        return parse_custom_property( data, start+MD_ATTR_LEN, end, MdAttr );
	else if ( strncmp(&data[start],XML_ATTR,XML_ATTR_LEN)==0 )
        return parse_custom_property( data, start+XML_ATTR_LEN, end, XmlAttr );
    else if ( strncmp(&data[start],XML_TAG,XML_TAG_LEN)==0 )
        return parse_custom_property( data, start+XML_TAG_LEN, end, XmlTag );
    else if ( strncmp(&data[start],MD_TAG,MD_TAG_LEN)==0 )
        return parse_custom_property( data, start+MD_TAG_LEN, end, MdTag );
    return prop_temp;
}
/**
 * Set the output value of this property
 * @param p the property in question
 * @param value the value for the output attribute
 */
void css_property_set_output_value( css_property *p, char *value )
{
    p->output_value = strdup( value );
    if ( p->output_value == NULL )
        warning( "css_property: failed to duplicate output value\n" );
}