# Makefile.am - process with automake to produce Makefile.in

bin_PROGRAMS = cuebreakpoints cueconvert cueprint
bin_SCRIPTS = cuetag.sh cuesplit.sh

LIBTOOL = /bin/libtool

LDADD = ../lib/libcue.la
AM_CFLAGS = -Werror -I$(srcdir)/../lib
