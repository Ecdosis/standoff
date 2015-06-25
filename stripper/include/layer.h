/* 
 * File:   layer.h
 * Author: desmond
 *
 * Created on August 11, 2012, 10:29 AM
 */

#ifndef LAYER_H
#define	LAYER_H

#ifdef	__cplusplus
extern "C" {
#endif
typedef struct layer_struct layer;
layer *layer_create( UChar *name, milestone *milestones );
layer *layer_dispose( layer *l );
UChar *layer_name( layer *l );
int layer_has_milestone( layer *l, UChar *mname );

#ifdef	__cplusplus
}
#endif

#endif	/* LAYER_H */

