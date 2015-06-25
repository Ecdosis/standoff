/**
 * A recipe is a set of rules for simplifying element+attribute
 * combinations in XML into simple properties in Aese format.
 * Each simplified property equates to a single format in HTML.
 * The recipe file allows us to customise the simplification
 * and also to reverse it. A rule applies iff the element name
 * (or property name in reverse) and its specified attributes match.
 * This module provides one method for loading a recipe and another
 * for simplifying a given xml element and its attributes. It also
 * has a method for testing if a recipe contains a given rule.
 * (c) Desmond Schmidt 2011
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include "attribute.h"
#include "simplification.h"
#include "milestone.h"
#include "layer.h"
#include "recipe.h"
#include "error.h"
#include "cJSON.h"
#include "utils.h"
#include "encoding.h"
#include "memwatch.h"
struct recipe_struct
{
    // list of removals
    UChar **removals;
    // list of simplifications
    simplification **rules;
    /** list of extra layers */
    layer **layers;
};
static simplification *current_rule = NULL;
/**
 * Allocate a totally empty recipe
 * @return the newly allocated recipe
 */
recipe *recipe_new()
{
    recipe *r = calloc(1, sizeof(recipe) );
    if ( r != NULL )
    {
        r->rules = calloc(1,sizeof(simplification*));
        if ( r->rules != NULL )
        {   
            r->removals = calloc( 1, sizeof(UChar*) );
            if ( r->removals == NULL )
            {
                r = recipe_dispose(r);
                fprintf(stderr, "recipe: failed to allocate removal\n" );
            }
        }
        else
        {
            r = recipe_dispose( r );
            fprintf(stderr, "recipe: failed to allocate rule\n" );
        }
    }
    else
        fprintf(stderr, "recipe: failed to allocate recipe\n" );
    return r;
}
/**
 * Add an attribute to a rule
 * @param s the rule to add it to
 * @param name the name of the attribute
 * @param value its value
 */
static void recipe_add_attribute( simplification *s, const UChar *name,
    const UChar *value )
{
    attr1bute *a = attribute_new( name, value );
    simplification_add_attribute( s, a );
}
/**
 * Count the number of simplification rules in a NULL-terminated array
 * @param rules a NULL-terminated array of pointers
 * @return the number of elements in the list
 */
static int count_rules( simplification **rules )
{
    int i = 0;
    while ( rules[i] != NULL )
        i++;
    return i;
}
/**
 * Add a layer to the recipe
 * @param r the recipe to add it to
 * @param l the layer object already created
 * @return 1 if it worked else 0
 */
static int recipe_add_layer( recipe *r, layer *l )
{
    int res = 0;
    if ( r->layers == NULL )
    {
        r->layers = calloc( 2, sizeof(layer*) );
        r->layers[0] = l;
    }
    else
    {
        int i = 0;
        while ( r->layers[i] != NULL )
            i++;
        layer **ll = calloc( i+1, sizeof(layer*) );
        if ( ll != NULL )
        {
            i = 0;
            while ( r->layers[i] != NULL )
            {
                ll[i] = r->layers[i];
                i++;
            }
            ll[i] = l;
            free( r->layers );
            r->layers = ll;
            res = 1;
        }
        else
            fprintf(stderr,"recipe: failed to expand layer array\n");
    }
    return res;
}
/**
 * Add a basic rule
 * @param r the recipe to add it to
 * @param xml_name the name of the xml element
 * @param prop_name the name of the aese property
 * @return the rule added
 */
static simplification *recipe_add_rule( recipe *r,
    UChar *xml_name, UChar *prop_name )
{
    int i;
    simplification **rules;
    simplification *s = simplification_new( xml_name, prop_name );
    int n_rules = count_rules( r->rules );
    rules = malloc( (n_rules+2)*sizeof(simplification*) );
    if ( rules == NULL )
        error("recipe: failed to reallocate recipe rules\n");
    for ( i=0;i<n_rules;i++ )
        rules[i] = r->rules[i];
    rules[i] = s;
    rules[i+1] = (simplification*)NULL;
    free( r->rules );
    r->rules = rules;
    return s;
}
/**
 * Count the number of removals
 * @param removals a NULL-terminated array of string pointers
 * @return the number of removals
 */
static int count_removals( UChar **removals )
{
    int i = 0;
    while ( removals[i] != NULL )
        i++;
    return i;
}
/**
 * Add a removal to the recipe
 * @param r the recipe to add it to
 * @param removal name of an element to remove (and all its descendants)
 */
static void recipe_add_removal( recipe *r, UChar *removal )
{
    int i;
    UChar **removals;
    UChar *rm = u_strdup( removal );
    if ( rm == NULL )
        error( "recipe: failed to allocate for removal\n" );
    int n_removals = count_removals( r->removals );
    removals = calloc( n_removals+2,sizeof(UChar*) );
    if ( removals == NULL )
        error( "recipe: failed to reallocate removals\n");
    // copy old removals to new removals
    for ( i=0;i<n_removals;i++ )
        removals[i] = r->removals[i];
    removals[i+1] = NULL;
    removals[i] = rm;
    free( r->removals );
    r->removals = removals;
}
/**
 * Parse simplification rules
 * @param r the recipe to store them in
 * @param item the item containing the "rules" keyword
 */
