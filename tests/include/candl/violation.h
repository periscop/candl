
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                  violation.h                            **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: december 12th 2005               **
    **--- |"-.-"| -------------------------------------------------------**
          |     |
          |     |
 ******** |     | *************************************************************
 * CAnDL  '-._,-' the Chunky Analyzer for Dependences in Loops (experimental) *
 ******************************************************************************
 *                                                                            *
 * Copyright (C) 2005-2008 Cedric Bastoul                                     *
 *                                                                            *
 * This is free software; you can redistribute it and/or modify it under the  *
 * terms of the GNU General Public License as published by the Free Software  *
 * Foundation; either version 2 of the License, or (at your option) any later *
 * version.                                                                   *
 *                                                                            *
 * This software is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * for more details.                                                          *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with software; if not, write to the Free Software Foundation, Inc.,        *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                     *
 *                                                                            *
 * CAnDL, the Chunky Dependence Analyzer                                      *
 * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                         *
 *                                                                            *
 ******************************************************************************/


#ifndef CANDL_VIOLATION_H
# define CANDL_VIOLATION_H

# include <stdio.h>
# include <candl/dependence.h>
# include <osl/scop.h>
# include <osl/relation.h>

# if defined(__cplusplus)
extern "C"
  {
# endif


/**
 * CandlViolation structure:
 * this structure contains all informations about a data dependence violation.
 */
struct candl_violation {
  candl_dependence_p dependence; /**< Pointer to violated dependence. */
  int dimension;                 /**< Violation dimension. */
  osl_relation_p domain;         /**< Violation polyhedron. */
  struct candl_violation *next;        /**< Pointer to next violation. */
  
  int source_nb_output_dims_scattering;
  int target_nb_output_dims_scattering;
  int source_nb_local_dims_scattering;
  int target_nb_local_dims_scattering;
};
typedef struct candl_violation  candl_violation_t;
typedef struct candl_violation* candl_violation_p;


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/
void candl_violation_idump(FILE *, candl_violation_p, int);
void candl_violation_dump(FILE *, candl_violation_p);
void candl_violation_pprint(FILE *, candl_violation_p);
void candl_violation_view(candl_violation_p);


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/
void candl_violation_free(candl_violation_p);


/******************************************************************************
 *                             Processing functions                           *
 ******************************************************************************/
candl_violation_p candl_violation_malloc();
void              candl_violation_add(candl_violation_p*, candl_violation_p*,
                                      candl_violation_p);
candl_violation_p candl_violation(osl_scop_p, candl_dependence_p,
                                  osl_scop_p, candl_options_p);


# if defined(__cplusplus)
  }
# endif
#endif /* define CANDL_VIOLATION_H */

