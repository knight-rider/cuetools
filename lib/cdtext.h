/*
	Copyright (c)	2004, 2005, 2006, 2007	Svend Sorensen
			2009, 2010		Jochen Keil
			2018			Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

// ref: MMC-3 draft revsion - 10g
#ifndef CDTEXT_H
#define CDTEXT_H

#include <stdbool.h>

#include "libcue.h"

struct Cdtext *cdtext_init(void);					// return a pointer to a new Cdtext
void cdtext_free(struct Cdtext *cdtext);				// release a Cdtext
bool cdtext_is_empty(struct Cdtext *cdtext);				// returns non-0 if no CD-TEXT field set, 0 otherwise
void cdtext_set(struct Cdtext *cdtext, enum Pti i, char *value);	// set CD-TEXT field to value for PTI pti
void cdtext_dump(struct Cdtext *cdtext, bool istrack);

/*
 * returns appropriate string for PTI pti
 * if istrack is zero, UPC/EAN string will be returned for PTI_UPC_ISRC
 * otherwise ISRC string will be returned
 */
const char *cdtext_get_key(enum Pti pti, int istrack);

void rem_set(struct Cdtext *cdtext, enum Rem i, char *value);

#endif
