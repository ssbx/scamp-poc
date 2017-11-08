dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(scamp2, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])
AM_PROG_CC_C_O
AC_OPENMP
AC_PROG_RANLIB
AC_SUBST(AM_CFLAGS, "-Wall -Werror $OPENMP_CFLAGS")

dnl LIBS="-lcfitsio"
dnl AC_CHECK_HEADERS([fitsio.h, cfitsio/fitsio.h])
dnl AC_CHECK_LIB(cfitsio,ffopen)

AC_CONFIG_FILES(Makefile src/Makefile src/catalog/fits/Makefile)
AC_OUTPUT
