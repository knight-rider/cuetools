/*
 * cuebreakpoints.c -- print track break points
 *
 * Copyright (C)	2004, 2005, 2006, 2007, 2013	Svend Sorensen
 * 			2018				Budi Rachmanto
 * 
 * For license terms, see the file COPYING in this distribution.
 */

#include <getopt.h>	// getopt_long()
#include <stdio.h>	// fprintf(), printf(), snprintf(), stderr
#include <stdlib.h>	// exit()
#include <string.h>	// strcasecmp()
#include <stdbool.h>

#include "libcue.h"

#if HAVE_CONFIG_H
#	include "config.h"
#else
#	define PACKAGE_STRING "cuebreakpoints"
#endif

char *progname;

/*
 * pregap correction modes:
 * APPEND	append pregap to previous track (except for first track), default
 * PREPEND	prefix pregap to current track
 * SPLIT	print breakpoints for beginning and end of pregap

 * LENGTH	print the net length of each track
 */
enum GapMode {APPEND, LENGTH, PREPEND, SPLIT};

void usage(int status)
{
	if (!status) {
		printf("Usage: %s [option...] [file...]\n", progname);
		printf("Report track breakpoints from a CUE or TOC file.\n"
		       "\n"
		       "OPTIONS\n"
		       "-h, --help			print usage\n"
		       "-i, --input-format cue|toc	set format of file(s)\n"
		       "-l, --length			print the net length of each track\n"
		       "-m, --millisecond		print in m:ss.nnn (millisecond) instead of m:ss.ff (frame) format\n"
		       "-p, --prepend-gaps		prefix pregaps to track\n"
		       "-s, --split-gaps		split at beginning and end of pregaps\n"
		       "-V, --version			print version information\n");
	} else
		fprintf(stderr, "Try `%s --help' for more information.\n", progname);

	exit (status);
}

void version()
{
	printf("%s\n", PACKAGE_STRING);

	exit(0);
}

void print_breakpoint(long frame, bool is_ms)
{
	int m, s, f;
	double n;

	// Do not print zero breakpoints
	if (frame <= 0)
		return;
	if (is_ms) {
		time_frame_to_ms(frame, &m, &n);
		printf("%02d:%06.3f\n", m, n);
	} else {
		time_frame_to_msf(frame, &m, &s, &f);
		printf("%02d:%02d.%02d\n", m, s, f);
	}
}

/*
 * When breakpoint is at
 * index 0: gap is prepended to track
 * index 1: gap is appended to previous track
 */

int breaks(char *name, enum Format format, enum GapMode gaps, bool is_ms)
{
	struct Cd *cd = cf_parse(name, &format);
	int	i,
		n = cd_get_ntrack(cd);

	if (!cd) {
		fprintf(stderr, "%s: error: unable to parse input file"
		        " `%s'\n", progname, name);
		return -1;
	}

	for (i = 1; i <= n; i++) {
		struct Track *track = cd_get_track(cd, i);
		long	start	= track_get_start(track),
			pre0	= track_get_zero_pre(track),
			length	= track_get_length(track);

		switch (gaps) {
		case APPEND:
			print_breakpoint(start, is_ms);
			break;
		case LENGTH:
			printf("%d\t", i);
			print_breakpoint(length, is_ms);
			if (length <= 0)
				printf("\n");
			break;
		case PREPEND:
			print_breakpoint(start - (pre0 < 0 ? 0 : pre0), is_ms);
			break;
		case SPLIT:
			print_breakpoint(start, is_ms);
			if (length > 0)
				print_breakpoint(start + length, is_ms);
		}
	}
//cd_dump(cd);
	return 0;
}

int main(int argc, char *argv[])
{
	enum Format	format	= UNKNOWN;
	enum GapMode	gaps	= APPEND;
	bool		is_ms	= false;
	int ret = 0;		/* return value of breaks() */

	/* option variables */
	int c;
	/* getopt_long() variables */
	extern char *optarg;
	extern int optind;

	static struct option longopts[] = {
		{"help",		no_argument, NULL, 'h'},
		{"input-format",	required_argument, NULL, 'i'},
		{"length",		no_argument, NULL, 'l'},
		{"millisecond",		no_argument, NULL, 'm'},
		{"prepend-gaps",	no_argument, NULL, 'p'},
		{"split-gaps",		no_argument, NULL, 's'},
		{"version",		no_argument, NULL, 'V'},
		{NULL, 0, NULL, 0}
	};

	progname = argv[0];

	while (-1 != (c = getopt_long(argc, argv, "hi:lmpsV", longopts, NULL))) {
		switch (c) {
		case 'h':
			usage(0);
			break;
		case 'i':
			if (0 == strcmp("cue", optarg)) {
				format = CUE;
			} else if (0 == strcmp("toc", optarg)) {
				format = TOC;
			} else {
				fprintf(stderr, "%s: error: unknown input file"
				        " format `%s'\n", progname, optarg);
				usage(1);
			}
			break;
		case 'l':
			gaps = LENGTH;
			break;
		case 'm':
			is_ms = true;
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
		ret = breaks("-", format, gaps, is_ms);
	else
		/* Report track breakpoints for each operand. */
		for (; optind < argc; optind++) {
			ret = breaks(argv[optind], format, gaps, is_ms);
			/* Exit if breaks() returns nonzero. */
			if (!ret)
				break;
		}

	return ret;
}
