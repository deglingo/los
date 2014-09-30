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
	lostest.h \
	lbase.h \
	ltrash.h \
	lobject.h \
	lnone.h lnone-def.h \
	lint.h lint-def.h \
	lstring.h lstring-def.h \
	llist.h llist-def.h \
	ltuple.h ltuple-def.h \
	ldict.h ldict-def.h \
	lstream.h lstream-def.h \
	lmemstream.h lmemstream-def.h \
	lfile.h lfile-def.h \
	lpackerbase.h \
	lpacker.h lpacker-def.h \
	lunpacker.h lunpacker-def.h

liblos_la_SOURCES = \
	ltrash.c \
	lobject.c \
	lnone.c \
	lint.c \
	lstring.c \
	llist.c \
	ltuple.c \
	ldict.c \
	lstream.c \
	lmemstream.c \
	lfile.c \
	lpackerbase.c \
	lpacker.c \
	lunpacker.c

BUILT_SOURCES = \
	lnone-def.h lnone.inl \
	lint-def.h lint.inl \
	lstring-def.h lstring.inl \
	llist-def.h llist.inl \
	ltuple-def.h ltuple.inl \
	ldict-def.h ldict.inl \
	lstream-def.h lstream.inl \
	lmemstream-def.h lmemstream.inl \
	lfile-def.h lfile.inl \
	lpacker-def.h lpacker.inl \
	lunpacker-def.h lunpacker.inl

lnone-def.h lnone.inl \
lint-def.h lint.inl \
lstring-def.h lstring.inl \
llist-def.h llist.inl \
ltuple-def.h ltuple.inl \
ldict-def.h ldict.inl \
lstream-def.h lstream.inl \
lmemstream-def.h lmemstream.inl \
lfile-def.h lfile.inl \
lpacker-def.h lpacker.inl \
lunpacker-def.h lunpacker.inl \
: ltypes.stamp-h

ltypes.stamp-h: ltypes.in
	cpp $< | $(top_builddir)/gentypes/los-gentypes
	echo >$@
