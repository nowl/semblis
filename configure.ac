#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([semblis], [0.2])
AM_INIT_AUTOMAKE

# disable static libraries
#AM_DISABLE_STATIC
#AM_DISABLE_SHARED
AC_PROG_LIBTOOL

#AC_CONFIG_SRCDIR([main.cpp])
#AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lc':
#AC_CHECK_LIB([c], [main])
# FIXME: Replace `main' with a function in `-ldl':
#AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lsemblis':
#AC_CHECK_LIB([semblis], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 src/libsemblis/Makefile
		 plugins/Makefile])
AC_OUTPUT
