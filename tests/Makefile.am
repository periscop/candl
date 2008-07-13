#
#   /**-------------------------------------------------------------------**
#    **                              CAnDL                                **
#    **-------------------------------------------------------------------**
#    **                           Makefile.am                             **
#    **-------------------------------------------------------------------**
#    **                 First version: september 8th 2003                 **
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


#############################################################################
SUBDIRS 		=

#############################################################################

MAINTAINERCLEANFILES    = Makefile.in

TESTS_ENVIRONMENT       = top_builddir=$(top_builddir) SRCDIR=$(srcdir)

#############################################################################

EXTRA_DIST              = checker.sh 

check_SCRIPTS		= check_unitary.sh check_complex.sh 

## Replace by program names when needed
# check_PROGRAMS = ...
TESTS			= $(check_SCRIPTS)

TEST_FILES             	= 			\
	ax-do.c					\
	gauss.c					\
	gemver.c				\
	iir_4_64.c				\
	matmul.c				\
	swim.c

UNITARY_TEST_FILES             	=		\
	unitary/classicloop.c   		\
	unitary/simpleloop.c			\
	unitary/basicnest.c     		\
	unitary/empty.c         		\
	unitary/simplecond.c			\
	unitary/assigniter.c    		\
	unitary/comments.c      		\
	unitary/increment.c


TEST_FILES_REF    =		 $(TEST_FILES:.c=.c.scop)
UNITARY_TEST_FILES_REF		= $(UNITARY_TEST_FILES:.c=.c.scop)

TEST_FILES_CLEAN		= $(UNITARY_TEST_FILES:.c=.c.test)
UNITARY_TEST_FILES_CLEAN	= $(UNITARY_TEST_FILES:.c=.c.test)


CLEANFILES			=		\
	$(TEST_FILES_CLEAN)			\
	$(UNITARY_TEST_FILES_CLEAN)


EXTRA_DIST	+=				\
	$(TEST_FILES)				\
	$(TEST_FILES_REF)			\
	$(UNITARY_TEST_FILES)			\
	$(UNITARY_TEST_FILES_REF)		\
	$(check_SCRIPTS)	


TESTS_ENVIRONMENT += 					\
	TEST_FILES="$(TEST_FILES)" 			\
	UNITARY_TEST_FILES="$(UNITARY_TEST_FILES)"
