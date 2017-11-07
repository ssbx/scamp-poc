dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(scamp2, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])
AM_PROG_CC_C_O

AC_OPENMP
AC_PROG_RANLIB
AC_SUBST(AM_CFLAGS, "-Wall $OPENMP_CFLAGS")

AC_CONFIG_FILES(Makefile src/Makefile src/fits/Makefile)
AC_OUTPUT
