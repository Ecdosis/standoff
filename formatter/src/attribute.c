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
#include <stdio.h>
#include <ctype.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include "attribute.h"
#include "error.h"
#include "utils.h"
#include "memwatch.h"
#include "hashmap.h"
#include "log.h"
#include "bigint.h"


struct attribute_struct
{
    char *name;
    char *value;
    // needed to convert back to annotation
    char *prop_name;
    attribute *next;
};
/*static hashmap *trails = NULL;*/
/**
 * Create an attribute
 * @param name its name
 * @param prop_name its original property name
 * @param value its value
 * @return the finished attribute or NULL
 */
attribute *attribute_create( char *name, char *prop_name, char *value )
{
    attribute *attr = calloc( 1, sizeof(attribute) );
    if ( attr != NULL )
    {
        // don't duplicate or dispose of this
        attr->prop_name = prop_name;
        attr->name = strdup( name );
        if ( attr->name == NULL )
        {
            attribute_dispose( attr );
            warning("attribute: failed to allocate attribute name\n");
            attr = NULL;
        }
        else
        {
            attr->value = strdup( value );
            if ( attr->value == NULL )
            {
                attribute_dispose( attr );
                warning("attribute: failed to allocate attribute value\n");
                attr = NULL;
            }
        }
    }
    else
        warning("attribute: failed to allocate attribute struct\n");
    return attr;
}
/**
 * Get the xml (property name)
 * @param attr the attribute
 * @return its property (xml) name
 */
char *attribute_prop_name( attribute *attr )
{
    return attr->prop_name;
}
/**
 * Add a suffix to an attribute value
 * @param attr the attribute
 * @param suffix the suffix
 * @return 1 if it worked else 0
 */
int attribute_append_value( attribute *attr, char *suffix )
{
    int vlen = strlen(attr->value);
    char *val1 = calloc( vlen+2,sizeof(char) );
    if ( val1 != NULL )
    {
        strcpy( val1, attr->value );
        strcat( val1, suffix );
        free( attr->value );
        attr->value = val1;
        return 1;
    }
    else
    {
        log_add("attribute: failed to copy attr\n");
        return 0;
    }
}
/**
 * Duplicate a string and concatenate it with another
 * @param src the source string to copy
 * @param suffix the suffix to append
 * @return an allocated string caller to free
 */
static char *dup_n_cat( char *src, char *suffix)
{
    char *dup = calloc( strlen(src)+strlen(suffix)+1, sizeof(char));
    strcat( dup, src );
    strcat( dup, suffix );
    return dup;
}
/**
 * Split an id into 2. instead of d10->d10a and d10b we use base 3 to obtain
 * d10.1 and d10.2 (without the dots),and express thatin base27 for 
 * compactness. Base27 has the property that 3 digits of base3 = one digit of 
 * base 27. Since this splitting process is repeatable we will get alignment 
 * of the left (d10... and the right (a10...) and the ids generated will be 
 * unique. Previously, generating sequential ids (a01, 10b etc) failed because 
 * the HTML was written out hierarchically so that you could get two 10bs.
 * @param attr the id-attribute to split
 * @return the id value of the next attribute
 */
