#

check_LTLIBRARIES = libltest.la

libltest_la_CFLAGS = -I$(top_srcdir) -I$(top_builddir) @GLIB_CFLAGS@

libltest_la_SOURCES = \
	packable.c \
	bytestream.c

BUILT_SOURCES = \
	packable-def.h packable.inl \
	bytestream-def.h bytestream.inl

packable-def.h packable.inl \
bytestream-def.h bytestream.inl \
: ltesttypes.stamp-h

ltesttypes.stamp-h: ltesttypes.in
	cpp $< | $(top_builddir)/gentypes/los-gentypes
	echo >$@
