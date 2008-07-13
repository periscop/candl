
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                  dependence.c                           **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: september 18th 2003              **
    **--- |"-.-"| -------------------------------------------------------**
          |     |
          |     |
 ******** |     | *************************************************************
 * CAnDL  '-._,-' the Chunky Analyzer for Dependences in Loops (experimental) *
 ******************************************************************************
 *                                                                            *
 * Copyright (C) 2003 Cedric Bastoul                                          *
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
/* CAUTION: the english used for comments is probably the worst you ever read,
 *          please feel free to correct and improve it !
 */

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "../include/candl/candl.h"


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/


/**
 * candl_dependence_print_structure function:
 * Displays a CandlDependence structure (dependence) into a file (file,
 * possibly stdout) in a way that trends to be understandable without falling
 * in a deep depression or, for the lucky ones, getting a headache... It
 * includes an indentation level (level) in order to work with others
 * print_structure functions.
 * - 18/09/2003: first version.
 */
void candl_dependence_print_structure(file, dependence, level)
FILE * file ;
CandlDependence * dependence ;
int level ;
{ int j, first=1 ;

  if (dependence != NULL)
  { /* Go to the right level. */
    for(j=0; j<level; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"+-- CandlDependence\n") ;
  }
  else
  { for(j=0; j<level; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"+-- NULL dependence\n") ;
  }

  while (dependence != NULL)
  { if (!first)
    { /* Go to the right level. */
      for(j=0; j<level; j++)
      fprintf(file,"|\t") ;
      fprintf(file,"|   CandlDependence\n") ;
    }
    else
    first = 0 ;

    /* A blank line. */
    for(j=0; j<=level+1; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"\n") ;

    /* Go to the right level and print the type. */
    for(j=0; j<=level; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"Type: ") ;
    switch (dependence->type)
    { case CANDL_UNSET : fprintf(file,"UNSET\n") ;        break ;
      case CANDL_RAW   : fprintf(file,"RAW (flow)\n") ;   break ;
      case CANDL_WAR   : fprintf(file,"WAR (anti)\n") ;   break ;
      case CANDL_WAW   : fprintf(file,"WAW (output)\n") ; break ;
      case CANDL_RAR   : fprintf(file,"RAR (input)\n") ;  break ;
      default : fprintf(file,"unknown\n") ;
    }

    /* A blank line. */
    for(j=0; j<=level+1; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"\n") ;

    /* Go to the right level and print the depth. */
    for(j=0; j<=level; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"Depth: %d\n",dependence->depth) ;

    /* A blank line. */
    for(j=0; j<=level+1; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"\n") ;

    /* Print the source statement. */
    candl_statement_print_structure(file,dependence->source,level+1) ;

    /* Print the target statement. */
    candl_statement_print_structure(file,dependence->target,level+1) ;

    /* Print the dependence polyhedron. */
    candl_matrix_print_structure(file,dependence->domain,level+1) ;

    dependence = dependence->next ;

    /* Next line. */
    if (dependence != NULL)
    { for (j=0; j<=level; j++)
      fprintf(file,"|\t") ;
      fprintf(file,"V\n") ;
    }
  }

  /* The last line. */
  for(j=0; j<=level; j++)
  fprintf(file,"|\t") ;
  fprintf(file,"\n") ;
}


/* candl_dependence_print function:
 * This function prints the content of a CandlDependence structure (dependence)
 * into a file (file, possibly stdout).
 */
void candl_dependence_print(FILE * file, CandlDependence * dependence)
{ candl_dependence_print_structure(file,dependence,0) ;
}


/* candl_dependence_pprint function:
 * This function prints the content of a CandlDependence structure (dependence)
 * into a file (file, possibly stdout) as a Graphviz input file.
 * See http://www.graphviz.org
 * - 08/12/2005: first version.
 */
