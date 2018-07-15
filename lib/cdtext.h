/*
	Copyright (c)	2004, 2005, 2006, 2007	Svend Sorensen
					2009, 2010				Jochen Keil
					2018					Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

// ref: MMC-3 draft revsion - 10g
#ifndef CDTEXT_H
#define CDTEXT_H

#include "libcue.h"

enum PtiFormat {
	FORMAT_CHAR,		/* single or double byte character string */
	FORMAT_BINARY		/* binary data */
};

/* return a pointer to a new Cdtext */
struct Cdtext *cdtext_init(void);

/* release a Cdtext */
void cdtext_delete(struct Cdtext *cdtext);

/* returns non-zero if there are no CD-TEXT fields set, zero otherwise */
int cdtext_is_empty(struct Cdtext *cdtext);

/* set CD-TEXT field to value for PTI pti */
void cdtext_set(int pti, char *value, struct Cdtext *cdtext);

/*
 * returns appropriate string for PTI pti
 * if istrack is zero, UPC/EAN string will be returned for PTI_UPC_ISRC
 * othwise ISRC string will be returned
 */
const char *cdtext_get_key(enum Pti pti, int istrack);

// dump all cdtext info in human readable format (for debugging)
void cdtext_dump(struct Cdtext *cdtext, int istrack);

#endif
