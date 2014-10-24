#

bin_PROGRAMS = los-gentypes

los_gentypes_CFLAGS = -DCL_LOG_DOMAIN=\"los-gentypes\" @GLIB_CFLAGS@ @CLOG_CFLAGS@

los_gentypes_LDFLAGS = @GLIB_LIBS@ @CLOG_LIBS@

AM_YFLAGS = -d

los_gentypes_SOURCES = \
	parsergram.y \
	base.c \
	cident.c \
	ast.c \
	lexer.c \
	parser.c \
	writer.c \
	dumper.c \
	main.c