void candl_dependence_pprint(FILE * file, CandlDependence * dependence)
{ int i=0 ;

  fprintf(file,"digraph G {\n") ;

  fprintf(file,"# Data Dependence Graph\n") ;
  fprintf(file,"# Generated by Candl "CANDL_RELEASE" "CANDL_VERSION" bits\n");
  while (dependence != NULL)
  { fprintf(file,"  S%d -> S%d [label=\" ",dependence->source->label,
                                           dependence->target->label) ;
    switch (dependence->type)
    { case CANDL_UNSET : fprintf(file,"UNSET") ; break ;
      case CANDL_RAW   : fprintf(file,"RAW")   ; break ;
      case CANDL_WAR   : fprintf(file,"WAR")   ; break ;
      case CANDL_WAW   : fprintf(file,"WAW")   ; break ;
      case CANDL_RAR   : fprintf(file,"RAR")   ; break ;
      default : fprintf(file,"unknown") ;
    }
    fprintf(file," depth %d, ref %d->%d \"];\n",dependence->depth,
                                                dependence->ref_source,
                                                dependence->ref_target) ;
    dependence = dependence->next ;
    i++ ;
  }

  if (i>4)
  fprintf(file,"# Number of edges = %i\n}\n",i) ;
  else
  fprintf(file,"}\n") ;
}


/* candl_dependence_view function:
 * This function uses dot (see http://www.graphviz.org) and gv (see
 * http://wwwthep.physik.uni-mainz.de/~plass/gv) tools to display the
 * dependence graph.
 * - 20/03/2006: first version.
 */