char *attribute_inc_value( attribute *attr )
{
    int len = strlen(attr->value);
    char *suffix = NULL;
    int i;
    for ( i=1;i<len;i++ )
    {
        if ( !isdigit(attr->value[i]) )
        {
            suffix = &attr->value[i];
            break;
        }
    }
    if ( suffix==NULL )
    {
        // it's a bare id without any suffix e.g. "d10"
        char *value_left = dup_n_cat(attr->value,"b");  // '1' in base 3
        char *value_right = dup_n_cat(attr->value,"c"); // '2' in base 3
        if ( value_left == NULL || value_right == NULL )
        {   // prevent memory leaks!
            if ( value_right!=NULL )
                free(value_right);
            if ( value_left != NULL )
                free(value_left);
            return NULL;
        }
        free( attr->value );
        attr->value = value_left;
        return value_right;
    }
    else
    {
        // we've already got some kind of suffix in base 27...
        char *base3_num = trim_zeros(base27_to_base3(suffix));
        if ( base3_num == NULL )
            return NULL;
        int len3 = strlen(base3_num);
        char *suffix_left,*suffix_right;
        if ( base3_num[len3-1] == '1' )
        {
            suffix_left = dup_n_cat(base3_num,"2");
            suffix_right = dup_n_cat(base3_num,"1");
        }
        else
        {
            suffix_left = dup_n_cat(base3_num,"1");
            suffix_right = dup_n_cat(base3_num,"2");
        }
        free( base3_num );
        if ( suffix_left == NULL || suffix_right == NULL )
        {
            // avoid memory leaks in this unlikely case
            if ( suffix_right!=NULL )
                free(suffix_right);
            if ( suffix_left != NULL )
                free(suffix_left);
            return NULL;
        }
        char *base27_left = base3_to_base27(suffix_left);
        char *base27_right = base3_to_base27(suffix_right);
        free( suffix_left );
        free( suffix_right );
        if ( base27_left == NULL || base27_right == NULL )
        {
            // avoid memory leaks in this unlikely case
            if ( base27_right!=NULL )
                free(base27_right);
            if ( base27_left != NULL )
                free(base27_left);
            return NULL;
        }
        int base_len = (strlen(attr->value)-strlen(suffix));
        char *value_left = calloc(base_len+1+strlen(base27_left),sizeof(char));
        if ( value_left == NULL )
            return NULL;
        strncat(value_left,attr->value,base_len);
        strcat(value_left,base27_left);
        free( base27_left );
        char *value_right = calloc(base_len+1+strlen(base27_right),sizeof(char));
        if ( value_right == NULL )
            return NULL;
        strncat(value_right,attr->value,base_len);
        strcat(value_right,base27_right);
        free( base27_right );
        free( attr->value);
        attr->value = value_left;
        return value_right;
    }
}
/**
 * Clone an existing attribute. If it's an id, renumber it.
 * @param attr the attribute to clone
 * @return the attribute or NULL
 */
attribute *attribute_clone( attribute *attr )
{
    attribute *new_attr = NULL;
    if ( strcmp(attr->name,"id")==0 )
    {
        // inc suffix
        int vlen = strlen(attr->value);
        int res = 1;
        if ( vlen > 0 && isdigit(attr->value[vlen-1]) )
        {
            res = attribute_append_value( attr, "a" );
        }
        if ( res )
        {
            char *value = attribute_inc_value( attr );
            if ( value != NULL )
            {
                new_attr = attribute_create( attr->name, attr->prop_name, value );
                free( value );
            }
            else
                log_add("attribute: failed to inc value\n");
        }
        else
            log_add("attribute: failed to append value\n");
    }
    else
        new_attr = attribute_create( attr->name, attr->prop_name, attr->value );
    log_add("exited attribute_clone\n");
    return new_attr;
}
/**
 * Dispose of an attribute
 * @param attr the attr to throw away
 */
void attribute_dispose( attribute *attr )
{
    if ( attr->next != NULL )
        attribute_dispose( attr->next );
    if ( attr->name != NULL )
    {
        free( attr->name );
        attr->name = NULL;
    }
    if ( attr->value != NULL )
    {
        free( attr->value );
        attr->value = NULL;
    }
    free( attr );
    attr = NULL;
}
/**
 * Add one attribute onto the end of the list of which we are a part
 * @param attrs the list of attributes
 * @param attr the new attribute to append to the end
 */
void attribute_append( attribute *attrs, attribute *attr )
{
    while ( attrs->next != NULL )
        attrs = attrs->next;
    attrs->next = attr;
}
char *attribute_get_name( attribute *attr )
{
    return attr->name;
}
char *attribute_get_value( attribute *attr )
{
    return attr->value;
}
attribute *attribute_get_next( attribute *attr )
{
    return attr->next;
}
int attribute_count( attribute *attr )
{
    int count = 0;
    while ( attr != NULL )
    {
        count++;
        attr = attr->next;
    }
    return count;
}
