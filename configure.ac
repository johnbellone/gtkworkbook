#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([gtkworkbook], 0.20, [jvb4@njit.edu])
AC_CONFIG_SRCDIR([libgtkworkbook/cell.c])
AM_INIT_AUTOMAKE([gtkworkbook], 0.20)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_CC_C_O

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CHECK_FUNCS([gethostbyname gettimeofday memset socket])

AC_OUTPUT([Makefile
	   cfg/Makefile
	   libgtkworkbook/OBJS/Makefile
           src/OBJS/Makefile])

