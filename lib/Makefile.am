# Makefile.am - process with automake to produce Makefile.in

BUILT_SOURCES = cue_parse.h toc_parse.h
EXTRA_DIST = $(BUILT_SOURCES)

AM_YFLAGS = -d
AM_LFLAGS = -olex.yy.c

AM_CFLAGS = -Werror
LIBTOOL = /bin/libtool

lib_LTLIBRARIES = libcue.la

libcue_la_LDFLAGS = -version-info 3:0:0
libcue_la_headers = cd.h cdtext.h libcue.h rem.h time.h toc.h toc_parse_prefix.h cue_parse_prefix.h
libcue_la_SOURCES = cd.c cdtext.c rem.c time.c cue_print.c toc_print.c \
		cue_parse.y cue_scan.l toc_parse.y toc_scan.l \
		$(libcuefile_a_headers)
