
# Version string for various different purposes. None I have found to be
# actually useful yet.
AC_INIT([GTKWorkbook],
        [0.2.3], 
	[jvb4@njit.edu],
	[gtkworkbook])

# Options for Autoconf that need to be specified up front.
AC_PREREQ(2.59)
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([build-aux/config.h])
AC_CONFIG_FILES([Makefile])

# Initialize Automake with basic options.
AM_INIT_AUTOMAKE([1.9 foreign subdir-objects])

# Check for programs and requirements for GTKWorkbook
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

PYTHON=  
AC_PATH_PROG([PYTHON],[python],[:])
AS_IF([test "$PYTHON" != ":"],
      [AM_PYTHON_CHECK_VERSION([$PYTHON],[2.3],[:],[PYTHON=":"])])
AM_CONDITIONAL([HAVE_PYTHON],[test "$PYTHON" != ":"])

GTEST=
AC_PATH_PROG([GTEST], [gtest-config], [:])
AM_CONDITIONAL([HAVE_GTEST], [test "$GTEST" != ":"])

# End of the line: output all the files and let's get ready to rock!
AC_OUTPUT

