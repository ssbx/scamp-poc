dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(scamp-poc, 1.0)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])
AM_PROG_CC_C_O
AC_PROG_RANLIB

#dnl openmp
#AC_OPENMP
AC_SUBST(AM_CFLAGS, "-Wall -pthread")

dnl cfitsio
#AC_CHECK_HEADER(fitsio.h,,AC_MSG_ERROR(Could not find fitsio.h),)
AC_CHECK_LIB(cfitsio, ffopen,,AC_MSG_ERROR(Could not find cfitsio))

dnl wcs
AC_CHECK_HEADER([wcslib/wcs.h],,AC_MSG_ERROR(Could not find wcshdr.h),)
AC_CHECK_LIB(wcs, wcspih,,AC_MSG_ERROR(Could not find wcslib))

dnl maths
AC_CHECK_HEADER(math.h,,AC_MSG_ERROR(Not any math.h),)
AC_CHECK_LIB(m, sqrt,,AC_MSG_ERROR(Could not find math lib))

dnl
AC_CONFIG_FILES(Makefile src/Makefile tests/Makefile)
AC_OUTPUT
