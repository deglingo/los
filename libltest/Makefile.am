#

check_LTLIBRARIES = libltest.la

libltest_la_CFLAGS = -I$(top_srcdir) -I$(top_builddir) @GLIB_CFLAGS@

libltest_la_SOURCES = \
	ltestpackable.c

BUILT_SOURCES = \
	ltestpackable-def.h ltestpackable.inl

ltestpackable-def.h ltestpackable.inl \
: ltesttypes.stamp-h

ltesttypes.stamp-h: ltesttypes.in
	cpp $< | $(top_builddir)/gentypes/los-gentypes
	echo >$@
