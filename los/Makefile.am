#

lib_LTLIBRARIES = liblos.la

liblos_la_CFLAGS = -DCL_LOG_DOMAIN=\"los\" -I$(top_srcdir) -I$(top_builddir) @GLIB_CFLAGS@ @CLOG_CFLAGS@

include_HEADERS = los.h

includelos_HEADERS = \
	lbase.h \
	lobject.h \
	lstring.h lstring-def.h

liblos_la_SOURCES = \
	lobject.c \
	lstring.c

BUILT_SOURCES = \
	lstring-def.h lstring.inl

lstring-def.h lstring.inl \
: ltypes.stamp-h

ltypes.stamp-h: ltypes.in
	cpp $< | $(top_builddir)/gentypes/los-gentypes
	echo >$@
