#!/bin/sh
make maintainer-clean
./autogen.sh
./configure --prefix=$HOME/usr \
            --with-piplib=system \
            --with-piplib-prefix=$HOME/usr \
            --with-scoplib=system \
            --with-scoplib-prefix=$HOME/usr \
            --with-osl=system \
            --with-osl-prefix=$HOME/usr \
            --with-clay=system \
            --with-clay-prefix=$HOME/usr
make -j 10