static void recipe_parse_rules( recipe *r, cJSON *item )
{
    cJSON *obj = item->child;
    while ( obj != NULL )
    {
        const char *prop_name = NULL;
        const char *xml_name = NULL;
        const char *attr_name = NULL;
        const char *attr_value = NULL;
        cJSON *field = obj->child;
        while ( field != NULL )
        {
            if ( strcmp(field->string,"xml_name")==0 )
            {
                xml_name = field->valuestring;
            }
            else if ( strcmp(field->string,"prop_name")==0 )
            {
                prop_name = field->valuestring;
            }
            else if ( strcmp(field->string,"attribute")==0 )
            {
                if ( field->child != NULL )
                {
                    attr_name = field->child->string;
                    attr_value = field->child->valuestring;
                }
                else
                    warning("empty attribute\n");
            }
            field = field->next;
        }
        if ( prop_name == NULL || xml_name == NULL )
        {
            warning( "recipe: missing attribute prop_name or "
                "xml_name for rule\n" );
            current_rule = NULL;
        }
        else
        {
            UChar *u_xml_name = utf8toutf16((char*)xml_name);
            UChar *u_prop_name = utf8toutf16((char*)prop_name);
            if ( u_xml_name != NULL && u_prop_name != NULL )
                current_rule = recipe_add_rule( r, u_xml_name, u_prop_name );
            if ( current_rule != NULL && attr_name != NULL 
                && attr_value != NULL )
            {
                UChar *u_attr_name = utf8toutf16((char*)attr_name);
                UChar *u_attr_value = utf8toutf16((char*)attr_value);
                if ( u_attr_name != NULL && u_attr_value != NULL )
                    recipe_add_attribute( current_rule, u_attr_name, u_attr_value );
                if ( u_attr_name != NULL )
                    free( u_attr_name );
                if ( u_attr_value != NULL )
                    free( u_attr_value );
            }
            if ( u_xml_name != NULL )
                free( u_xml_name );
            if ( u_prop_name != NULL )
                free( u_prop_name );
        }
        obj = obj->next;
    }
}
/**
 * Parse a milestone set
 * @param item the parent of the milestone set
 * @return a list of milestones all in the set
 */
static milestone *parse_milestone_set( cJSON *item )
{
    milestone *list = NULL;
    cJSON *obj = item->child;
    while ( obj != NULL )
    {
        cJSON *field = obj->child;
        while ( field != NULL )
        {
            if ( strcmp(field->string,"xml_name")==0 )
            {
                UChar *u_valuestring = utf8toutf16(field->valuestring);
                if ( u_valuestring != NULL )
                {
                    milestone *m = milestone_create(u_valuestring);
                    free( u_valuestring );
                    if ( m != NULL )
                    {
                        if ( list == NULL )
                            list = m;
                        else
                            milestone_append(list,m);
                    }
                    else    // error reported in milestone
                        break;
                }
                else
                    break;
            }
            field = field->next;
        }
        obj = obj->next;
    }
    return list;
}
/**
 * Parse the layers element
 * @param r the recipe to store the layers in
 * @param item the item containing the "layers" element
 */
static void recipe_parse_layers( recipe *r, cJSON *item )
{
    cJSON *obj = item->child;
    while ( obj != NULL )
    {
        cJSON *field = obj->child;
        char *name = NULL;
        milestone *milestones = NULL;
        while ( field != NULL )
        {
            if ( strcmp(field->string,"name")==0 )
                name = field->valuestring;
            else if ( strcmp(field->string,"milestones")==0 )
                milestones = parse_milestone_set( field );
            field = field->next;
        }
        // now build the layer
        if ( name != NULL && milestones != NULL )
        {
            UChar *u_name = utf8toutf16(name);
            if ( u_name != NULL )
            {
                layer *l = layer_create( u_name, milestones );
                free( u_name );
                if ( l != NULL )
                {
                    if ( !recipe_add_layer(r,l) )
                        break;
                    milestones = NULL;
                    name = NULL;
                }
                else
                    break;
            }
            else
                break;
        }
        obj = obj->next;
    }
}
/**
 * Parse a json recipe file. Should be simple.
 * @param root the root element of the JSON tree
 * @param r a recipe object to fill in
 */
