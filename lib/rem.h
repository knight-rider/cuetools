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

void rem_set(enum RemType, char *, struct Rem *);

// dump all rem comments in human readable form
void rem_dump(struct Rem *);

#endif
