
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
#include <osl/relation.h>
#include <osl/macros.h> /* Need OSL_PRECISION for compatibility with piplib */
#include <candl/matrix.h>
#include <candl/piplib-wrapper.h>


/**
 * pip_relation2matrix function :
 * This function is used to keep the compatibility with Piplib
 */
PipMatrix* pip_relation2matrix(osl_relation_p in) {
  if (in == NULL)
    return NULL;
    
  #if defined(LINEAR_VALUE_IS_INT)
    if (OSL_PRECISION_SP != in->precision)
      CANDL_FAIL("Precision not compatible with piplib ! (pip_relation2matrix)");
  #elif defined(LINEAR_VALUE_IS_LONGLONG)
    if (OSL_PRECISION_DP != in->precision)
      CANDL_FAIL("Precision not compatible with piplib ! (pip_relation2matrix)");
  #elif defined(LINEAR_VALUE_IS_MP)
    if (OSL_PRECISION_MP != in->precision)
      CANDL_FAIL("Precision not compatible with piplib ! (pip_relation2matrix)");
  #endif
  
  PipMatrix *out;
  int i, j;
  
  out = pip_matrix_alloc(in->nb_rows, in->nb_columns);
  
  for (i = 0 ; i < in->nb_rows ; i++) {
    for (j = 0 ; j < in->nb_columns ; j++) {
      #if defined(LINEAR_VALUE_IS_INT)
        osl_int_assign(OSL_PRECISION_SP,
                       out->p[i], j,
                       in->m[i], j);
      #elif defined(LINEAR_VALUE_IS_LONGLONG)
        osl_int_assign(OSL_PRECISION_DP,
                       out->p[i], j,
                       in->m[i], j);
      #elif defined(LINEAR_VALUE_IS_MP)
        osl_int_assign(OSL_PRECISION_MP,
                       out->p[i], j,
                       in->m[i], j);
      #endif
    }
  }
  
  return out;
}


/**
 * pip_matrix2relation function :
 * This function is used to keep the compatibility with Piplib
 */
osl_relation_p pip_matrix2relation(PipMatrix* in) {
  if (in == NULL)
    return NULL;
  
  int precision;
  
  #if defined(LINEAR_VALUE_IS_INT)
    precision = OSL_PRECISION_SP;
  #elif defined(LINEAR_VALUE_IS_LONGLONG)
    precision = OSL_PRECISION_DP;
  #elif defined(LINEAR_VALUE_IS_MP)
    precision = OSL_PRECISION_MP;
  #endif
  
  osl_relation_p out;
  int i, j;
  
  out = osl_relation_pmalloc(precision, in->NbRows, in->NbColumns);
  
  for (i = 0 ; i < in->NbRows ; i++) {
    for (j = 0 ; j < in->NbColumns ; j++) {
      #if defined(LINEAR_VALUE_IS_INT)
        osl_int_assign(OSL_PRECISION_SP,
                       out->m[i], j,
                       in->p[i], j);
      #elif defined(LINEAR_VALUE_IS_LONGLONG)
        osl_int_assign(OSL_PRECISION_DP,
                       out->m[i], j,
                       in->p[i], j);
      #elif defined(LINEAR_VALUE_IS_MP)
        osl_int_assign(OSL_PRECISION_MP,
                       out->m[i], j,
                       in->p[i], j);
      #endif
    }
  }
  
  return out;
}


int
pip_has_rational_point(osl_relation_p system,
                       osl_relation_p context,
                       int conservative) {
// FIXME : compatibility with osl
//#ifdef CANDL_HAS_PIPLIB_HYBRID
//  return piplib_hybrid_has_rational_point(system, context, conservative);
//#else
  PipOptions* options;
  int ret = 0;
  options = pip_options_init ();
  options->Simplify = 1;
  options->Urs_parms = -1;
  options->Urs_unknowns = -1;
  options->Nq = 0;
  PipQuast* solution = pip_solve_osl(system, context, -1, options);
  if ((solution != NULL) &&
      ((solution->list != NULL) || (solution->condition != NULL)))
    ret = 1;
  pip_options_free(options);
  pip_quast_free(solution);
  return ret;
//#endif
}


/**
 * pip_solve_osl function :
 * A pip_solve with osl_relation_p instead of PipMatrix
 */
PipQuast* pip_solve_osl(osl_relation_p inequnk, osl_relation_p ineqpar,
                        int Bg, PipOptions *options) {
  PipMatrix *pip_unk  = pip_relation2matrix(inequnk);
  PipMatrix *pip_par  = pip_relation2matrix(ineqpar);
  PipQuast  *solution = pip_solve(pip_unk, pip_par, Bg, options);
  if (pip_unk) pip_matrix_free(pip_unk);
  if (pip_par) pip_matrix_free(pip_par);
  return solution;
}


/**
 * Return true if the 'size' first elements of 'l1' and 'l2' are equal.
 */
int piplist_are_equal(PipList* l1, PipList* l2, int size) {
  if (l1 == NULL && l2 == NULL)
    return 1;
  if (l1 == NULL || l2 == NULL)
    return 0;
  if (l1->vector == NULL && l2->vector == NULL)
    return 1;
  if (l1->vector == NULL || l2->vector == NULL)
    return 0;

  int count = 0;
  for (; l1 && l2 && count < size; l1 = l1->next, l2 = l2->next, ++count) {
    if (l1->vector == NULL && l2->vector == NULL)
      return 1;
    if (l1->vector == NULL || l2->vector == NULL)
      return 0;
    if (l1->vector->nb_elements != l2->vector->nb_elements)
      return 0;
    int j;
    for (j = 0; j < l1->vector->nb_elements; ++j)
      if (! CANDL_eq(l1->vector->the_vector[j],
                     l2->vector->the_vector[j]) ||
          ! CANDL_eq(l1->vector->the_deno[j],
                     l2->vector->the_deno[j]))
        return 0;
  }

  return 1;
}
