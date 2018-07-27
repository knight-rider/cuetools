/*
	Copyright (c)	2009, 2010				Jochen Keil
					2018					Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rem.h"

struct Rem *rem_new(void)
{
	struct Rem rem[] = {
		{REM_DATE,			NULL},
		{REM_DISCNUMBER,		NULL},
		{REM_REPLAYGAIN_ALBUM_GAIN,	NULL},
		{REM_REPLAYGAIN_ALBUM_PEAK,	NULL},
		{REM_REPLAYGAIN_TRACK_GAIN,	NULL},
		{REM_REPLAYGAIN_TRACK_PEAK,	NULL},
		{REM_END,			NULL}
	};

	struct Rem *new_rem = (struct Rem *) calloc(sizeof(rem) / sizeof(*new_rem), sizeof(*new_rem));

	if (!new_rem)
		fprintf(stderr, "rem_new(): problem allocating memory\n");
	else
		memcpy(new_rem, rem, sizeof(rem));
	return new_rem;
}

void rem_free(struct Rem *rem)
{
	struct Rem *ptr = rem;

	if (!rem)
		return;
	do
		free(ptr->value);
	while ((++ptr)->cmt != REM_END);

	free(rem);
}

int rem_is_empty(struct Rem *rem)
{
	if (!rem)
		return 1;

	do {
		if (rem->value)
			return 0;
	} while ((++rem)->cmt != REM_END);

	return 1;
}

void rem_set(unsigned int cmt, char* value, struct Rem *rem)
{
	if (!rem || !value)
		return;

	do
		if (rem->cmt == cmt) {
			free(rem->value);
			rem->value = strdup(value);
			return;
		}
	while ((++rem)->cmt != REM_END);
}

char *rem_get(enum RemType cmt, struct Rem *rem)
{
	if (!rem)
		return NULL;

	do
		if (rem->cmt == cmt)
			return rem->value;
	while ((++rem)->cmt != REM_END);

	return NULL;
}

void rem_dump(struct Rem *rem)
{
	if (!rem)
		return;

	do
		printf("REM %u: %s\n", rem->cmt, rem->value);
	while ((++rem)->cmt != REM_END);
}
