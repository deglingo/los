AC_INIT([los], [0.0.0])
AM_INIT_AUTOMAKE

AC_SUBST([pkgconfigdir], ['${libdir}/pkgconfig'])

: ${CFLAGS:=-g -Wall}

AC_PROG_CC
AM_PROG_CC_C_O
LT_INIT

# [fixme] check for python3
AC_PATH_PROG([PYTHON], [python3])
test x"$PYTHON" != x || AC_ERROR([python3 not found])

PKG_CHECK_MODULES([GLIB], [glib-2.0])

AC_CONFIG_FILES([ \
  Makefile \
  gentypes/Makefile \
])
AC_CONFIG_FILES([los.pc])
AC_CONFIG_FILES([gentypes/los-gentypes], [chmod +x gentypes/los-gentypes])
AC_OUTPUT
