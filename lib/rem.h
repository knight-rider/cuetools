/*
	Copyright (c)	2009, 2010	Jochen Keil
					2018		Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

#ifndef REM_H
#define REM_H

#include "libcue.h"

struct Rem {
	enum RemType cmt;
	char *value;
};

// return new allocated rem struct
struct Rem *rem_new(void);

// free rem struct
void rem_free(struct Rem *);

// return true if allocated rem struct is empty
int rem_is_empty(struct Rem *);

/**
 * set value of rem comment
 * @param unsigned int: enum of rem comment
 */
void rem_set(unsigned int, char *, struct Rem *);

/**
 * dump all rem comments in human readable form
 * @param bool: if track: true; else false
 */
void rem_dump(struct Rem *);

#endif
