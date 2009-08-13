#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([gtkworkbook], [0.20], [jvb4@njit.edu])
AC_CONFIG_SRCDIR([gtkworkbook/cell.c])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname gettimeofday memset socket])

AC_CONFIG_FILES([Makefile
                 gtkworkbook/OBJS/Makefile
                 src/OBJS/Makefile
                 src/largefile/OBJS/Makefile
                 src/realtime/OBJS/Makefile])
AC_OUTPUT
