/*
 * format.h
 *
 *  Created on: 22/10/2010
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

#ifndef FORMAT_H_
#define FORMAT_H_
typedef struct format_struct format;
typedef int (*format_write_header)(void *arg, dest_file *dst, UChar *format );
typedef int (*format_write_tail)(void *arg, dest_file *dst);
typedef int (*format_write_range)( UChar *name, UChar **atts, int removed,
	int offset, int len, UChar *content, int content_len, int final, 
    dest_file *dst );
format_write_range format_rfunc(format *f);
format_write_tail format_tfunc(format *f);
format_write_header format_hfunc(format *f);
const char *format_text_suffix(format *f);
const char *format_markup_suffix(format *f);
const char *format_middle_name(format *f);
#endif /* FORMAT_H_ */
