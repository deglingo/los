#

EXTRA_DIST = \
	ltypes.in \
	private.h

clean-local:
	-rm -f *-def.h *.inl *.stamp-h

lib_LTLIBRARIES = liblos.la

liblos_la_CFLAGS = -DCL_LOG_DOMAIN=\"los\" -I$(top_srcdir) -I$(top_builddir) @GLIB_CFLAGS@ @CLOG_CFLAGS@

include_HEADERS = los.h

includelos_HEADERS = \
	lbase.h \
	lobject.h \
	lint.h lint-def.h \
	lstring.h lstring-def.h \
	lstream.h lstream-def.h \
	lmemstream.h lmemstream-def.h \
	lfile.h lfile-def.h \
	lpackerbase.h \
	lpacker.h lpacker-def.h \
	lunpacker.h lunpacker-def.h

liblos_la_SOURCES = \
	lobject.c \
	lint.c \
	lstring.c \
	lstream.c \
	lmemstream.c \
	lfile.c \
	lpackerbase.c \
	lpacker.c \
	lunpacker.c

BUILT_SOURCES = \
	lint-def.h lint.inl \
	lstring-def.h lstring.inl \
	lstream-def.h lstream.inl \
	lmemstream-def.h lmemstream.inl \
	lfile-def.h lfile.inl \
	lpacker-def.h lpacker.inl \
	lunpacker-def.h lunpacker.inl

lint-def.h lint.inl \
lstring-def.h lstring.inl \
lstream-def.h lstream.inl \
lmemstream-def.h lmemstream.inl \
lfile-def.h lfile.inl \
lpacker-def.h lpacker.inl \
lunpacker-def.h lunpacker.inl \
: ltypes.stamp-h

ltypes.stamp-h: ltypes.in
	cpp $< | $(top_builddir)/gentypes/los-gentypes
	echo >$@
