#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([npapi-chewing],[0.0.1],[kanru@kanru.info])
AC_CONFIG_SRCDIR([src/np_chewing.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])

LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_LANG([C++])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
