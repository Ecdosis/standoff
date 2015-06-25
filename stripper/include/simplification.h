#ifndef SIMPLIFICATION_H
#define SIMPLIFICATION_H
typedef struct simplification_struct simplification;
simplification *simplification_new( UChar *xml_name, UChar *prop_name );
int simplification_contains( simplification *s, UChar **attrs );
void simplification_delete( simplification *s );
void simplification_add_attribute( simplification *s, attr1bute *a );
void simplification_remove_attribute( simplification *s, UChar ** attrs );
UChar *simplification_get_xml_name( simplification *s );
UChar *simplification_get_prop_name( simplification *s );
#endif
