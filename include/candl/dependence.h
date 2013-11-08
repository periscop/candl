
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                  dependence.h                           **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: september 18th 2003              **
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


#ifndef CANDL_DEPENDENCE_H
# define CANDL_DEPENDENCE_H


# include <stdio.h>
# include <osl/scop.h>
# include <osl/statement.h>
# include <osl/relation.h>
# include <osl/extensions/dependence.h>
# include <candl/matrix.h>
# include <candl/options.h>


# define CANDL_ARRAY_BUFF_SIZE	 2048
# define CANDL_VAR_UNDEF		     1
# define CANDL_VAR_IS_DEF		     2
# define CANDL_VAR_IS_USED		   3
# define CANDL_VAR_IS_DEF_USED	 4


# if defined(__cplusplus)
extern "C"
  {
# endif



#ifdef CANDL_SUPPORTS_ISL
osl_dependence_p candl_dependence_isl_simplify(osl_dependence_p,
                                                 osl_scop_p);
# endif


/*+***************************************************************************
 *                          Structure display function                       *
 *****************************************************************************/
void             candl_dependence_pprint(FILE *, osl_dependence_p);
void             candl_dependence_view(osl_dependence_p);


/******************************************************************************
 *                             Processing functions                           *
 ******************************************************************************/
int              candl_dependence_gcd_test(osl_statement_p,
                                           osl_statement_p,
                                           osl_relation_p, int);
int              candl_dependence_check(osl_scop_p,
                                        osl_dependence_p,
                                        candl_options_p);
osl_dependence_p candl_dependence(osl_scop_p, candl_options_p);
void             candl_dependence_add_extension(osl_scop_p, candl_options_p);


/*+***************************************************************************
 *                    Memory allocation/deallocation function                *
 *****************************************************************************/
void             candl_dependence_init_fields(osl_scop_p, osl_dependence_p);


/******************************************************************************
 *                          Scalar analysis functions                         *
 ******************************************************************************/
int              candl_dependence_var_is_scalar (osl_scop_p, int);
osl_statement_p* candl_dependence_refvar_chain(osl_scop_p, osl_statement_p,
                                               int, int);
int              candl_dependence_var_is_ref(osl_statement_p, int);
int              candl_dependence_check_domain_is_included(osl_statement_p,
                                     osl_statement_p, osl_relation_p, int);
int              candl_dependence_scalar_is_privatizable_at(osl_scop_p,
                                                            int, int);
int              candl_dependence_is_loop_carried(osl_scop_p, osl_dependence_p,
                                                  int);
void             candl_dependence_prune_scalar_waw(osl_scop_p, candl_options_p,
                                                   osl_dependence_p*);
void             candl_dependence_prune_with_privatization(osl_scop_p,
                                     candl_options_p, osl_dependence_p*);
int              candl_dependence_scalar_renaming(osl_scop_p, candl_options_p,
                                                  osl_dependence_p*);
int              candl_dependence_analyze_scalars(osl_scop_p, candl_options_p);

/******************************************************************************
 *                          Miscellaneous functions                           *
 ******************************************************************************/
osl_relation_p   candl_dependence_get_relation_ref_source_in_dep(osl_dependence_p);
osl_relation_p   candl_dependence_get_relation_ref_target_in_dep(osl_dependence_p);
int              candl_num_dependences(osl_dependence_p);
void             candl_compute_last_writer(osl_dependence_p, osl_scop_p);
osl_dependence_p candl_dependence_prune_transitively_covered(osl_dependence_p);

# if defined(__cplusplus)
  }
# endif
#endif /* define CANDL_DEPENDENCE_H */

