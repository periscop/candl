#!/bin/sh

#   /*--------------------------------------------------------------------+
#    |                              Clay                                  |
#    |--------------------------------------------------------------------|
#    |                             Clay.c                                 |
#    |--------------------------------------------------------------------|
#    |                    First version: 03/04/2012                       |
#    +--------------------------------------------------------------------+

# +--------------------------------------------------------------------------+
# |  / __)(  )    /__\ ( \/ )                                                |
# | ( (__  )(__  /(__)\ \  /         Chunky Loop Alteration wizardrY         |
# |  \___)(____)(__)(__)(__)                                                 |
# +--------------------------------------------------------------------------+
# | Copyright (C) 2012 University of Paris-Sud                               |
# |                                                                          |
# | This library is free software; you can redistribute it and/or modify it  |
# | under the terms of the GNU Lesser General Public License as published by |
# | the Free Software Foundation; either version 2.1 of the License, or      |
# | (at your option) any later version.                                      |
# |                                                                          |
# | This library is distributed in the hope that it will be useful but       |
# | WITHOUT ANY WARRANTY; without even the implied warranty of               |
# | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser  |
# | General Public License for more details.                                 |
# |                                                                          |
# | You should have received a copy of the GNU Lesser General Public License |
# | along with this software; if not, write to the Free Software Foundation, |
# | Inc., 51 Franklin Street, Fifth Floor,                                   |
# | Boston, MA  02110-1301  USA                                              |
# |                                                                          |
# | Clay, the Chunky Loop Alteration wizardrY                                |
# | Written by Joel Poudroux, joel.poudroux@u-psud.fr                        |
# +--------------------------------------------------------------------------*/


STRING=$1
FILES=$2
CHECK_ERROR=$3

echo "$STRING"

for name in $FILES; do
  echo "check $name \c"
  clay "$name.orig.scop" 2>/tmp/clay_scop_stderr | grep -v "enerated by" >/tmp/clay_scop
  
  n=0
  if [ "$CHECK_ERROR" = "0" ]; then
    n=`diff "$name.clay.scop" /tmp/clay_scop | wc -l`
  else
    n=`diff "$name.clay.scop" /tmp/clay_scop_stderr | wc -l`
  fi

  if [ $n -ne 0 ]; then
    echo "\n\033[31m[ FAIL ] \c"
    cat /tmp/clay_scop_stderr
    echo "\033[0m"
  else
    echo "\033[32m[ OK ]\033[0m"
  fi

  rm -f /tmp/clay_scop_stderr
  rm -f /tmp/clay_scop
done
