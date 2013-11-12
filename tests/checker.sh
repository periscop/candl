#!/bin/sh

#
#   /**-------------------------------------------------------------------**
#    **                              CAnDL                                **
#    **-------------------------------------------------------------------**
#    **                           Makefile.am                             **
#    **-------------------------------------------------------------------**
#    **                 First version: june 29th 2012                     **
#    **-------------------------------------------------------------------**/
#
#/*****************************************************************************
# *   CAnDL : the Chunky Analyser for Dependences in Loops (experimental)     *
# *****************************************************************************
# *                                                                           *
# * Copyright (C) 2003-2008 Cedric Bastoul                                    *
# *                                                                           *
# * This is free software; you can redistribute it and/or modify it under the *
# * terms of the GNU Lesser General Public License as published by the Free   *
# * Software Foundation; either version 3 of the License, or (at your option) *
# * any later version.							      *
# *                                                                           *
# * This software is distributed in the hope that it will be useful, but      *
# * WITHOUT ANY WARRANTY; without even the implied warranty of                *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
# * Public License for more details.                                          *
# *                                                                           *
# * You should have received a copy of the GNU Lesser General Public License  *
# * along with software; if not, write to the Free Software Foundation, Inc., *
# * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                    *
# *                                                                           *
# * CAnDL, the Chunky Dependence Analyser                                     *
# * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                        *
# *                                                                           *
# *****************************************************************************/

STRING=$1
FILES=$2
TRANSFO=$3

set -x verbose  #echo on

echo "$STRING"

for name in $FILES; do
  echo "check $name \c"

  orig_scop="$name.orig.scop"
  struct="$name.struct"
  clay_scop="$name.clay.scop"  # only for transformations tests

  # read candl options
  candloptions=`grep "candl options" "$name" | cut -d'|' -f2`

  case $TRANSFO in
    0)
      ../candl $candloptions "$orig_scop" -struct | grep -v "enerated by" > /tmp/candl_struct
      ;;
    1)
      ../candl $candloptions "$clay_scop" -test "$orig_scop" -struct | grep -v "enerated by" >/tmp/candl_struct
      ;;
  esac
  
  n=`diff /tmp/candl_struct "$struct" | wc -l`
  if [ $n -ne 0 ]; then
    echo "\033[31m[ FAIL ]\033[0m"
  else
    echo "\033[32m[ OK ]\033[0m"
  fi

  rm -f /tmp/candl_struct
done
