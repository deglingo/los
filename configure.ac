AC_INIT([los], [0.0.0])
AM_INIT_AUTOMAKE

AC_SUBST([pkgconfigdir], ['${libdir}/pkgconfig'])
AC_SUBST([includelosdir], ['${includedir}/los'])
# [FIXME] this one is defined in Makefile but does not appear in
# los.pc !?
AC_SUBST([pkglibdir], ['${libdir}/${PACKAGE}'])
AC_SUBST([vgsuppdir], ['${pkglibdir}/valgrind'])

: ${CFLAGS:=-g -O0 -Wall}

AC_PROG_CC
AM_PROG_CC_C_O
LT_INIT
AC_PROG_YACC

# [fixme] check for python3
AC_PATH_PROG([PYTHON], [python3])
test x"$PYTHON" != x || AC_ERROR([python3 not found])

PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([CLOG], [clog])

# test suite
AM_PIFTEST_SUITE([test])

AC_CONFIG_FILES([ \
  Makefile \
  gentypes/Makefile \
  los/Makefile \
  libltest/Makefile \
  test/Makefile \
])
AC_CONFIG_FILES([los.pc])
AC_OUTPUT
