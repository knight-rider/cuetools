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
#include "cd.h"
#include "toc.h"

enum DataType {
	DATA_AUDIO,
	DATA_DATA,
	DATA_FIFO,
	DATA_ZERO
};

struct Data {
	enum DataType	type;
	char	*name;
	long	start,
		length;
};

struct Track {
	struct Data	zero_pre,	// pre-gap generated with zero data
			file,		// track data file
			zero_post;	// post-gap generated with zero data
	int		mode,		// track mode
			sub_mode,	// sub-channel mode
			flags;		// flags
	char		*isrc;		// IRSC Code (5.22.4) 12 bytes
	struct Cdtext	*cdtext;
	long		index[MAXINDEX];	// indexes (in frames, 5.29.2.5) relative to start of file
};

struct Cd {
	enum DiscMode	mode;		// disc mode
	char		*catalog,	// Media Catalog Number (5.22.3)
			*cdtextfile;	// Filename of CDText File
	struct Cdtext	*cdtext;
	struct Track	*track[MAXTRACK];
};

struct Cd *cd_init(void)
{
	struct Cd *cd = calloc(1, sizeof(*cd));

	if (cd && !(cd->cdtext = cdtext_init())) {
		free(cd);
		return NULL;
	}
	return cd;
}

struct Cdtext *track_get_cdtext(const struct Track *track)
{
	return track ? track->cdtext : NULL;
}

void track_free(struct Track *track)
{
	if (!track)
		return;
	cdtext_free(track_get_cdtext(track));
	free(track->isrc);
	free(track->zero_pre.name);
	free(track->zero_post.name);
	free(track->file.name);
	free(track);
}

int cd_get_ntrack(const struct Cd *cd)
{
	int i = -1;

	if (cd)
		do
			i++;
		while (i < MAXTRACK && cd->track[i]);
	return i;
}

void cd_free(struct Cd* cd)
{
	int	i,
		n = cd_get_ntrack(cd);

	if (!cd)
		return;
	free(cd->catalog);
	free(cd->cdtextfile);
	for (i = 0; i < n; i++)
		track_free(cd->track[i]);
	cdtext_free(cd_get_cdtext(cd));
	free(cd);
}

struct Track *track_init(void)
{
	struct Track *track = malloc(sizeof(*track));

	if (!track)
		fprintf(stderr, "unable to create track\n");
	else {
		track->zero_pre.type	= DATA_ZERO;
		track->zero_pre.name	= NULL;
		track->zero_pre.start	= -1;
		track->zero_pre.length	= -1;

		track->file.type	= DATA_AUDIO;
		track->file.name	= NULL;
		track->file.start	= -1;
		track->file.length	= -1;

		track->zero_post.type	= DATA_ZERO;
		track->zero_post.name	= NULL;
		track->zero_post.start	= -1;
		track->zero_post.length	= -1;

		track->mode	= MODE_AUDIO;
		track->sub_mode	= SUB_MODE_RW;
		track->flags	= FLAG_NONE;
		track->isrc	= NULL;
		if (!(track->cdtext = cdtext_init())) {
			free(track);
			return NULL;
		}

		int i;
		for (i = 0; i < MAXINDEX; i++)
			track->index[i] = -1;
	}
	return track;
}

void cd_set_mode(struct Cd *cd, int mode)
{
	cd->mode = mode;
}

enum DiscMode cd_get_mode(const struct Cd *cd)
{
	return cd->mode;
}

void cd_set_catalog(struct Cd *cd, char *catalog)
{
	if (cd->catalog)
		free(cd->catalog);
	cd->catalog = strdup(catalog);
}

char *cd_get_catalog(struct Cd *cd)
{
	return cd->catalog;
}

void cd_set_cdtextfile(struct Cd *cd, char *cdtextfile)
{
	if (cd->cdtextfile)
		free(cd->cdtextfile);
	cd->cdtextfile = strdup(cdtextfile);
}

const char *cd_get_cdtextfile(const struct Cd *cd)
{
	return cd->cdtextfile;
}

struct Cdtext *cd_get_cdtext(const struct Cd *cd)
{
	return cd ? cd->cdtext : NULL;
}

struct Track *cd_add_track(struct Cd *cd)
{
	int	n = cd_get_ntrack(cd);

	if (!cd)
		return NULL;
	if (n >= MAXTRACK) {
		fprintf(stderr, "too many tracks\n");
		n--;
	}
	free(cd->track[n]);
	cd->track[n] = track_init();	// reinit last track if there were too many
	return cd->track[n];
}

struct Track *cd_get_track(const struct Cd *cd, int i)
{
	if (cd && 0 < i && i <= cd_get_ntrack(cd))
		return cd->track[i - 1];
	return NULL;
}

void track_set_filename(struct Track *track, char *filename)
{
	if (track->file.name)
		free(track->file.name);
	track->file.name = strdup(filename);
}

char *track_get_filename(const struct Track *track)
{
	return track->file.name;
}

void track_set_start(struct Track *track, long start)
{
	track->file.start = start;
}

long track_get_start(const struct Track *track)
{
	return track->file.start;
}

void track_set_length(struct Track *track, long length)
{
	track->file.length = length;
}

long track_get_length(const struct Track *track)
{
	return track->file.length;
}

void track_set_mode(struct Track *track, enum TrackMode mode)
{
	track->mode = mode;
}

