#! /bin/sh
#
#   /**-------------------------------------------------------------------**
#    **                              CAnDL                                **
#    **-------------------------------------------------------------------**
#    **                            checker.sh                             **
#    **-------------------------------------------------------------------**/
# 
#/*****************************************************************************
# *   CAnDL : the Chunky Analyser for Dependences in Loops (experimental)     *
# *****************************************************************************
# *                                                                           *
# * Copyright (C) 2003-2008 Cedric Bastoul                                    *
# *                                                                           *
# * This is free software; you can redistribute it and/or modify it under the *
# * terms of the GNU General Public License as published by the Free Software *
# * Foundation; either version 2 of the License, or (at your option) any      *
# * later version.							      *
# *                                                                           *
# * This software is distributed in the hope that it will be useful, but      *
# * WITHOUT ANY WARRANTY; without even the implied warranty of                *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
# * Public License for more details.                                          *
# *                                                                           *
# * You should have received a copy of the GNU General Public License along   *
# * with software; if not, write to the Free Software Foundation, Inc.,       *
# * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                    *
# *                                                                           *
# * CAnDL, the Chunky Dependence Analyser                                     *
# * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                        *
# *                                                                           *
# *****************************************************************************/


output=0
TEST_FILES="$2";
echo "[CHECK:] $1";
for i in $TEST_FILES; do
    outtemp=0
    echo "[TEST:] Dependence analyzer test:== $i.candl ==";
    $top_builddir/source/candl $i.candl > $i.candltest 2>/tmp/clanout
    z=`diff $i.candltest $i.dep 2>&1`
    err=`cat /tmp/clanout`;
    if ! [ -z "$z" ]; then
	echo -e "\033[31m[FAIL:] Dependence analyzer: Error in dependence computation\033[0m";
	outtemp=1;
    fi
    if ! [ -z "$err" ]; then
	if [ $outtemp = "0" ]; then
	    echo "[INFO:] Dependence analyzer: OK";
	fi
	echo -e "\033[31m[FAIL:] Dependence analyzer: stderr output: $err\033[0m";
	outtemp=1
	output=1
    fi
    if [ $outtemp = "0" ]; then
	echo "[PASS:] Dependence analyzer: OK";
	rm -f $i.candltest
    fi
    rm -f /tmp/clanout
    echo "[TEST:] Dependence analyzer test:== $i.scop ==";
    $top_builddir/source/candl -scop $i.scop > $i.scoptest 
    $top_builddir/source/candl -scop -structure $i.scop > $i.structest 
    z=`diff $i.scoptest $i.dep`
    y=`diff $i.structest $i.struct`
    if ! [ -z "$z" ] || ! [ -z "$y" ]; then
	echo -e "\033[31m[FAIL:] Dependence analyzer: Error in dependence computation\033[0m";
	outtemp=1
	output=1
    else
	echo "[PASS:] Dependence analyzer: OK";
	rm -f $i.scoptest
	rm -f $i.structest
    fi
done
if [ $output = "1" ]; then
    echo -e "\033[31m[FAIL:] $1\033[0m";
else
    echo "[PASS:] $1";
fi
exit $output
