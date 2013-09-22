#!/bin/bash
git submodule init
git submodule update
if test -f osl/autogen.sh; then
	(cd osl; ./autogen.sh && ./configure)
fi
if test -f piplib/autogen.sh; then
	(cd piplib; ./autogen.sh && ./configure)
fi
