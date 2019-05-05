/*
 * cueprint.c -- print cd information based on a template
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2013 Svend Sorensen
 * For license terms, see the file COPYING in this distribution.
 */

#include <ctype.h>	// isdigit()
#include <getopt.h>	// getopt_long()
#include <stdio.h>	// fprintf(), printf(), snprintf(), stderr
#include <stdlib.h>	// exit()
#include <string.h>	// strcasecmp()

#include "libcue.h"

#if HAVE_CONFIG_H
#	include "config.h"
#else
#	define PACKAGE_STRING "cueprint"
#endif

/* default templates */

#define D_TEMPLATE "\
Disc Information\n\
arranger:	%A\n\
composer:	%C\n\
genre:		%G\n\
message:	%M\n\
no. of tracks:	%N\n\
performer:	%P\n\
songwriter:	%S\n\
title:		%T\n\
disc no.:	%D\n\
year/date:	%Y\n\
UPC/EAN:	%U\n\
"

#define T_TEMPLATE "\
Track %n Information\n\
arranger:	%a\n\
composer:	%c\n\
genre:		%g\n\
ISRC:		%i\n\
message:	%m\n\
track number:	%n\n\
performer:	%p\n\
title:		%t\n\
ISRC (CD-TEXT):	%u\n\
"

/* default string to print for unset (NULL) values */
#define VALUE_UNSET ""

// *_get_* functions can return an int or a char *
union Value {
	int ival;
	char *sval;
	char cval;
};

char *progname;

/* Print usage information and exit */
void usage(int status)
{
	if (!status) {
		printf("Usage: %s [option...] [file...]\n", progname);
		printf("Report disc and track information from a CUE or TOC file.\n"
		       "\n"
		       "OPTIONS\n"
		       "-h, --help			print usage\n"
		       "-i, --input-format cue|toc	set format of file(s)\n"
		       "-n, --track-number <number>	only print track information for single track\n"
		       "-d, --disc-template <template>	set disc template\n"
		       "-t, --track-template <template>	set track template\n"
		       "-V, --version			print version information\n"
		       "\n"
		       "Default disc template: %s\n"
		       "Default track template: %s\n"
		       "See the manual page for more information.\n",
		       D_TEMPLATE, T_TEMPLATE);
	} else
		fprintf(stderr, "Try `%s --help' for more information.\n", progname);

	exit(status);
}

/* Print version information and exit */
void version()
{
	printf("%s\n", PACKAGE_STRING);
	exit(0);
}

// TODO: Shouldn't we be using vprintf() to help us out with this stuff? (Branden Robinson)
void disc_field(char *conv, int length, struct Cd *cd, union Value *value)
{
	struct Cdtext	*cdtext	= cd_get_cdtext(cd);
	char		*c	= conv + length - 1;	// pointer to conversion character

//printf("DEBUG %s:%s cdtext:%p\n", __FILE__, __FUNCTION__);
	/*
	 * After setting value, set *c to specify value type:
	 * 'd' integer
	 * 's' string
	 * 'c' character
	 */
	switch (*c) {
	case 'A':
		value->sval = cdtext_get(cdtext, PTI_ARRANGER);
		*c = 's';
		break;
	case 'C':
		value->sval = cdtext_get(cdtext, PTI_COMPOSER);
		*c = 's';
		break;
	case 'D':
		value->sval = rem_get(cdtext, REM_DISCNUMBER);
		*c = 's';
		break;
	case 'G':
		value->sval = cdtext_get(cdtext, PTI_GENRE);
		*c = 's';
		break;
	case 'M':
		value->sval = cdtext_get(cdtext, PTI_MESSAGE);
		*c = 's';
		break;
	case 'N':
		value->ival = cd_get_ntrack(cd);
		*c = 'd';
		break;
	case 'P':
		value->sval = cdtext_get(cdtext, PTI_PERFORMER);
		*c = 's';
		break;
	case 'R':
		value->sval = cdtext_get(cdtext, PTI_ARRANGER);
		*c = 's';
		break;
	case 'S':
		value->sval = cdtext_get(cdtext, PTI_SONGWRITER);
		*c = 's';
		break;
	case 'T':
		value->sval = cdtext_get(cdtext, PTI_TITLE);
		*c = 's';
		break;
	case 'U':
		value->sval = cdtext_get(cdtext, PTI_UPC_ISRC);
		*c = 's';
		break;
	case 'Y':
		value->sval = rem_get(cdtext, REM_DATE);
		*c = 's';
		break;
	default:
		value->cval = *c;
		*c = 'c';
		break;
	}
}

