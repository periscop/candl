#!/bin/bash
git submodule init
git submodule update
if test -f osl/get_submodules.sh; then
	(cd osl; ./get_submodules.sh)
fi
if test -f piplib/get_submodules.sh; then
	(cd piplib; ./get_submodules.sh)
fi
