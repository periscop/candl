#!/bin/sh
autoreconf -i
if test -f osl/autogen.sh; then
	(cd osl; ./autogen.sh)
fi
if test -f piplib/autogen.sh; then
	(cd piplib; ./autogen.sh)
fi