void candl_dependence_view(CandlDependence * dependence)
{ FILE * temp_output ;

  temp_output = fopen(CANDL_TEMP_OUTPUT,"w") ;
  candl_dependence_pprint(temp_output,dependence) ;
  fclose(temp_output) ;
  system("(dot -Tps "CANDL_TEMP_OUTPUT" | gv - &) && rm -f "CANDL_TEMP_OUTPUT) ;
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/* candl_dependence_free function:
 * This function frees the allocated memory for a CandlDependence structure.
 * - 18/09/2003: first version.
 */
void candl_dependence_free(CandlDependence * dependence)
{ CandlDependence * next ;

  while (dependence != NULL)
  { next = dependence->next ;
    candl_matrix_free(dependence->domain) ;
    free(dependence) ;
    dependence = next ;
  }
}


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/


/**
 * candl_dependence_malloc function:
 * This function allocates the memory space for a CandlDependence structure and
 * sets its fields with default values. Then it returns a pointer to the
 * allocated space.
 * - 07/12/2005: first version.
 */
CandlDependence * candl_dependence_malloc()
{ CandlDependence * dependence ;

  /* Memory allocation for the CandlDependence structure. */
  dependence = (CandlDependence *)malloc(sizeof(CandlDependence)) ;
  if (dependence == NULL)
  { fprintf(stderr, "[Candl]ERROR: memory overflow.\n") ;
    exit(1) ;
  }

  /* We set the various fields with default values. */
  dependence->source     = NULL ;
  dependence->target     = NULL ;
  dependence->depth      = CANDL_UNSET ;
  dependence->type       = CANDL_UNSET ;
  dependence->ref_source = CANDL_UNSET ;
  dependence->ref_target = CANDL_UNSET ;
  dependence->domain     = NULL ;
  dependence->next       = NULL ;

  dependence->solved = 0;
  dependence->id = 0;
  dependence->tag = NULL;

  return dependence ;
}


/**
 * candl_dependence_add function:
 * This function adds a CandlDependence structure (dependence) at a given place
 * (now) of a NULL terminated list of CandlDependence structures. The beginning
 * of this list is (start). This function updates (now) to the end of the loop
 * list (loop), and updates (start) if the added element is the first one -that
 * is when (start) is NULL-.
 * - 18/09/2003: first version.
 */
void candl_dependence_add(start, now, dependence)
CandlDependence ** start, ** now, * dependence ;
{ if (dependence != NULL)
  { if (*start == NULL)
    { *start = dependence ;
      *now = *start ;
    }
    else
    { (*now)->next = dependence ;
      *now = (*now)->next ;
    }

    while ((*now)->next != NULL)
    *now = (*now)->next ;
  }
}


/**
 * candl_dependence_system function :
 * this function builds a node of the dependence graph: it studies a dependence
 * between a given statement couple, reference couple, type and depth. If a
 * data dependence actually exists, it returns a dependence structure, it
 * returns NULL otherwise.
 * - source is the source statement,
 * - target is the target statement,
 * - context is the program context (contraints on global parameters),
 * - array_s is the array list for the source,
 * - array_t is the array list for the target,
 * - ref_s is the position of the source reference in array_s,
 * - ref_s is the position of the target reference in array_t,
 * - depth is the dependence depth,
 * - type is the dependence type (RAW, WAW, WAR or RAR).
 ***
 * - 18/09/2003: first version.
 * - 09/12/2005: few corrections and polishing.
 */
CandlDependence * candl_dependence_system(source, target, context, array_s,
                                          array_t, ref_s, ref_t, type, depth)
CandlStatement  * source, * target ;
CandlMatrix * context, * array_s, * array_t ;
int ref_s, ref_t, type, depth ;
{ CandlDependence * dependence=NULL ;
  CandlMatrix * system ;
  PipOptions * options ;
  PipQuast * solution ;

  /* First, a trivial case: for two different statements at depth 0, there is
   * a dependence only if the source is textually before the target.
   */
  if ((source != target) && (depth == 0) && (source->label > target->label))
  { return NULL ;
  }

  /* We build the system of constraints. */
  system = candl_matrix_dependence(source->domain, target->domain,
                                   array_s, array_t, ref_s, ref_t,
				   depth, (source->label >= target->label),
				   context->NbColumns-2) ;

  options = pip_options_init() ;
  options->Simplify = 1 ;
  options->Urs_parms = -1;
  options->Urs_unknowns = -1;
  solution = pip_solve(system,context,-1,options) ;

  if ((solution != NULL) &&
      ((solution->list != NULL) || (solution->condition != NULL)))
  { dependence = candl_dependence_malloc() ;

    /* We set the various fields with corresponding values. */
    dependence->type       = type ;
    dependence->depth      = depth ;
    dependence->source     = source ;
    dependence->target     = target ;
    dependence->ref_source = ref_s ;
    dependence->ref_target = ref_t ;
    dependence->domain     = system ;

    dependence->solved = 0;
    dependence->id = 0;
    dependence->tag = NULL;
  }
  else
  candl_matrix_free(system) ;

  pip_options_free(options) ;
  pip_quast_free(solution) ;

  return dependence ;
}


/**
 * candl_dependence_between function :
 * this function builds the dependence list from the statement "source" to
 * statement "target": it will study the dependence for each reference and for
 * each depth, under a particular context (context) and according to some
 * user options.
 * - 18/09/2003: first version.
 * - 07/12/2005: (debug) correction of depth bounds.
 * - 09/12/2005: We may take commutativity into consideration.
 */
CandlDependence * candl_dependence_between(source, target, context, options)
CandlStatement  * source, * target ;
CandlMatrix * context ;
CandlOptions * options ;
{ int i, j, k, min_depth, max_depth ;
  CandlDependence * new, * dependence=NULL, * now ;

  /* If the statements commute and the user asks to use this information to
   * simplify the dependence graph, we return no dependences.
   */
  if (options->commute && candl_statement_commute(source,target))
  return NULL ;

  /* In the case of a self-dependence, the dependence depth can be as low as 1
   * (not 0 because at depth 0 there is no loop, thus there is only one
   * instance of the statement !) and as high as the statement depth.
   * In the case of different statements, the dependence depth can be as low
   * as 0 and as high as the number of shared loops.
   */
  if (source == target)
  { min_depth = 1 ;
    max_depth = source->depth ;
  }
  else
  { /* Depth 0 is for statements that don't share any loop. */
    min_depth = (source->index[0] == target->index[0]) ? 1 : 0 ;

    max_depth = 0 ;
    while ((max_depth < source->depth) &&
           (max_depth < target->depth) &&
           (source->index[max_depth] == target->index[max_depth]))
    max_depth ++ ;
  }

  /* Flow and output-dependences analysis. */
  for (j=0;j<source->written->NbRows;j++)
  if (CANDL_notzero_p(source->written->p[j][0]))
  { /* Flow-dependences analysis. */
    if (options->raw)
    for (k=0;k<target->read->NbRows;k++)
    if (CANDL_eq(target->read->p[k][0],source->written->p[j][0]))
    { for (i=min_depth;i<=max_depth;i++)
      { new = candl_dependence_system(source,target,context,source->written,
                                      target->read,j,k,CANDL_RAW,i) ;
	candl_dependence_add(&dependence,&now,new) ;
      }
    }
    /* Output-dependences analysis. */
    if (options->waw)
    for (k=0;k<target->written->NbRows;k++)
    if (CANDL_eq(target->written->p[k][0],source->written->p[j][0]))
    { for (i=min_depth;i<=max_depth;i++)
      { new = candl_dependence_system(source,target,context,source->written,
                                      target->written,j,k,CANDL_WAW,i) ;
	candl_dependence_add(&dependence,&now,new) ;
      }
    }
  }

  /* Anti and input-dependences analysis. */
  for (j=0;j<source->read->NbRows;j++)
  if (source->read->p[j][0] != 0)
  { /* Anti-dependences analysis. */
    if (options->war)
    for (k=0;k<target->written->NbRows;k++)
    if (CANDL_eq(target->written->p[k][0],source->read->p[j][0]))
    { for (i=min_depth;i<=max_depth;i++)
      { new = candl_dependence_system(source,target,context,source->read,
                                      target->written,j,k,CANDL_WAR,i) ;
	candl_dependence_add(&dependence,&now,new) ;
      }
    }
    /* Input-dependences analysis. */
    if (options->rar)
    for (k=0;k<target->read->NbRows;k++)
    if (CANDL_eq(target->read->p[k][0],source->read->p[j][0]))
    { for (i=min_depth;i<=max_depth;i++)
      { new = candl_dependence_system(source,target,context,source->read,
                                      target->read,j,k,CANDL_RAR,i) ;
	candl_dependence_add(&dependence,&now,new) ;
      }
    }
  }

  return dependence ;
}



/**
 * Check if there is an integral point in the set of constraints.
 *
 */
static
int
candl_dependence_check_point (CandlMatrix* domain,
			      CandlMatrix* context)
{
  PipOptions* options;
  PipQuast* solution;
  int ret = 0;
  options = pip_options_init ();
  options->Simplify = 1;
  options->Urs_parms = -1;
  options->Urs_unknowns = -1;
  solution = pip_solve (domain, context, -1, options);

  if ((solution != NULL) &&
      ((solution->list != NULL) || (solution->condition != NULL)))
    ret = 1;
  pip_options_free (options);
  pip_quast_free (solution);

  return ret;
}


/**
 * Extract the array index of scalar variables represented in the
 * access matrix.
 *
 */
static
void
candl_dependence_extract_scalar_variables (CandlMatrix* m,
					   Entier** scalars)
{
  int i, j, k;
  Entier val;
  Entier mone;
  CANDL_init(val);
  CANDL_init(mone);
  CANDL_set_si(mone, -1);

  // Collect in matrix.
  for (j = 0; j < m->NbRows; ++j)
    {
      CANDL_assign(val, m->p[j][0]);
      // Check if it is a scalar.
      for (k = 1; k < m->NbColumns && CANDL_zero_p(m->p[j][k]); ++k)
	;
      // No. Remove it in case it was previously recognized as a
      // scalar (for ex. a[0]).
      if (k != m->NbColumns)
	{
 	  for (k = 0; ! CANDL_eq((*scalars)[k], mone)
		 && ! CANDL_eq((*scalars)[k], val); ++k)
	    ;
	  if (! CANDL_eq((*scalars)[k], mone))
	    for (; ! CANDL_eq((*scalars)[k], mone); ++k)
	      CANDL_assign((*scalars)[k], (*scalars)[k + 1]);
	  continue;
	}
      // Yes. Add it to the scalar list, if not already there.
      for (k = 0; ! CANDL_eq((*scalars)[k], mone)
	     && ! CANDL_eq((*scalars)[k], val); ++k)
	;
      if (CANDL_eq((*scalars)[k], mone))
	{
	  // Bufferization.
	  if (k && (k % CANDL_ARRAY_BUFF_SIZE == 0))
	    {
	      *scalars = (Entier*) realloc
		(*scalars, (k + CANDL_ARRAY_BUFF_SIZE) * sizeof(Entier));
	      for (i = 0; i < CANDL_ARRAY_BUFF_SIZE; ++i)
		{
		  CANDL_init((*scalars)[k + i]);
		  CANDL_set_si((*scalars)[k + i], -1);
		}
	    }
	  // Backup the scalar index.
	  CANDL_assign((*scalars)[k], val);
	}
    }
  // Be clean.
  CANDL_clear(val);
  CANDL_clear(mone);
}


/**
 * Expand the scalar variable of index 'scalar' by adding one
 * dimension (x becomes x[0]) to each access matrix refering this
 * variable in the statement list.
 *
 */
static
void
candl_dependence_expand_scalar (CandlStatement** sl,
				Entier scalar)
{
  CandlMatrix* m;
  int i, l, n, j;

  // Iterate on all statements of the list.
  for (i = 0; sl[i] != NULL; ++i)
    {
      // Check if the scalar is referenced in the 'read' access function.
      for (j = 0; j < sl[i]->read->NbRows &&
	     ! CANDL_eq(sl[i]->read->p[j][0], scalar); ++j)
	;
      // It is.
      if (j < sl[i]->read->NbRows)
	{
	  // Add a row to the 'read' matrix, just after the reference
	  // to 'scalar'.
	  m = candl_matrix_malloc (sl[i]->read->NbRows +1,
				   sl[i]->read->NbColumns);
	  for (l = 0; l <= j; ++l)
	    for (n = 0; n < m->NbColumns; ++n)
	      CANDL_assign(m->p[l][n], sl[i]->read->p[l][n]);
	  for (++l; l < m->NbRows; ++l)
	    for (n = 0; n < m->NbColumns; ++n)
	      CANDL_assign(m->p[l][n], sl[i]->read->p[l - 1][n]);
	  for (n = 0; n < m->NbColumns; ++n)
	    CANDL_set_si(m->p[j + 1][n], 0);
	  candl_matrix_free (sl[i]->read);
	  sl[i]->read = m;
	}

      // Same for 'written' access function.
      for (j = 0; j < sl[i]->written->NbRows &&
	     ! CANDL_eq(sl[i]->written->p[j][0], scalar);++j)
	;
      if (j < sl[i]->written->NbRows)
	{
	  m = candl_matrix_malloc (sl[i]->written->NbRows +1,
				   sl[i]->written->NbColumns);
	  for (l = 0; l <= j; ++l)
	    for (n = 0; n < m->NbColumns; ++n)
	      CANDL_assign(m->p[l][n], sl[i]->written->p[l][n]);
	  for (++l; l < m->NbRows; ++l)
	    for (n = 0; n < m->NbColumns; ++n)
	      CANDL_assign(m->p[l][n], sl[i]->written->p[l - 1][n]);
	  for (n = 0; n < m->NbColumns; ++n)
	    CANDL_set_si(m->p[j + 1][n], 0);
	  candl_matrix_free (sl[i]->written);
	  sl[i]->written = m;
	}
    }
}


/**
 * Perform a scalar privatization with "array expansion". If a scalar
 * variable can be privatized, then the according array expansion is
 * performed onto this variable.
 *
 * For example:
 * for (i = 0; i < n; ++i)
 *  {
 *     w = a[i];
 *     for (j = 0; j < n; ++j)
 *        w += a[j];
 *     a[i] = w;
 *  }
 *
 * Becomes, in the array access function definitions:
 * for (i = 0; i < n; ++i)
 *  {
 *     w[i] = a[i];
 *     for (j = 0; j < n; ++j)
 *        w[i] += a[j];
 *     a[i] = w[i];
 *  }
 *
 */
static
void
candl_dependence_privatize_scalar (Entier scalar,
				   CandlStatement** tsl,
				   CandlStatement** sl,
				   int sidx,
				   int depth)
{
  CandlStatement* s;
  int i, j, k;

  // Iterate on all statements of list 'tsl' within the same loop at
  // depth 'depth'.
  for (i = sidx; tsl[i] != NULL &&
	 tsl[i]->index[depth - 1] == tsl[sidx]->index[depth - 1]; ++i)
    {
      s = tsl[i];
      // Check if the scalar is referenced in the 'read' access function.
      for (j = 0; j < s->read->NbRows &&
	     ! CANDL_eq(s->read->p[j][0], scalar); ++j)
	;
      // It is.
      if (j < s->read->NbRows)
	{
	  // If the scalar has already been expanded once, move the
	  // former expansion to the 2nd line of the access
	  // function. For example, for scalar referenced as '1':
	  // id  i  j  n  1
	  //  1  0  1  0  0
	  //  0  0  0  0  0
	  // Becomes:
	  // id  i  j  n  1
	  //  1  0  0  0  0
	  //  0  0  1  0  0
	  if (j + 1 < s->read->NbRows && CANDL_zero_p(s->read->p[j + 1][0]))
	    for (k = 1; k < s->read->NbColumns; ++k)
	      {
		CANDL_assign(s->read->p[j + 1][k], s->read->p[j][k]);
		CANDL_set_si(s->read->p[j][k], 0);
	      }
	  // At the current depth (i), the variable is privatizable:
	  // id  i  j  n  1
	  //  1  1  0  0  0
	  //  0  0  1  0  0
	  CANDL_set_si(s->read->p[j][depth], 1);
	}
      // Same for the 'written' access function.
      for (j = 0; j < s->written->NbRows &&
	     ! CANDL_eq(s->written->p[j][0], scalar); ++j)
	;
      if (j < s->written->NbRows)
	{
	  if (j + 1 < s->written->NbRows &&
	      CANDL_zero_p(s->written->p[j + 1][0]))
	    for (k = 1; k < s->written->NbColumns; ++k)
	      {
		CANDL_assign(s->written->p[j + 1][k], s->written->p[j][k]);
		CANDL_set_si(s->written->p[j][k], 0);
	      }
	  CANDL_set_si(s->written->p[j][depth], 1);
	}
    }
}


/**
 * Perform scalar privatization by array expansion. Check for all
 * scalar variables of the program if it can be privatized. If so,
 * perform array expansion on this scalar variable (update of the
 * array access functions)
 *
 */
void
candl_dependence_perform_scalar_privatization (CandlProgram* p,
					       CandlOptions* options)
{
  CandlStatement** sl = (CandlStatement**)
    malloc (CANDL_ARRAY_BUFF_SIZE * sizeof(CandlStatement*));
  CandlStatement** tsl = (CandlStatement**)
    malloc (CANDL_ARRAY_BUFF_SIZE * sizeof(CandlStatement*));
  Entier* scalars = (Entier*) malloc (CANDL_ARRAY_BUFF_SIZE * sizeof(Entier));
  int i, j, k, l, m;
  int count;
  int max = 0;
  int current;
  int dimadded;
  int firstadded = 0;
  Entier mone;
  CANDL_init(mone);
  CANDL_set_si(mone, -1);

  // Initialize the array of scalar indices.
  for (i = 0; i < CANDL_ARRAY_BUFF_SIZE; ++i)
    {
      CANDL_init(scalars[i]);
      CANDL_set_si(scalars[i], -1);
    }
  // Inspect and collect all scalar indices.
  for (i = 0; i < p->nb_statements; ++i)
    {
      // Collect in read matrix.
      candl_dependence_extract_scalar_variables (p->statement[i]->read,
						 &scalars);
      // Collect in written matrix.
      candl_dependence_extract_scalar_variables (p->statement[i]->written,
						 &scalars);
    }

  // Check for all scalars if it can be privatized.
  for (i = 0; ! CANDL_eq(scalars[i], mone); ++i)
    {
      for (j = 0; j < CANDL_ARRAY_BUFF_SIZE; ++j)
	sl[j] = tsl[j] = NULL;
      count = 0;
      // Get all statements referring to this scalar.
      for (j = 0; j < p->nb_statements; ++j)
	{
	  for (k = 0; k < p->statement[j]->read->NbRows &&
		 ! CANDL_eq(p->statement[j]->read->p[k][0], scalars[i]); ++k)
	    ;
	  if (k != p->statement[j]->read->NbRows)
	    sl[count++] = p->statement[j];
	  else
	    {
	      for (k = 0; k < p->statement[j]->written->NbRows &&
		     ! CANDL_eq(p->statement[j]->written->p[k][0], scalars[i]);
		   ++k)
		;
	      if (k != p->statement[j]->written->NbRows)
		sl[count++] = p->statement[j];
	    }
	  if (count && (count % CANDL_ARRAY_BUFF_SIZE == 0))
	    {
	      sl = (CandlStatement**) realloc
		(sl, (count + CANDL_ARRAY_BUFF_SIZE) * sizeof(CandlStatement*));
	      for (k = 0; k < CANDL_ARRAY_BUFF_SIZE; ++k)
		sl[count + k] = NULL;
	    }
	}
      max = 0;
      firstadded = 0;
      count = 0;
      // Get the maximum depth in the listed statements.
      for (j = 0; sl[j] != NULL; ++j)
	max = max < sl[j]->depth ? sl[j]->depth : max;
      // Iterate on depths, from the inner-most to the outer-most.
      for (j = max; j > 0; --j)
	{
	  dimadded = 0;
	  // Get all statements with depth >= current.
	  for (k = 0; sl[k] != NULL; ++k)
	    if (sl[k]->depth >= j)
	      tsl[count++] = sl[k];
	  // Iterate on each first statement of a loop.
	  for (k = 0; tsl[k] != NULL; )
	    {
	      // Check if the statement is executed at least once.
	      if (! candl_dependence_check_point (tsl[k]->domain, p->context))
		{
		  // No point, iteration domain is empty, skip that
		  // statement.
		  ++k;
		  continue;
		}
	      // Check if the scalar is written.
	      for (l = 0; l < tsl[k]->written->NbRows &&
		     ! CANDL_eq(tsl[k]->written->p[l][0], scalars[i]); ++l)
		;
	      // Check if the scalar is read.
	      for (m = 0; m < tsl[k]->read->NbRows &&
		     ! CANDL_eq(tsl[k]->read->p[m][0], scalars[i]); ++m)
		;
	      // Scalar is written and not read, for it's first use:
	      // it can be privatized.
	      if (l != tsl[k]->written->NbRows && m == tsl[k]->read->NbRows)
		{
		  // Only perform expansion when needed.
		  if (! dimadded++)
		    if (firstadded++)
		    candl_dependence_expand_scalar (sl, scalars[i]);
		  // Privatize.
		  candl_dependence_privatize_scalar
		    (scalars[i], tsl, sl, k, j);
		}
	      // Move to the next non-common loop.
	      current = tsl[k]->index[j - 1];
	      for (; tsl[k] != NULL && tsl[k]->index[j - 1] == current; ++k)
		;
	    }
	}
      /// FIXME: add a check to ensure that all instance of the scalar
      /// was privatized at least once. If not, fallback.
      for (k = 0; sl[k] != NULL; ++k)
	{
	  for (j = 0; j < sl[k]->read->NbRows && 
		 ! CANDL_eq(sl[k]->read->p[j][0], scalars[i]); ++j)
	    ;
/* 	  if (CANDL_zero_p(sl[k]->read->p[j][1])) */
/* 	    { */
/* 	      candl_dependence_privatize_scalar_fallback (sl, scalars[i]); */
/* 	      break; */
/* 	    } */
	}
    }

  // Be clean.
  for (i = 0; ! CANDL_eq(scalars[i], mone); ++i)
    CANDL_clear(scalars[i]);
  free (scalars);
  CANDL_clear(mone);
}


/**
 * candl_dependence function :
 * this function builds the dependence graph of a program (program)
 * according to some user options (options).
 * - 18/09/2003: first version.
 */
CandlDependence * candl_dependence(program, options)
CandlProgram * program ;
CandlOptions * options ;
{ int i, j ;
  CandlDependence * dependence=NULL, * new=NULL, * now ;
  CandlStatement  ** statement ;
  CandlMatrix * context ;

  statement = program->statement ;
  context = program->context ;

  if (options->scalar_privatization)
    candl_dependence_perform_scalar_privatization (program, options);

  // candl_program_print (stdout, program);

  for (i=0; i<program->nb_statements; i++)
  { /* We add self dependence. */
    /* S->S */
    new = candl_dependence_between(statement[i],statement[i],context,options) ;
    candl_dependence_add(&dependence,&now,new) ;

    for (j=i+1; j<program->nb_statements; j++)
    { /* And dependences with other statements. */
      /* S1->S2 */
      new = candl_dependence_between(statement[i],statement[j],context,options);
      candl_dependence_add(&dependence,&now,new) ;

      /* S2->S1 */
      new = candl_dependence_between(statement[j],statement[i],context,options);
      candl_dependence_add(&dependence,&now,new) ;
    }
  }

  return dependence ;
}
