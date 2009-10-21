#!/bin/sh

# Script to do the configuration work required before running configure
# when starting from fresh CVS checkout, or cvs update

package="gtkworkbook"

# a silly hack that generates autoregen.sh but it's handy
echo "#!/bin/sh" > autoregen.sh
echo "./autogen.sh $@ \$@" >> autoregen.sh
chmod +x autoregen.sh

if test `uname` = FreeBSD -a -e macros/$package.0.m4 ; then
	echo
	echo -n 'FreeBSD patch: removing "macros/$package.0.m4"... '
	rm -f macros/$package.0.m4
	echo "done"
	echo
fi

LIBTOOLIZE="libtoolize --copy --force --automake"
ACLOCAL=aclocal
AUTOCONF=autoconf
AUTOHEADER=autoheader
AUTOMAKE="automake -a -c --foreign"

# Get m4 from current PATH!
echo -n "Locating m4 macro language processor... "
for GNUM4 in ${M4} m4 gm4 gnum4 ; do
	if test -x "`which ${GNUM4}`"; then
		ok=yes
		GNUM4=`which ${GNUM4}`
   		echo "found: ${GNUM4}"
		break;
	fi
done
if test x${ok} != xyes ; then
    echo "not found -- aborting `basename $0`"
    exit 1
fi

# Clean up old files which could hurt otherwise
rm -f config.cache config.log config.status

# generate changes from git 
if ( git --version ) </dev/null > /dev/null 2>&1; then
    echo -n "Writing git branch change history to CHANGES..."
    git log > CHANGES
    echo "done."
else
    echo "git not found -- cannot write branch change history"
fi

# generate config.guess config.sub ltconfig ltmain.sh
if ( libtoolize --version ) </dev/null > /dev/null 2>&1; then
	rm -f config.guess config.sub ltconfig ltmain.sh
	echo -n "Building files for libtool... "
	${LIBTOOLIZE}
	echo "done."
else
	echo "libtoolize not found -- aborting `basename $0`"
	exit 1
fi

# generate aclocal.m4
if ( aclocal --version ) </dev/null > /dev/null 2>&1; then
	rm -f aclocal.m4
	echo -n "Building aclocal.m4... "
	${ACLOCAL}
	echo "done."
else
	echo "aclocal not found -- aborting `basename $0`"
	exit 1
fi

# generate configure from configure.in
if ( autoconf --version ) </dev/null > /dev/null 2>&1; then
	rm -f configure
	echo -n "Building configure... "
	${AUTOCONF}
	echo "done."
else
	echo "autoconf not found -- aborting `basename $0`"
	exit 1
fi

# generate stamp-h.in and all Makefile.in
if ( automake --version ) </dev/null > /dev/null 2>&1; then
	rm -f stamp-h.in
	echo -n "Building Makefile templates... "
	${AUTOMAKE}
	echo "done."
else
	echo "automake not found -- aborting `basename $0`"
	exit 1
fi

echo ./configure $@
./configure $@ || {
        echo "  configure failed"
        exit 1
}

echo "Now type 'sh build all' to compile $package."