enum TrackMode track_get_mode(const struct Track *track)
{
	return track->mode;
}

void track_set_sub_mode(struct Track *track, enum TrackSubMode sub_mode)
{
	track->sub_mode = sub_mode;
}

enum TrackSubMode track_get_sub_mode(const struct Track *track)
{
	return track->sub_mode;
}

void track_set_flag(struct Track *track, enum TrackFlag flag)
{
	track->flags |= flag;
}

void track_clear_flag(struct Track *track, enum TrackFlag flag)
{
	track->flags &= ~flag;
}

int track_is_set_flag(const struct Track *track, enum TrackFlag flag)
{
	return track->flags & flag;
}

void track_set_zero_pre(struct Track *track, long length)
{
	track->zero_pre.length = length;
}

long track_get_zero_pre(const struct Track *track)
{
	return track->zero_pre.length;
}

void track_set_zero_post(struct Track *track, long length)
{
	track->zero_post.length = length;
}

long track_get_zero_post(const struct Track *track)
{
	return track->zero_post.length;
}

void track_set_isrc(struct Track *track, char *isrc)
{
	if (track->isrc)
		free(track->isrc);
	track->isrc = strdup(isrc);
}

char *track_get_isrc(const struct Track *track)
{
	return track->isrc;
}

long track_get_index(const struct Track *track, int i)
{
	return (0 <= i) && (i < MAXINDEX) ? track->index[i] : -1;
}

int track_get_nindex(struct Track *track)
{
	int	i,
		n = 0;

	for (i = 0; i < MAXINDEX; i++)
		if (track_get_index(track, i) != -1)
			n = i + 1;
	return n;
}

void track_set_index(struct Track *track, int i, long idx)
{
	if (i < MAXINDEX) {
		track->index[i] = idx;
		return;
	}
	fprintf(stderr, "too many indexes\n");
}

void track_add_index(struct Track *track, long idx)
{
	track_set_index(track, track_get_nindex(track), idx);
}

// dump cd info
static void cd_track_dump(struct Track *track)
{
	int i;

	printf("zero_pre: %ld\n",	track->zero_pre.length);
	printf("filename: %s\n",	track->file.name);
	printf("start: %ld\n",		track->file.start);
	printf("length: %ld\n",		track->file.length);
	printf("zero_post: %ld\n",	track->zero_post.length);
	printf("mode: %d\n",		track->mode);
	printf("sub_mode: %d\n",	track->sub_mode);
	printf("flags: 0x%x\n",		track->flags);
	printf("isrc: %s\n",		track->isrc);
	printf("indexes: %d\n", track_get_nindex(track));

	for (i = 0; i < MAXINDEX; ++i)
		if (track->index[i] != -1)
			printf("index %d: %ld\n", i, track->index[i]);

	if (track->cdtext) {
		printf("cdtext:\n");
		cdtext_dump(track->cdtext, 1);
	}
}

void cd_dump(struct Cd *cd)
{
	int	i,
		n = cd_get_ntrack(cd);

	printf("\nDisc Info\n");
	printf("mode: %d\n",		cd->mode);
	printf("catalog: %s\n",		cd->catalog);
	printf("cdtextfile: %s\n",	cd->cdtextfile);
	printf("tracks: %d\n",		n);
	if (cd->cdtext) {
		printf("cdtext:\n");
		cdtext_dump(cd->cdtext, 0);
	}

	for (i = 0; i < n; ++i) {
		printf("\nTrack %d Info\n", i + 1);
		cd_track_dump(cd->track[i]);
	}
}

enum Format cf_format_from_suffix(char *name)
{
	char *suffix;

	if (suffix = strrchr(name, '.')) {
		if (!strcasecmp(".cue", suffix))
			return CUE;
		else if (!strcasecmp(".toc", suffix))
			return TOC;
	}

	return UNKNOWN;
}

struct Cd *cf_parse(char *name, enum Format *format)
{
	FILE *fp = NULL;
	struct Cd *cd = NULL;

	if (UNKNOWN == *format)
		if (UNKNOWN == (*format = cf_format_from_suffix(name))) {
			fprintf(stderr, "%s: unknown file suffix\n", name);
			return NULL;
		}

	if (!strcmp("-", name))
		fp = stdin;
	else if (!(fp = fopen(name, "r"))) {
		fprintf(stderr, "%s: error opening file\n", name);
		return NULL;
	}

	switch (*format) {
	case CUE:
		cd = cue_parse_file(fp);
		break;
	case TOC:
		cd = toc_parse(fp);
		break;
	}

	if(stdin != fp)
		fclose(fp);

	return cd;
}

int cf_print(char *name, enum Format *format, struct Cd *cd)
{
	FILE *fp = NULL;

	if (UNKNOWN == *format)
		if (UNKNOWN == (*format = cf_format_from_suffix(name))) {
			fprintf(stderr, "%s: unknown file suffix\n", name);
			return -1;
		}

	if (!strcmp("-", name))
		fp = stdout;
	else if (!(fp = fopen(name, "w"))) {
		fprintf(stderr, "%s: error opening file\n", name);
		return -1;
	}

	switch (*format) {
	case CUE:
		cue_print(fp, cd);
		break;
	case TOC:
		toc_print(fp, cd);
		break;
	}

	if(stdout != fp)
		fclose(fp);

	return 0;
}