void track_field(char *conv, int length, struct Cd *cd, int trackno, union Value *value)
{
	char *c = conv + length - 1;	// pointer to conversion character

	struct Track	*track	= cd_get_track(cd, trackno);
	struct Cdtext	*cdtext	= track_get_cdtext(track);

//printf("DEBUG %s:%s cdtext:%p\n", __FILE__, __FUNCTION__);
	switch (*c) {
	case 'a':
		value->sval = cdtext_get(cdtext, PTI_ARRANGER);
		*c = 's';
		break;
	case 'c':
		value->sval = cdtext_get(cdtext, PTI_COMPOSER);
		*c = 's';
		break;
	case 'f':
		value->sval = track_get_filename(track);
		*c = 's';
		break;
	case 'g':
		value->sval = cdtext_get(cdtext, PTI_GENRE);
		if (!value->sval)
			value->sval = cdtext_get(cd_get_cdtext(cd), PTI_GENRE);
		*c = 's';
		break;
	case 'i':
		value->sval = track_get_isrc(track);
		*c = 's';
		break;
	case 'm':
		value->sval = cdtext_get(cdtext, PTI_MESSAGE);
		*c = 's';
		break;
	case 'n':
		value->ival = trackno;
		*c = 'd';
		break;
	case 'p':
		value->sval = cdtext_get(cdtext, PTI_PERFORMER);
		if (!value->sval)
			value->sval = cdtext_get(cd_get_cdtext(cd), PTI_PERFORMER);
		*c = 's';
		break;
	case 's':
		value->sval = cdtext_get(cdtext, PTI_SONGWRITER);
		*c = 's';
		break;
	case 't':
		value->sval = cdtext_get(cdtext, PTI_TITLE);
		*c = 's';
		break;
	case 'u':
		value->sval = cdtext_get(cdtext, PTI_UPC_ISRC);
		*c = 's';
		break;
	default:
		disc_field(conv, length, cd, value);
		break;
	}

}

/*
 * Print a conversion specification.
 * [flag(s)][width][.precision]<conversion-char>
 */
void print_conv(char *start, int length, struct Cd *cd, int trackno)
{
	char *conv;	/* copy of conversion specification */
	union Value value;
	char *c;	/* pointer to conversion-char */

	/* TODO: use strndup? */
	conv = malloc((unsigned) (length + 1));
	strncpy(conv, start, length);
	conv[length] = '\0';

	/* conversion character */
	if (!trackno)
		disc_field(conv, length, cd, &value);
	else
		track_field(conv, length, cd, trackno, &value);

	c = conv + length - 1;

	switch (*c) {
	case 'c':
		printf(conv, value.cval);
		break;
	case 'd':
		printf(conv, value.ival);
		break;
	case 's':
		if (!value.sval)
			printf(conv, VALUE_UNSET);
		else
			printf(conv, value.sval);
		break;
	default:
		printf("%zu: ", strlen(conv));
		printf("%s", conv);
	}

	free(conv);
}

void cd_printf(char *template, struct Cd *cd, int trackno)
{
	char *c;	/* pointer into template */
	char *conv_start;
	int conv_length;

	for (c = template; '\0' != *c; c++) {
		if ('%' == *c) {
			conv_start = c;
			conv_length = 1;
			c++;

			/* flags */
			while ('-' == *c || '+' == *c || ' ' == *c || '0' == *c || '#' == *c) {
				conv_length++;
				c++;
			}

			/* field width */
			/* '*' not recognized */
			while (0 != isdigit(*c)) {
				conv_length++;
				c++;
			}

			/* precision */
			/* '*' not recognized */
			if ('.' == *c) {
				conv_length++;
				c++;

				while (0 != isdigit(*c)) {
					conv_length++;
					c++;
				}
			}

			/* length modifier (h, l, or L) */
			/* not recognized */

			/* conversion character */
			conv_length++;

			print_conv(conv_start, conv_length, cd, trackno);
		} else
			putchar(*c);
	}
}

