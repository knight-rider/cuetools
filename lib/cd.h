/*
	Copyright (c)	2004, 2005, 2006, 2007	Svend Sorensen
					2009, 2010				Jochen Keil
					2018					Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

// ref: MMC-3 draft revsion - 10g

#ifndef CD_H
#define CD_H

#include "libcue.h"
#include "cdtext.h"
#include "rem.h"

#define MAXTRACK		99		// Red Book track limit (from 01 to 99)
#define MAXINDEX		99		// Red Book index limit (from 00 to 98)
#define PARSER_BUFFER	1024    // Parser buffer size

enum Format {CUE, TOC, UNKNOWN};

/* return pointer to CD structure */
struct Cd *cd_init(void);
struct Track *track_init(void);
void track_delete(struct Track* track);
void cd_dump(struct Cd *cd);

// Cd functions
void cd_set_mode(struct Cd *cd, int mode);
void cd_set_catalog(struct Cd *cd, char *catalog);
char *cd_get_catalog(struct Cd *cd);
void cd_set_cdtextfile(struct Cd *cd, char *cdtextfile);

/*
 * add a new track to cd, increment number of tracks
 * and return pointer to new track
 */
struct Track *cd_add_track(struct Cd *cd);

// Track functions
/* filename of data file */
void track_set_filename(struct Track *track, char *filename);
/* track start is starting position in data file */
void track_set_start(struct Track *track, long start);
/* track length is length of data file to use */
void track_set_length(struct Track *track, long length);
/* see enum TrackMode */
void track_set_mode(struct Track *track, int mode);
/* see enum TrackSubMode */
void track_set_sub_mode(struct Track *track, int sub_mode);
/* see enum TrackFlag */
void track_set_flag(struct Track *track, int flag);
void track_clear_flag(struct Track *track, int flag);

void track_set_zero_pre(struct Track *track, long length);
void track_set_zero_post(struct Track *track, long length);
void track_set_isrc(struct Track *track, char *isrc);
void track_set_index(struct Track *track, int i, long index);

int track_get_nindex(struct Track *track);
void track_add_index(struct Track *track, long idx);

enum Format cf_format_from_suffix(char *name);
struct Cd *cf_parse(char *fname, enum Format *format);
int cf_print(char *fname, enum Format *format, struct Cd *cue);
void cue_print(FILE *fp, struct Cd *cd);

#endif
