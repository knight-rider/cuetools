/*
 * cuebreakpoints.c -- print track break points
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2013 Svend Sorensen
 * For license terms, see the file COPYING in this distribution.
 */

#include <getopt.h>	// getopt_long()
#include <stdio.h>	// fprintf(), printf(), snprintf(), stderr
#include <stdlib.h>	// exit()
#include <string.h>	// strcasecmp()

#include "cd.h"
#include "time.h"

#if HAVE_CONFIG_H
#include "config.h"
#else /* not HAVE_CONFIG_H */
#define PACKAGE_STRING "cuebreakpoints"
#endif /* HAVE_CONFIG_H */

char *progname;

/*
 * pregap correction modes:
 * APPEND - append pregap to previous track (except for first track)
 * PREPEND - prefix pregap to current track
 * SPLIT - print breakpoints for beginning and end of pregap
 */
enum GapMode {APPEND, PREPEND, SPLIT};

/* Print usage information and exit */
void usage(int status)
{
	if (!status) {
		printf("Usage: %s [option...] [file...]\n", progname);
		printf("Report track breakpoints from a CUE or TOC file.\n"
		       "\n"
		       "OPTIONS\n"
		       "-h, --help			print usage\n"
		       "-i, --input-format cue|toc	set format of file(s)\n"
		       "--append-gaps			append pregaps to previous track (default)\n"
		       "--prepend-gaps			prefix pregaps to track\n"
		       "--split-gaps			split at beginning and end of pregaps\n"
		       "-V, --version			print version information\n");
	} else
		fprintf(stderr, "Try `%s --help' for more information.\n", progname);

	exit (status);
}

/* Print version information and exit */
void version()
{
	printf("%s\n", PACKAGE_STRING);

	exit(0);
}

void print_m_ss_ff(long frame)
{
	int m, s, f;

	time_frame_to_msf(frame, &m, &s, &f);
	printf ("%d:%02d.%02d\n", m, s, f);
}

void print_breakpoint(long b)
{
	/* Do not print zero breakpoints. */
	if (b)
		print_m_ss_ff(b);
}

void print_breaks(struct Cd *cd, int gaps)
{
	int		i;
	long	b,
			pg;
	struct Track *track;

	for (i = 1; i <= cd_get_ntrack(cd); i++) {
		track = cd_get_track(cd, i);
		/*
		 * When breakpoint is at:
		 * index 0: gap is prepended to track
		 * index 1: gap is appended to previous track
		 */
		b = track_get_start(track);
		pg = track_get_index(track, 1) - track_get_zero_pre(track);

		if (gaps == PREPEND || gaps == SPLIT) {
			print_breakpoint(b);
		/*
		 * There is no previous track to append the first track's
		 * pregap to.
		 */
		} else if (gaps == APPEND && 1 < i)
			print_breakpoint(b + pg);

		/* If pregap exists, print breakpoints (in split mode). */
		if (gaps == SPLIT && 0 < pg)
			print_breakpoint(b + pg);
	}
}

int breaks(char *name, enum Format format, int gaps)
{
	struct Cd *cd = NULL;

	if (!(cd = cf_parse(name, &format))) {
		fprintf(stderr, "%s: error: unable to parse input file"
		        " `%s'\n", progname, name);
		return -1;
	}

	print_breaks(cd, gaps);
	return 0;
}

int main(int argc, char *argv[])
{
	int	format	= UNKNOWN,
		gaps	= APPEND,
		ret		= 0;		/* return value of breaks() */

	/* option variables */
	int c;
	/* getopt_long() variables */
	extern char *optarg;
	extern int optind;

	static struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"input-format", required_argument, NULL, 'i'},
		{"append-gaps", no_argument, NULL, 'a'},
		{"prepend-gaps", no_argument, NULL, 'p'},
		{"split-gaps", no_argument, NULL, 's'},
		{"version", no_argument, NULL, 'V'},
		{NULL, 0, NULL, 0}
	};

	progname = argv[0];

	while (-1 != (c = getopt_long(argc, argv, "hi:V", longopts, NULL))) {
		switch (c) {
		case 'h':
			usage(0);
			break;
		case 'i':
			if (0 == strcmp("cue", optarg))
				format = CUE;
			else if (0 == strcmp("toc", optarg))
				format = TOC;
			else {
				fprintf(stderr, "%s: error: unknown input file"
				        " format `%s'\n", progname, optarg);
				usage(1);
			}
			break;
		case 'a':
			gaps = APPEND;
			break;
		case 'p':
			gaps = PREPEND;
			break;
		case 's':
			gaps = SPLIT;
			break;
		case 'V':
			version();
			break;
		default:
			usage(1);
			break;
		}
	}

	/* What we do depends on the number of operands. */
	if (optind == argc)
		/* No operands: report breakpoints of stdin. */
		ret = breaks("-", format, gaps);
	else
		/* Report track breakpoints for each operand. */
		for (; optind < argc; optind++) {
			ret = breaks(argv[optind], format, gaps);
			/* Exit if breaks() returns nonzero. */
			if (!ret)
				break;
		}

	return ret;
}