int info(char *name, enum Format format, int trackno, char *d_template, char *t_template)
{
	struct Cd *cd = cf_parse(name, &format);
	int ntrack;

	if (!cd) {
		fprintf(stderr, "%s: error: unable to parse input file"
		        " `%s'\n", progname, name);
		return -1;
	}

	ntrack = cd_get_ntrack(cd);

	if (-1 == trackno) {
		cd_printf(d_template, cd, 0);

		for (trackno = 1; trackno <= ntrack; trackno++)
			cd_printf(t_template, cd, trackno);
	} else if (!trackno)
		cd_printf(d_template, cd, trackno);
	else if (0 < trackno && ntrack >= trackno)
		cd_printf(t_template, cd, trackno);
	else {
		fprintf(stderr, "%s: error: track number out of range\n", progname);
		return -1;
	}

	return 0;
}

/* 
 * Translate escape sequences in a string.
 * The string is overwritten and terminated.
 * TODO: this does not handle octal and hexidecimal escapes except for \0
 */
void translate_escapes(char *s)
{
	char	*read	= s,
			*write	= s;

	while ('\0' != *read) {
		if ('\\' == *read) {
			read++;

			*write	= *read == 'a' ? '\a'
				: *read == 'b' ? '\b'
				: *read == 'f' ? '\f'
				: *read == 'n' ? '\n'
				: *read == 'r' ? '\r'
				: *read == 't' ? '\t'
				: *read == 'v' ? '\v'
				: *read == '0' ? '\0'
				: *read;	// ?, ', " are handled by the default
		} else
			*write = *read;

		read++;
		write++;
	}

	*write = '\0';
}

int main(int argc, char *argv[])
{
	enum Format	format		= UNKNOWN;
	int		trackno		= -1,	// track number (-1 = unspecified, 0 = disc info)
			ret		= 0;	// return value of info()
	char		*d_template	= NULL,	// disc template
			*t_template	= NULL;	// track template

	/* option variables */
	int	c;
	/* getopt_long() variables */
	extern char	*optarg;
	extern int	optind;

	static struct option longopts[] = {
		{"help",		no_argument,		NULL, 'h'},
		{"input-format",	required_argument,	NULL, 'i'},
		{"track-number",	required_argument,	NULL, 'n'},
		{"disc-template",	required_argument,	NULL, 'd'},
		{"track-template",	required_argument,	NULL, 't'},
		{"version",		no_argument,		NULL, 'V'},
		{NULL, 0, NULL, 0}
	};

	progname = argv[0];

	while (-1 != (c = getopt_long(argc, argv, "hi:n:d:t:V", longopts, NULL))) {
		switch (c) {
		case 'h':
			usage(0);
			break;
		case 'i':
			if (!strcmp("cue", optarg))
				format = CUE;
			else if (!strcmp("toc", optarg))
				format = TOC;
			else {
				fprintf(stderr, "%s: error: unknown input file"
				        " format `%s'\n", progname, optarg);
				usage(1);
			}
			break;
		case 'n':
			trackno = atoi(optarg);
			break;
		case 'd':
			d_template = optarg;
			break;
		case 't':
			t_template = optarg;
			break;
		case 'V':
			version();
			break;
		default:
			usage(1);
			break;
		}
	}

	/* If no disc or track template is set, use the defaults for both. */
	if (!d_template && !t_template) {
		d_template = strdup(D_TEMPLATE);
		t_template = strdup(T_TEMPLATE);
	} else {
		if (!d_template)
			d_template = strdup("");
		if (!t_template)
			t_template = strdup("");
	}

	/* Translate escape sequences. */
	translate_escapes(d_template);
	translate_escapes(t_template);

	/* What we do depends on the number of operands. */
	if (optind == argc)
		/* No operands: report information about stdin. */
		ret = info("-", format, trackno, d_template, t_template);
	else
		/* Report information about each operand. */
		for (; optind < argc; optind++) {
			ret = info(argv[optind], format, trackno, d_template, t_template);
			/* Exit if info() returns nonzero. */
			if (!ret)
				break;
		}

	return ret;
}
