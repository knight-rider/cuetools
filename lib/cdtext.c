/*
	Copyright (c)	2004, 2005, 2006, 2007	Svend Sorensen
					2009, 2010				Jochen Keil
					2018					Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdtext.h"

struct Cdtext {
	enum Pti		pti;
	enum PtiFormat	format;
	char			*value;
};

struct Cdtext *cdtext_init(void)
{
	struct Cdtext cdtext[] = {
		{PTI_TITLE,			FORMAT_CHAR,	NULL},
		{PTI_PERFORMER,		FORMAT_CHAR,	NULL},
		{PTI_SONGWRITER,	FORMAT_CHAR,	NULL},
		{PTI_COMPOSER,		FORMAT_CHAR,	NULL},
		{PTI_ARRANGER,		FORMAT_CHAR,	NULL},
		{PTI_MESSAGE,		FORMAT_CHAR,	NULL},
		{PTI_DISC_ID,		FORMAT_BINARY,	NULL},
		{PTI_GENRE,			FORMAT_BINARY,	NULL},
		{PTI_TOC_INFO1,		FORMAT_BINARY,	NULL},
		{PTI_TOC_INFO2,		FORMAT_BINARY,	NULL},
		{PTI_RESERVED1,		FORMAT_CHAR,	NULL},
		{PTI_RESERVED2,		FORMAT_CHAR,	NULL},
		{PTI_RESERVED3,		FORMAT_CHAR,	NULL},
		{PTI_RESERVED4,		FORMAT_CHAR,	NULL},
		{PTI_UPC_ISRC,		FORMAT_CHAR,	NULL},
		{PTI_SIZE_INFO,		FORMAT_BINARY,	NULL},
		{PTI_END,			FORMAT_CHAR,	NULL}
	};

	struct Cdtext *new_cdtext =
		(struct Cdtext *) calloc (sizeof (cdtext) / sizeof (*new_cdtext), sizeof (*new_cdtext));

	if (!new_cdtext)
		fprintf (stderr, "problem allocating memory\n");
	else
		memcpy (new_cdtext, cdtext, sizeof(cdtext));

	return new_cdtext;
}

void cdtext_delete(struct Cdtext *cdtext)
{
	int i;

	if (cdtext) {
		for (i = 0; PTI_END != cdtext[i].pti; i++)
			free (cdtext[i].value);
		free (cdtext);
	}
}

// return 0 if there is no cdtext, returns non-zero otherwise
int cdtext_is_empty(struct Cdtext *cdtext)
{
	for (; PTI_END != cdtext->pti; cdtext++)
		if (cdtext->value)
			return -1;

	return 0;
}

// sets cdtext's pti entry to field
void cdtext_set(int pti, char *value, struct Cdtext *cdtext)
{
	if (value)	// don't pass NULL to strdup
		for (; PTI_END != cdtext->pti; cdtext++)
			if (pti == cdtext->pti) {
				free (cdtext->value);
				cdtext->value = strdup (value);
			}
}

// returns value for pti, NULL if pti is not found
char *cdtext_get(enum Pti pti, const struct Cdtext *cdtext)
{
	for (; PTI_END != cdtext->pti; cdtext++)
		if (pti == cdtext->pti)
			return cdtext->value;
	return NULL;
}

const char *cdtext_get_key(enum Pti pti, int istrack)
{
	const char *key = NULL;

	switch (pti) {
	case PTI_TITLE:
		key = "TITLE";
		break;
	case PTI_PERFORMER:
		key = "PERFORMER";
		break;
	case PTI_SONGWRITER:
		key = "SONGWRITER";
		break;
	case PTI_COMPOSER:
		key = "COMPOSER";
		break;
	case PTI_ARRANGER:
		key = "ARRANGER";
		break;
	case PTI_MESSAGE:
		key = "MESSAGE";
		break;
	case PTI_DISC_ID:
		key = "DISC_ID";
		break;
	case PTI_GENRE:
		key = "GENRE";
		break;
	case PTI_TOC_INFO1:
		key = "TOC_INFO1";
		break;
	case PTI_TOC_INFO2:
		key = "TOC_INFO1";
		break;
	case PTI_RESERVED1:
		/* reserved */
		break;
	case PTI_RESERVED2:
		/* reserved */
		break;
	case PTI_RESERVED3:
		/* reserved */
		break;
	case PTI_RESERVED4:
		/* reserved */
		break;
	case PTI_UPC_ISRC:
		if (0 == istrack)
			key = "UPC_EAN";
		else
			key = "ISRC";
		break;
	case PTI_SIZE_INFO:
		key = "SIZE_INFO";
		break;
	}

	return key;
}

void cdtext_dump(struct Cdtext *cdtext, int istrack)
{
	int pti;
	const char *value = NULL;

	for (pti = 0; PTI_END != pti; pti++)
		if (value = cdtext_get(pti, cdtext)) {
			printf("%s: ", cdtext_get_key(pti, istrack));
			printf("%s\n", value);
		}
}