static void recipe_parse_json( recipe *r, cJSON *root )
{
    cJSON *item = root->child;
    while ( item != NULL )
    {
        if ( strcmp(item->string,"rules")==0 )
            recipe_parse_rules( r, item );
        else if ( strcmp(item->string,"layers")==0 )
            recipe_parse_layers( r, item );
        else if ( strcmp(item->string,"type")==0 )
        {
            if ( item->valuestring==NULL
                ||strcmp(item->valuestring,"stripper")!=0 )
            {
                error("incorrect recipe type %s\n",item->valuestring);
                break;
            }
            // else ignore it
        }
        else if ( strcmp(item->string,"removals")==0 )
        {
        	cJSON *child = item->child;
            while ( child != NULL )
            {
                if ( child->valuestring != NULL )
                {
                    UChar *u_valuestring = utf8toutf16(child->valuestring);
                    if ( u_valuestring != NULL )
                    {
                        recipe_add_removal( r, u_valuestring );
                        free( u_valuestring);
                    }
                }
                child = child->next;
            }
        }
		item = item->next;
    }
}
/**
 * Load a recipe from its json file
 * @param r the recipe object
 * @param buf the JSON recipe file as a string
 * @return a loaded recipe or NULL 
 */
static recipe *recipe_load_json( const char *buf )
{
    recipe *r = recipe_new();
    if ( r != NULL )
    {
        cJSON *root = cJSON_Parse( buf );
        if ( root != NULL )
        {
            recipe_parse_json( r, root );
            cJSON_Delete( root );
        }
        else
            warning("parse of JSON config failed!\n");
    }
    return r;
}
/**
 * Is the first non-whitespace character a '<' or '{' etc?
 * @param buf the string to test
 * @param first the first character
 * @return 1 or 0
 */
static int begins_with( const char *buf, char first )
{
    int i = 0;
    while ( buf[i] != 0 )
    {
        if ( !isspace(buf[i]) )
        {
            if ( buf[i] == first )
                return 1;
            else
                return 0;
        }
        i++;
    }
    return 0;
}
/**
 * Load a recipe from a json OR xml file
 * @param r the recipe object
 * @param buf the recipe file as a string
 * @return a loaded recipe or NULL 
 */
recipe *recipe_load( const char *buf, int len )
{
    if ( begins_with(buf,'{') )
        return recipe_load_json( buf );
    else
    {
        warning("invalid config format\n");
        return NULL;
    }
}
/**
 * Simplify an XML element and its attributes. Assume that the recipe
 * already matches the element.
 * @param r the recipe to use
 * @param name the name of the XML element
 * @param attrs copy of its attributes (will be modified!)
 * @return the name of the Aese property
 */
UChar *recipe_simplify( recipe *r, UChar *name, UChar **attrs )
{
    simplification *s = recipe_has_rule( r, name, attrs );
    if ( s != NULL )
    {
        simplification_remove_attribute( s, attrs );
        return simplification_get_prop_name(s);
    }
    return NULL;
}
/**
 * Does this recipe contain a rule for the current element?
 * @param r the recipe to use
 * @param name the name of the XML element
 * @param attrs its attributes from expat
 * @return pointer to the rule for this element, else NULL
 */
simplification *recipe_has_rule( recipe *r, UChar *name,
    UChar **attrs )
{
    int i=0;
    while ( r->rules[i] != NULL )
    {
        if ( u_strcmp(simplification_get_xml_name(r->rules[i]),name)==0
            && simplification_contains(r->rules[i],attrs) )
            return r->rules[i];
        i++;
    }
    return NULL;
}
/**
 * Count the number of layers
 * @param r the recipe to count layers for
 * @return the number of layers defined in the recipe
 */
int recipe_num_layers( recipe *r )
{
    int i = 0;
    if ( r->layers != NULL )
    {
        while ( r->layers[i] != NULL )
            i++;
    }
    return i;
}       
/**
 * Get a specific layer;
 * @param r the recipe
 * @param i its index
 * @return the layer
 */
layer *recipe_layer( recipe *r, int i )
{
    return r->layers[i];
}
/**
 * Dispose of a recipe and all its children
 * @param r the recipe
 * @return NULL;
 */
recipe *recipe_dispose( recipe *r )
{
    int i;
    if ( r->removals != NULL )
    {
        i = 0;
        while ( r->removals[i] != NULL )
            free( r->removals[i++] );
        free( r->removals );
    }
    if ( r->rules != NULL )
    {
        i = 0;
        while ( r->rules[i] != NULL )
            simplification_delete( r->rules[i++] );
        free( r->rules );
    }
    if ( r->layers != NULL )
    {
        i = 0;
        while ( r->layers[i] != NULL )
            layer_dispose( r->layers[i++] );
        free( r->layers );
    }
    free( r );
    return NULL;
}
/**
 * Do we have the given element in our removals list?
 * @param r the recipe in question
 * @param removal the name of the element that may be removed
 * @return 1 if it is slated for removal, 0 otherwise
 */
int recipe_has_removal( recipe *r, UChar *removal )
{
    int i = 0;
    while ( r->removals[i] != NULL )
    {
        if ( u_strcmp(removal,r->removals[i++])==0 )
            return 1;
    }
    return 0;
}
