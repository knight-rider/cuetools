/*
	Copyright (c)	2004, 2005, 2006, 2007	Svend Sorensen
			2009, 2010		Jochen Keil
			2018			Budi Rachmanto

	For license terms, see the file COPYING in this distribution.
*/

// ref: MMC-3 draft revsion - 10g
#ifndef CD_H
#define CD_H

#include "libcue.h"

#define MAXTRACK	99	// Red Book track limit (from 01 to 99)
#define MAXINDEX	99	// Red Book index limit (from 00 to 98)
#define PARSER_BUFFER	1024    // Parser buffer size

/*
 * disc modes
 * DATA FORM OF MAIN DATA (5.29.2.8)
 */
enum DiscMode {
	MODE_CD_DA,		// CD-DA
	MODE_CD_ROM,		// CD-ROM mode 1
	MODE_CD_ROM_XA		// CD-ROM XA and CD-I
};

/*
 * track modes
 * 5.29.2.8 DATA FORM OF MAIN DATA
 * Table 350 - Data Block Type Codes
 */
enum TrackMode {
	MODE_AUDIO,		// 2352 byte block length
	MODE_MODE1,		// 2048 byte block length
	MODE_MODE1_RAW,		// 2352 byte block length
	MODE_MODE2,		// 2336 byte block length
	MODE_MODE2_FORM1,	// 2048 byte block length
	MODE_MODE2_FORM2,	// 2324 byte block length
	MODE_MODE2_FORM_MIX,	// 2332 byte block length
	MODE_MODE2_RAW		// 2352 byte block length
};

/*
 * sub-channel mode
 * 5.29.2.13 Data Form of Sub-channel
 * NOTE: not sure if this applies to cue files
 */
enum TrackSubMode {
	SUB_MODE_RW,		/* RAW Data */
	SUB_MODE_RW_RAW		/* PACK DATA (written R-W */
};

/*
 * track flags
 * Q Sub-channel Control Field (4.2.3.3, 5.29.2.2)
 */
enum TrackFlag {
	FLAG_NONE		= 0x00,	/* no flags set */
	FLAG_PRE_EMPHASIS	= 0x01,	/* audio recorded with pre-emphasis */
	FLAG_COPY_PERMITTED	= 0x02,	/* digital copy permitted */
	FLAG_DATA		= 0x04,	/* data track */
	FLAG_FOUR_CHANNEL	= 0x08,	/* 4 audio channels */
	FLAG_SCMS		= 0x10,	/* SCMS (not Q Sub-ch.) (5.29.2.7) */
	FLAG_ANY		= 0xff	/* any flags set */
};

// Cd functions
enum DiscMode cd_get_mode(const struct Cd *cd);
void cd_set_mode(struct Cd *cd, int mode);
void cd_set_catalog(struct Cd *cd, char *catalog);
char *cd_get_catalog(struct Cd *cd);
void cd_set_cdtextfile(struct Cd *cd, char *cdtextfile);
const char *cd_get_cdtextfile(const struct Cd *cd);

// add new track to cd, return pointer of new track
struct Track *cd_add_track(struct Cd *cd);

// Track functions
enum TrackMode track_get_mode(const struct Track *track);
enum TrackSubMode track_get_sub_mode(const struct Track *track);
int track_is_set_flag(const struct Track *track, enum TrackFlag flag);
void track_set_filename(struct Track *track, char *filename);	// filename of data file
void track_set_start(struct Track *track, long start);		// starting position in data file
void track_set_length(struct Track *track, long length);	// length of data file to use
void track_set_mode(struct Track *track, enum TrackMode mode);
void track_set_sub_mode(struct Track *track, enum TrackSubMode sub_mode);
void track_set_flag(struct Track *track, enum TrackFlag flag);
void track_clear_flag(struct Track *track, enum TrackFlag flag);

void track_set_zero_pre(struct Track *track, long length);
void track_set_zero_post(struct Track *track, long length);
void track_set_isrc(struct Track *track, char *isrc);
void track_set_index(struct Track *track, int i, long index);

int track_get_nindex(struct Track *track);
void track_add_index(struct Track *track, long idx);

void cue_print(FILE *fp, struct Cd *cd);

#endif
