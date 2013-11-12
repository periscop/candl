#
#   /**-------------------------------------------------------------------**
#    **                              CAnDL                                **
#    **-------------------------------------------------------------------**
#    **                           Makefile.am                             **
#    **-------------------------------------------------------------------**
#    **                 First version: june 28th 2012                     **
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

############################################################################
SUBDIRS 		=

#############################################################################

MAINTAINERCLEANFILES    = Makefile.in

TESTS_ENVIRONMENT       = top_builddir=$(top_builddir) SRCDIR=$(srcdir)

#############################################################################

check_SCRIPTS		= check_unitary.sh check_fail.sh check_working.sh

EXTRA_DIST              = checker.sh

## Replace by program names when needed
# check_PROGRAMS = ...
TESTS			= $(check_SCRIPTS)

UNITARY_TEST_FILES = \
	$(top_srcdir)/tests/unitary/ax-do.c \
	$(top_srcdir)/tests/unitary/can_reorder_loop.c \
	$(top_srcdir)/tests/unitary/gemver.c \
	$(top_srcdir)/tests/unitary/jacobi1d.c \
	$(top_srcdir)/tests/unitary/loulou.c \
	$(top_srcdir)/tests/unitary/lu.c \
	$(top_srcdir)/tests/unitary/matmul.c \
	$(top_srcdir)/tests/unitary/multiscop3.c \
	$(top_srcdir)/tests/unitary/scalpriv.c \
	$(top_srcdir)/tests/unitary/scalexp.c \
	$(top_srcdir)/tests/unitary/simple.c \
	$(top_srcdir)/tests/unitary/swim.c 

TRANSFO_FAIL_TEST_FILES = \
	$(top_srcdir)/tests/transformations/must_fail/no_scalren.c

TRANSFO_WORKING_TEST_FILES = \
	$(top_srcdir)/tests/transformations/working/can_reorder_loop.c \
	$(top_srcdir)/tests/transformations/working/loulou.c \
	$(top_srcdir)/tests/transformations/working/scalren.c

UNITARY_TEST_FILES_REF = $(UNITARY_TEST_FILES:.c=.c.scop)
TRANSFO_FAIL_TEST_FILES_REF = $(TRANSFO_FAIL_TEST_FILES:.c=.c.scop)
TRANSFO_WORKING_TEST_FILES_REF = $(TRANSFO_WORKING_TEST_FILES:.c=.c.scop)

EXTRA_DIST += \
	$(UNITARY_TEST_FILES) \
	$(TRANSFO_FAIL_TEST_FILES) \
	$(TRANSFO_WORKING_TEST_FILES) \
	$(check_SCRIPTS)

CLEANFILES = ""

TESTS_ENVIRONMENT += \
	CHECKER="$(top_srcdir)/tests/checker.sh" \
	UNITARY_TEST_FILES="$(UNITARY_TEST_FILES)" \
  TRANSFO_FAIL_TEST_FILES="$(TRANSFO_FAIL_TEST_FILES)" \
  TRANSFO_WORKING_TEST_FILES="$(TRANSFO_WORKING_TEST_FILES)"
