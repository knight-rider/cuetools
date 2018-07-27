/*
 * Copyright (c) 2009, 2010 Jochen Keil
 * For license terms, see the file COPYING in this distribution.
 */

#ifndef LIBCUE_H
#define LIBCUE_H

#include <stdio.h>

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

enum DataType {
	DATA_AUDIO,
	DATA_DATA,
	DATA_FIFO,
	DATA_ZERO
};

/* struct Cdtext pack type indicators */
enum Pti {
	PTI_TITLE,	/* title of album or track titles */
	PTI_PERFORMER,	/* name(s) of the performer(s) */
	PTI_SONGWRITER,	/* name(s) of the songwriter(s) */
	PTI_COMPOSER,	/* name(s) of the composer(s) */
	PTI_ARRANGER,	/* name(s) of the arranger(s) */
	PTI_MESSAGE,	/* message(s) from the content provider and/or artist */
	PTI_DISC_ID,	/* (binary) disc identification information */
	PTI_GENRE,	/* (binary) genre identification and genre information */
	PTI_TOC_INFO1,	/* (binary) table of contents information */
	PTI_TOC_INFO2,	/* (binary) second table of contents information */
	PTI_RESERVED1,	/* reserved */
	PTI_RESERVED2,	/* reserved */
	PTI_RESERVED3,	/* reserved */
	PTI_RESERVED4,	/* reserved for content provider only */
	PTI_UPC_ISRC,	/* UPC/EAN code of the album and ISRC code of each track */
	PTI_SIZE_INFO,	/* (binary) size information of the block */
	PTI_END		/* terminating PTI (for stepping through PTIs) */
};

enum RemType {
	REM_DATE,	/* date of cd/track */
	REM_DISCNUMBER,
	REM_REPLAYGAIN_ALBUM_GAIN,
	REM_REPLAYGAIN_ALBUM_PEAK,
	REM_REPLAYGAIN_TRACK_GAIN,
	REM_REPLAYGAIN_TRACK_PEAK,
	REM_END		/* terminating REM (for stepping through REMs) */
};

struct Cd *cue_parse_file(FILE *);
struct Cd *cue_parse_string(const char *);
void cd_delete(struct Cd *cd);

// Cd functions
enum DiscMode cd_get_mode(const struct Cd *cd);
const char *cd_get_cdtextfile(const struct Cd *cd);

// return number of tracks in cd
int cd_get_ntrack(const struct Cd *cd);

// Track functions
struct Track *cd_get_track(const struct Cd *cd, int i);
char *track_get_filename(const struct Track *track);
long track_get_start(const struct Track *track);
long track_get_length(const struct Track *track);
enum TrackMode track_get_mode(const struct Track *track);
enum TrackSubMode track_get_sub_mode(const struct Track *track);
int track_is_set_flag(const struct Track *track, enum TrackFlag flag);
long track_get_zero_pre(const struct Track *track);
long track_get_zero_post(const struct Track *track);
char *track_get_isrc(const struct Track *track);
long track_get_index(const struct Track *track, int i);

// Cdtext functions
struct Cdtext *cd_get_cdtext(const struct Cd *cd);
struct Cdtext *track_get_cdtext(const struct Track *track);
char *cdtext_get(enum Pti pti, const struct Cdtext *cdtext);

struct Rem *cd_get_rem(const struct Cd *cd);
struct Rem *track_get_rem(const struct Track *track);
char *rem_get(enum RemType, struct Rem *);

#endif
