/*
 * STIL.h
 *
 *  Created on: 23/10/2010
 *      Author: desmond
 */

#ifndef STIL_H_
#define STIL_H_

int STIL_write_header(void *arg, dest_file *dst, UChar *style );
int STIL_write_tail(void *arg, dest_file *dst);
int STIL_write_range( UChar *name, UChar **atts, int removed,
	int offset, int len, UChar *content, int content_len, int first, 
    dest_file *dst );
#endif /* STIL_H_ */
