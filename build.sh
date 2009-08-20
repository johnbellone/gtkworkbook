#!/bin/sh
# Script to build the system and copy the files into BIN directory.
# @author: jb <jvb4@njit.edu>

package="gtkworkbook"

if [ ! -f Makefile ]; then
    echo "Building project... "
    sh autogen.sh
fi

echo make $@
make $@ || {
    echo "  build failed"
    exit 1
}

cp src/OBJS/gtkworkbook bin/gtkworkbook
cp src/OBJS/.libs/realtime.so bin/realtime.so
cp src/OBJS/.libs/largefile.so bin/largefile.so
cp cfg/application.cfg bin/application.cfg

echo "build complete!"