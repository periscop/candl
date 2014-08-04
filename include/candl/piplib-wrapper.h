
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
 * \file piplib-wrapper.h
 * \author Louis-Noel Pouchet
 */


#ifndef CANDL_PIPLIB_WRAPPER_H
# define CANDL_PIPLIB_WRAPPER_H

# if defined(__cplusplus)
extern "C"
  {
# endif

struct osl_relation;
struct pipmatrix;
struct pipquast;
struct pipoptions;
struct piplist;

struct pipmatrix*    pip_relation2matrix(struct osl_relation*);
struct osl_relation* pip_matrix2relation(struct pipmatrix*);
int                  pip_has_rational_point(struct osl_relation*,
                         struct osl_relation*, int);
struct pipquast*     pip_solve_osl(struct osl_relation*, struct osl_relation*,
                         int, struct pipoptions*);
int                  piplist_are_equal(struct piplist*, struct piplist*, int);
struct osl_relation* pip_quast_to_polyhedra(struct pipquast*, int, int);
struct osl_relation* pip_quast_no_solution_to_polyhedra(struct pipquast*,
                         int, int);

# if defined(__cplusplus)
  }
# endif
#endif /* define CANDL_PIPLIB_WRAPPER_H */

