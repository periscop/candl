
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
# include <candl/matrix.h>
# include <candl/options.h>


# define CANDL_ARRAY_BUFF_SIZE		2048
# define CANDL_VAR_UNDEF		1
# define CANDL_VAR_IS_DEF		2
# define CANDL_VAR_IS_USED		3
# define CANDL_VAR_IS_DEF_USED		4


# if defined(__cplusplus)
extern "C"
  {
# endif


/**
 * candl_dependence structure:
 * this structure contains all the informations about a data dependence, it is
 * also a node of the linked list of all dependences of the dependence graph.
 
 
  Dependence domain structure
                          ———————————————————————————————————————————————————————————————— 
                        / source (output) | target (input)  |          local dims          \ 
                     —— |—————————————————|—————————————————|———————————————————————————————|—————————————
                   / eq | output | output | output | output |ld dom |ld acc |ld dom |ld acc |           |  \ 
                   | in | domain | access | domain | access |source |source |target |target |parameters | 1 |
   ————————————————|————|————————|————————|————————|————————|———————|———————|———————|———————|———————————|———|
  |Domain source   | X  |   X    :        |        :        |   X   :       |       :       |     X     | X |
  |————————————————|————|————————:————————|————————:————————|———————:———————|———————:———————|———————————|———|
  |Domain target   | X  |        :        |    X   :        |       :       |   X   :       |     X     | X |
  |————————————————|————|————————:————————|————————:————————|———————:———————|———————:———————|———————————|———|
  |Access source   | X  |   X    :   X    |        :        |       :   X   |       :       |     X     | X |
  |————————————————|————|————————:————————|————————:————————|———————:———————|———————:———————|———————————|———| 
  |Access target   | X  |        :        |    X   :   X    |       :       |       :   X   |     X     | X |
  |————————————————|————|————————:————————|————————:————————|———————:———————|———————:———————|———————————|———|
  |Access equality |    |        :  Id    |        :  -Id   |       :       |       :       |           |   |
  |————————————————|————|————————:————————|————————:————————|———————:———————|———————:———————|———————————|———|    | 0    : 0..depth-1 
  |Precedence      | X  |  Id    :        |   -Id  :        |       :       |       :       |           | X | <--| 0|-1 : depth
  \————————————————|————|————————:————————|————————:————————|———————:———————|———————:———————|———————————|———/
 
                           (1)      (2)      (3)      (4)      (5)     (6)     (7)     (8)
 */
struct candl_dependence {
  osl_statement_p source;      /**< Pointer to source statement. */
  osl_statement_p target;      /**< Pointer to target statement. */
  int depth;                   /**< Dependence level. */
  int type;                    /**< Dependence type: a dependence from source
                                 *   to target can be:
				                         *   - CANDL_UNSET if the dependence type is
				                         *     still not set,
				                         *   - CANDL_RAW if source writes M and
				                         *     target read M (flow-dependence),
				                         *   - CANDL_WAR if source reads M and
				                         *     target writes M (anti-dependence),
				                         *   - CANDL_WAW if source writes M and
				                         *     target writes M too (output-dependence)
				                         *   - CANDL_RAR if source reads M and
				                         *     target reads M too (input-dependence).
				                         */
  int ref_source;                /**< Position of source reference in the array access list. */
  int ref_target;                /**< Position of target reference in the array access list. */
  osl_relation_p domain;         /**< Dependence polyhedron. */
  
  int source_nb_output_dims_domain; // (1)
  int source_nb_output_dims_access; // (2)
  
  int target_nb_output_dims_domain; // (3)
  int target_nb_output_dims_access; // (4)
  
  int source_nb_local_dims_domain; // (5)
  int source_nb_local_dims_access; // (6)
  int target_nb_local_dims_domain; // (7)
  int target_nb_local_dims_access; // (8)

  void* usr;			 /**< User field, for library users
				    convenience. */
  struct candl_dependence * next; /**< Pointer to next dependence */
  
  /* If it's not null the function candl_dependence_get_relation_ref_*_in_dep
   * return these pointer
   * else the pointer will be computed
   */
  osl_relation_p cache_ref_source_ptr;
  osl_relation_p cache_ref_target_ptr;
};

typedef struct candl_dependence candl_dependence_t;
typedef struct candl_dependence *candl_dependence_p;


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/
void candl_dependence_idump(FILE *, candl_dependence_p, int);
void candl_dependence_dump(FILE *, candl_dependence_p);
void candl_dependence_sprint(char **, candl_dependence_p);
void candl_dependence_print(FILE *, candl_dependence_p);
void candl_dependence_pprint(FILE *, candl_dependence_p);
void candl_dependence_view(candl_dependence_p);

candl_dependence_p candl_dependence_read_from_scop(osl_scop_p, char *);

#ifdef CANDL_SUPPORTS_ISL
candl_dependence_p candl_dependence_isl_simplify(candl_dependence_p,
                                                 osl_scop_p);
# endif


/******************************************************************************
 *                         Memory alloc/dealloc function                      *
 ******************************************************************************/
candl_dependence_p      candl_dependence_malloc();
void			              candl_dependence_free(candl_dependence_p);


/******************************************************************************
 *                             Processing functions                           *
 ******************************************************************************/
int                  candl_dependence_gcd_test(osl_statement_p,
						                                   osl_statement_p,
						                                   osl_relation_p, int);
int			             candl_dependence_check(osl_scop_p,
					                                  candl_dependence_p,
					                                  candl_options_p);
candl_dependence_p   candl_dependence(osl_scop_p, candl_options_p);


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
int              candl_dependence_is_loop_carried(osl_scop_p, candl_dependence_p,
                                                  int);
void             candl_dependence_prune_scalar_waw(osl_scop_p, candl_options_p,
                                                   candl_dependence_p*);
void             candl_dependence_prune_with_privatization(osl_scop_p,
                                     candl_options_p, candl_dependence_p*);
int              candl_dependence_scalar_renaming(osl_scop_p, candl_options_p,
                                                  candl_dependence_p*);
int              candl_dependence_analyze_scalars(osl_scop_p, candl_options_p);

/******************************************************************************
 *                          Miscellaneous functions                           *
 ******************************************************************************/
osl_relation_p candl_dependence_get_relation_ref_source_in_dep(candl_dependence_p);
osl_relation_p candl_dependence_get_relation_ref_target_in_dep(candl_dependence_p);
int                candl_num_dependences(candl_dependence_p);
void               candl_compute_last_writer(candl_dependence_p, osl_scop_p);
candl_dependence_p candl_dependence_prune_transitively_covered(candl_dependence_p);
    
# if defined(__cplusplus)
  }
# endif
#endif /* define CANDL_DEPENDENCE_H */

