#

check_LTLIBRARIES = libltest.la

libltest_la_CFLAGS = -DCL_LOG_DOMAIN=\"libltest\" -I$(top_srcdir) -I$(top_builddir) @GLIB_CFLAGS@ @CLOG_CFLAGS@

libltest_la_SOURCES = \
	fake.c \
	fake2.c \
	packable.c \
	bytestream.c \
	tprop.c \
	sigobj.c

BUILT_SOURCES = \
	fake-def.h fake.inl \
	fake2-def.h fake2.inl \
	packable-def.h packable.inl \
	bytestream-def.h bytestream.inl \
	tprop-def.h tprop.inl \
	sigobj-def.h sigobj.inl

fake-def.h fake.inl \
fake2-def.h fake2.inl \
packable-def.h packable.inl \
bytestream-def.h bytestream.inl \
tprop-def.h tprop.inl \
sigobj-def.h sigobj.inl \
: ltesttypes.stamp-h

ltesttypes.stamp-h: ltesttypes.in
	cpp $< | $(top_builddir)/gentypes/los-gentypes
	echo >$@
