/*
	Copyright (c)	2004, 2005, 2006, 2007	Svend Sorensen
			2009, 2010		Jochen Keil
			2018			Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cdtext.h"

struct Cdtext *cdtext_init(void)
{
	return calloc(1, sizeof(struct Cdtext));
}

void cdtext_free(struct Cdtext *cdtext)
{
	enum Pti i;
	enum Rem j;

	if (cdtext) {
		for (i = PTI_TITLE; i < PTI_SIZE; i++)
			free(cdtext->pti[i]);
		for (j = REM_DATE; j < REM_SIZE; j++)
			free(cdtext->rem[j]);
		free(cdtext);
	}
}

bool cdtext_is_empty(struct Cdtext *cdtext)
{
	enum Pti i;

	if (cdtext)
		for (i = PTI_TITLE; i < PTI_SIZE; i++)
			if (cdtext->pti[i])
				return false;
	return true;
}

void cdtext_set(struct Cdtext *cdtext, enum Pti i, char *value)
{
	if (value) {	// don't pass NULL to strdup
		free(cdtext->pti[i]);
		cdtext->pti[i] = strdup (value);
	}
}

char *cdtext_get(const struct Cdtext *cdtext, enum Pti i)
{
	return !cdtext ? NULL : cdtext->pti[i];
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
		if (istrack)
			key = "ISRC";
		else
			key = "UPC_EAN";
		break;
	case PTI_SIZE_INFO:
		key = "SIZE_INFO";
		break;
	}

	return key;
}

void cdtext_dump(struct Cdtext *cdtext, bool istrack)
{
	enum Pti i;
	enum Rem j;
	const char *value = NULL;

	for (i = PTI_TITLE; i < PTI_SIZE; i++)
		if (value = cdtext_get(cdtext, i))
			printf("%s: %s\n", cdtext_get_key(i, istrack), value);
	for (j = REM_DATE; j < REM_SIZE; j++)
		if (value = cdtext->rem[j])
			printf("REM %u: %s\n", j, value);
}

void rem_set(struct Cdtext *cdtext, enum Rem i, char *value)
{
	if (!cdtext || !value)
		return;
	free(cdtext->rem[i]);
	cdtext->rem[i] = strdup(value);
}

char *rem_get(struct Cdtext *cdtext, enum Rem i)
{
	return !cdtext ? NULL : cdtext->rem[i];
}
