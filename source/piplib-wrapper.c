
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (               piplib-wrapper.c                          **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: January 31st 2012                **
    **--- |"-.-"| -------------------------------------------------------**
          |     |
          |     |
 ******** |     | *************************************************************
 * CAnDL  '-._,-' the Chunky Analyzer for Dependences in Loops (experimental) *
 ******************************************************************************
 *                                                                            *
 * Copyright (C) 2003-2008 Cedric Bastoul                                     *
 *                                                                            *
 * This is free software; you can redistribute it and/or modify it under the  *
 * terms of the GNU Lesser General Public License as published by the Free    *
 * Software Foundation; either version 3 of the License, or (at your option)  *
 * any later version.                                                         *
 *                                                                            *
 * This software is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * for more details.                                                          *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with software; if not, write to the Free Software Foundation, Inc.,  *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                     *
 *                                                                            *
 * CAnDL, the Chunky Dependence Analyzer                                      *
 * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                         *
 *                                                                            *
 ******************************************************************************/
/**
 * \file piplib-wrapper.c
 * \author Louis-Noel Pouchet
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <candl/candl.h>


int
pip_has_rational_point(PipMatrix* system,
		       PipMatrix* context,
		       int conservative)
{
#ifdef CANDL_HAS_PIPLIB_HYBRID
  return piplib_hybrid_has_rational_point(system, context, conservative);
#else
  PipOptions* options;
  int ret = 0;
  options = pip_options_init ();
  options->Simplify = 1;
  options->Urs_parms = -1;
  options->Urs_unknowns = -1;
  options->Nq = 0;
  PipQuast* solution = pip_solve (system, context, -1, options);
  if ((solution != NULL) &&
      ((solution->list != NULL) || (solution->condition != NULL)))
    ret = 1;
  pip_options_free (options);
  pip_quast_free (solution);
  return ret;
#endif
}

