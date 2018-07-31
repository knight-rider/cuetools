/*
 * Copyright (c) 2009, 2010	Jochen Keil
 * 		 2018		Budi Rachmanto
 *
 * For license terms, see the file COPYING in this distribution.
 */

// Only install this header for exports.

#ifndef LIBCUE_H
#define LIBCUE_H

#include <stdio.h>

enum Format {CUE, TOC, UNKNOWN};

// struct Cdtext pack type indicators
enum Pti {
	PTI_TITLE,	// title of album or track titles
	PTI_PERFORMER,	// name(s) of the performer(s)
	PTI_SONGWRITER,	// name(s) of the songwriter(s)
	PTI_COMPOSER,	// name(s) of the composer(s)
	PTI_ARRANGER,	// name(s) of the arranger(s)
	PTI_MESSAGE,	// message(s) from the content provider and/or artist
	PTI_DISC_ID,	// (binary) disc identification information
	PTI_GENRE,	// (binary) genre identification and genre information
	PTI_TOC_INFO1,	// (binary) table of contents information
	PTI_TOC_INFO2,	// (binary) second table of contents information
	PTI_RESERVED1,
	PTI_RESERVED2,
	PTI_RESERVED3,
	PTI_RESERVED4,	// for content provider only
	PTI_UPC_ISRC,	// UPC/EAN code of the album and ISRC code of each track
	PTI_SIZE_INFO,	// (binary) size information of the block
	PTI_SIZE	// terminating PTI (for stepping through PTIs)
};

enum Rem {
	REM_DATE,	// date of cd/track
	REM_DISCNUMBER,
	REM_REPLAYGAIN_ALBUM_GAIN,
	REM_REPLAYGAIN_ALBUM_PEAK,
	REM_REPLAYGAIN_TRACK_GAIN,
	REM_REPLAYGAIN_TRACK_PEAK,
	REM_SIZE	// terminating REM (for stepping through REMs)
};

struct Cdtext {
	char	*pti[PTI_SIZE],
		*rem[REM_SIZE];
};

// cue_parse.y
struct Cd *cue_parse_file(FILE *);
struct Cd *cue_parse_string(const char *);

// cuefile functions (cd.c)
struct Cd *cf_parse(char *fname, enum Format *format);
enum Format cf_format_from_suffix(char *name);
int cf_print(char *fname, enum Format *format, struct Cd *cue);

// Cd functions (cd.c)
struct Cd *cd_init(void);
void cd_free(struct Cd *cd);
void cd_dump(struct Cd *cd);
int cd_get_ntrack(const struct Cd *cd);
struct Track *cd_get_track(const struct Cd *cd, int i);

// Track functions (cd.c)
char *track_get_filename(const struct Track *track);
long track_get_start(const struct Track *track);
long track_get_length(const struct Track *track);
long track_get_zero_pre(const struct Track *track);
long track_get_zero_post(const struct Track *track);
char *track_get_isrc(const struct Track *track);
long track_get_index(const struct Track *track, int i);

// Cdtext & REM functions (cdtext.c)
char *cdtext_get(const struct Cdtext *cdtext, enum Pti i);
struct Cdtext *cd_get_cdtext(const struct Cd *cd);
struct Cdtext *track_get_cdtext(const struct Track *track);
char *rem_get(struct Cdtext *cdtext, enum Rem i);

#endif
