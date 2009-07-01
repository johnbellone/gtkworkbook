#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(gtkworkbook, 0.12, jvb4@njit.edu)
AC_CONFIG_SRCDIR(gtkworkbook/cell.c)
AM_INIT_AUTOMAKE(gtkworkbook, 0.12)

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
AC_CHECK_FUNCS([gethostbyname memset socket])

AC_OUTPUT([Makefile
	   gtkworkbook/OBJS/Makefile
	   src/OBJS/Makefile
	   src/realtime/OBJS/Makefile
	   cfg/Makefile])
