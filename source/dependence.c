
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
# include <candl/candl.h>


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
void candl_dependence_print_structure(FILE* file,
				      candl_dependence_p dependence,
				      int level)
{
  int j, first = 1;

  if (dependence != NULL)
    { /* Go to the right level. */
      for(j=0; j<level; j++)
	fprintf(file, "|\t");
      fprintf(file, "+-- CandlDependence\n");
    }
  else
    { for(j=0; j<level; j++)
	fprintf(file, "|\t");
      fprintf(file, "+-- NULL dependence\n");
    }

  while (dependence != NULL)
    { if (!first)
	{ /* Go to the right level. */
	  for(j=0; j<level; j++)
	    fprintf(file, "|\t");
	  fprintf(file, "|   CandlDependence\n");
	}
      else
	first = 0;

      /* A blank line. */
      for(j=0; j<=level+1; j++)
	fprintf(file, "|\t");
      fprintf(file, "\n");

      /* Go to the right level and print the type. */
      for(j=0; j<=level; j++)
	fprintf(file, "|\t");
      fprintf(file, "Type: ");
      switch (dependence->type)
	{
	case CANDL_UNSET : fprintf(file, "UNSET\n");        break;
	case CANDL_RAW   : fprintf(file, "RAW (flow)\n");   break;
	case CANDL_WAR   : fprintf(file, "WAR (anti)\n");   break;
	case CANDL_WAW   : fprintf(file, "WAW (output)\n"); break;
	case CANDL_RAR   : fprintf(file, "RAR (input)\n");  break;
	default : fprintf(file, "unknown\n"); break;
	}

      /* A blank line. */
      for(j=0; j<=level+1; j++)
	fprintf(file, "|\t");
      fprintf(file, "\n");

      /* Go to the right level and print the depth. */
      for(j=0; j<=level; j++)
	fprintf(file, "|\t");
      fprintf(file, "Depth: %d\n", dependence->depth);

      /* A blank line. */
      for(j=0; j<=level+1; j++)
	fprintf(file, "|\t");
      fprintf(file, "\n");

      /* Print the source statement. */
      candl_statement_print_structure(file, dependence->source, level+1);

      /* Print the target statement. */
      candl_statement_print_structure(file, dependence->target, level+1);

      /* Print the dependence polyhedron. */
      candl_matrix_print_structure(file, dependence->domain, level+1);

      dependence = dependence->next;

      /* Next line. */
      if (dependence != NULL)
	{ for (j=0; j<=level; j++)
	    fprintf(file, "|\t");
	  fprintf(file, "V\n");
	}
    }

  /* The last line. */
  for(j=0; j<=level; j++)
    fprintf(file, "|\t");
  fprintf(file, "\n");
}


/* candl_dependence_print function:
 * This function prints the content of a CandlDependence structure (dependence)
 * into a file (file, possibly stdout).
 */
void candl_dependence_print(FILE * file, candl_dependence_p dependence)
{
  candl_dependence_print_structure(file, dependence, 0);
}


/* candl_dependence_pprint function:
 * This function prints the content of a CandlDependence structure (dependence)
 * into a file (file, possibly stdout) as a Graphviz input file.
 * See http://www.graphviz.org
 * - 08/12/2005: first version.
 */
void candl_dependence_pprint(FILE * file, candl_dependence_p dependence)
{
  int i = 0;

  fprintf(file, "digraph G {\n");

  fprintf(file, "# Data Dependence Graph\n");
  fprintf(file, "# Generated by Candl "CANDL_RELEASE" "CANDL_VERSION" bits\n");
  while (dependence != NULL)
    {
      fprintf(file, "  S%d -> S%d [label=\" ", dependence->source->label,
	      dependence->target->label);
      switch (dependence->type)
	{
	case CANDL_UNSET : fprintf(file, "UNSET"); break;
	case CANDL_RAW   : fprintf(file, "RAW")  ; break;
	case CANDL_WAR   : fprintf(file, "WAR")  ; break;
	case CANDL_WAW   : fprintf(file, "WAW")  ; break;
	case CANDL_RAR   : fprintf(file, "RAR")  ; break;
	default : fprintf(file, "unknown"); break;
	}
      fprintf(file, " depth %d, ref %d->%d \"];\n", dependence->depth,
	      dependence->ref_source,
	      dependence->ref_target);
      dependence = dependence->next;
      i++;
    }

  if (i>4)
    fprintf(file, "# Number of edges = %i\n}\n", i);
  else
    fprintf(file, "}\n");
}


/* candl_dependence_view function:
 * This function uses dot (see http://www.graphviz.org) and gv (see
 * http://wwwthep.physik.uni-mainz.de/~plass/gv) tools to display the
 * dependence graph.
 * - 20/03/2006: first version.
 */
void candl_dependence_view(candl_dependence_p dependence)
{
  FILE * temp_output;

  temp_output = fopen(CANDL_TEMP_OUTPUT, "w");
  candl_dependence_pprint(temp_output, dependence);
  fclose(temp_output);
  system("(dot -Tps "CANDL_TEMP_OUTPUT" | gv - &) && rm -f "CANDL_TEMP_OUTPUT);
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/* candl_dependence_free function:
 * This function frees the allocated memory for a CandlDependence structure.
 * - 18/09/2003: first version.
 */
void candl_dependence_free(candl_dependence_p dependence)
{
  candl_dependence_p next;

  while (dependence != NULL)
    {
      next = dependence->next;
      candl_matrix_free(dependence->domain);
      free(dependence);
      dependence = next;
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
candl_dependence_p candl_dependence_malloc()
{
  candl_dependence_p dependence;

  /* Memory allocation for the CandlDependence structure. */
  dependence = (candl_dependence_p) malloc(sizeof(CandlDependence));
  if (dependence == NULL)
    {
      fprintf(stderr,  "[Candl]ERROR: memory overflow.\n");
      exit(1);
    }

  /* We set the various fields with default values. */
  dependence->source     = NULL;
  dependence->target     = NULL;
  dependence->depth      = CANDL_UNSET;
  dependence->type       = CANDL_UNSET;
  dependence->ref_source = CANDL_UNSET;
  dependence->ref_target = CANDL_UNSET;
  dependence->domain     = NULL;
  dependence->next       = NULL;

  dependence->solved = 0;
  dependence->id = 0;
  dependence->tag = NULL;

  return dependence;
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
void candl_dependence_add(candl_dependence_p* start,
			  candl_dependence_p* now,
			  candl_dependence_p dependence)
{
  if (dependence != NULL)
    {
      if (*start == NULL)
	{
	  *start = dependence;
	  *now = *start;
	}
      else
	{
	  (*now)->next = dependence;
	  *now = (*now)->next;
	}

      while ((*now)->next != NULL)
	*now = (*now)->next;
    }
}


/**
 * GCD computation.
 */
static
int
candl_dependence_gcd(int a, int b)
{
  int z = 1;

  if (a < 0)
    a *= -1;
  if (b < 0)
    b *= -1;
  if (a == 0)
    return b;
  if (b == 0)
    return a;
  if (b > a)
    {
      int temp = a;
      a = b;
      b = temp;
    }

  while (z != 0)
    {
      z = a % b;
      a = b;
      b = z;
    }

  return a;
}

/**
 *
 *
 */
static
int candl_dependence_gcd_test_context (CandlMatrix* system, int id)
{
  /* FIXME: implement me! */

  return 1;
}


/**
 * candl_dependence_gcd_test function:
 * This functions performs a GCD test on a dependence polyhedra
 * represented exactly by a set of constraints 'system' organized in
 * such a way:
 * - first lines: iteration domain of 'source'
 * - then: iteration domain of 'target'
 * - then: array access equality(ies)
 * - then (optional): precedence constraint inequality.
 *
 * The function returns false if the dependence is impossible, true
 * otherwise. A series of simple checks (SIV/ZIV/MIV/bounds checking)
 * are also performed before the actual GCD test.
 *
 */
int candl_dependence_gcd_test(CandlStatement* source,
			      CandlStatement* target,
			      CandlMatrix* system,
			      int level)
{
  int i;
  int gcd;
  int id;
  int value;
  int null_iter, null_param, null_cst, pos_iter, neg_iter,
    strict_pred, null_level = 1;

  /* Check that the precedence constraint, if any, is not strict in a
     self-dependence. */
  if (source == target &&
      CANDL_get_si(system->p[system->NbRows - 1][0]) == 1 &&
      CANDL_get_si(system->p[system->NbRows - 1][system->NbColumns - 1]) == -1)
    strict_pred = 1;
  else
    strict_pred = 0;

  /* Inspect the array access function equalities. */
  for (id = source->domain->NbRows + target->domain->NbRows;
       id < system->NbRows && CANDL_get_si(system->p[id][0]) == 0; ++id)
    {
      /* Inspect which parts of the access function equality are null,
	 positive or negative. */
      null_iter = null_param = null_cst = pos_iter = neg_iter = 0;
      for (i = 1; i < source->depth + target->depth + 1 &&
	     CANDL_get_si(system->p[id][i]) == 0; ++i)
	;
      if (i == source->depth + target->depth + 1)
	null_iter = 1;
      else
	for (pos_iter = 1, neg_iter = 1;
	     i < source->depth + target->depth + 1; ++i)
	  {
	    if (CANDL_get_si(system->p[id][i]) < 0)
	      pos_iter = 0;
	    else if (CANDL_get_si(system->p[id][i]) > 0)
	      neg_iter = 0;
	    if (i == level || i == level + source->depth)
	      null_level &= CANDL_get_si(system->p[id][i]) != 0 ? 0 : 1;
	  }
      for (; i < system->NbColumns - 1 && CANDL_get_si(system->p[id][i]) == 0;
	   ++i)
	;
      if (i == system->NbColumns - 1)
	null_param = 1;
      null_cst = ! CANDL_get_si(system->p[id][system->NbColumns - 1]);

      /* Some useful ZIV/SIV/MIV tests. */
      if (null_iter && null_param && !null_cst)
	return 0;
      if (null_iter)
	if (! candl_dependence_gcd_test_context (system, id))
	  return 0;
      if (null_cst || !null_param)
	continue;
/* FIXME: implement the loop bound check. */
/*       /\* A clever test on access bounds. *\/ */
/*       if (null_param && pos_iter &&  */
/* 	  CANDL_get_si(system->p[id][system->NbColumns - 1]) > 0) */
/* 	return 0; */
/*       if (null_param && neg_iter &&  */
/* 	  CANDL_get_si(system->p[id][system->NbColumns - 1]) < 0) */
/* 	return 0; */

      /* Compute GCD test for the array access equality. */
      for (i = 1, gcd = CANDL_get_si(system->p[id][i]);
	   i < source->depth + target->depth; ++i)
	gcd = candl_dependence_gcd(gcd, CANDL_get_si(system->p[id][i + 1]));
      value = system->p[id][system->NbColumns - 1];
      value = value < 0 ? -value : value;
      if ((gcd == 0 && value != 0) || value % gcd)
	return 0;
    }
  /* Precedence constraint incompatible with the access function. */
  if (strict_pred && !null_level)
    return 0;

  return 1;
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
candl_dependence_p candl_dependence_system(CandlStatement* source,
					   CandlStatement* target,
					   CandlMatrix* context,
					   CandlMatrix* array_s,
					   CandlMatrix* array_t,
					   int ref_s, int ref_t,
					   int type, int depth)
{
  candl_dependence_p dependence = NULL;
  CandlMatrix * system;
  PipOptions * options;
  PipQuast * solution;

  /* First, a trivial case: for two different statements at depth 0, there is
   * a dependence only if the source is textually before the target.
   */
  if ((source != target) && (depth == 0) && (source->label > target->label))
    return NULL;

  /* We build the system of constraints. */
  system = candl_matrix_dependence(source->domain,  target->domain,
                                   array_s, array_t, ref_s, ref_t,
				   depth, (source->label >= target->label),
				   context->NbColumns-2);
  /* We start by simple SIV/ZIV/GCD tests. */
  if (!candl_dependence_gcd_test(source, target, system, depth))
    return NULL;

  options = pip_options_init();
  options->Simplify = 1;
  options->Urs_parms = -1;
  options->Urs_unknowns = -1;
  solution = pip_solve(system,context, -1, options);

  if ((solution != NULL) &&
      ((solution->list != NULL) || (solution->condition != NULL)))
    {
      dependence = candl_dependence_malloc();

      /* We set the various fields with corresponding values. */
      dependence->type       = type;
      dependence->depth      = depth;
      dependence->source     = source;
      dependence->target     = target;
      dependence->ref_source = ref_s;
      dependence->ref_target = ref_t;
      dependence->domain     = system;

      dependence->solved = 0;
      dependence->id = 0;
      dependence->tag = NULL;
    }
  else
    candl_matrix_free(system);

  pip_options_free(options);
  pip_quast_free(solution);

  return dependence;
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
candl_dependence_p candl_dependence_between(CandlStatement* source,
					    CandlStatement* target,
					    CandlMatrix* context,
					    CandlOptions* options)
{
  int i, j, k, min_depth, max_depth;
  candl_dependence_p new;
  candl_dependence_p dependence = NULL;
  candl_dependence_p now;

  /* If the statements commute and the user asks to use this information to
   * simplify the dependence graph, we return no dependences.
   */
  if (options->commute && candl_statement_commute(source, target))
    return NULL;

  /* In the case of a self-dependence, the dependence depth can be as low as 1
   * (not 0 because at depth 0 there is no loop, thus there is only one
   * instance of the statement !) and as high as the statement depth.
   * In the case of different statements, the dependence depth can be as low
   * as 0 and as high as the number of shared loops.
   */
  if (source == target)
    {
      min_depth = 1;
      max_depth = source->depth;
    }
  else
    {
      /* Depth 0 is for statements that don't share any loop. */
      min_depth = (source->index[0] == target->index[0]) ? 1 : 0;

      max_depth = 0;
      while ((max_depth < source->depth) &&
	     (max_depth < target->depth) &&
	     (source->index[max_depth] == target->index[max_depth]))
	max_depth++;
    }

  /* Flow and output-dependences analysis. */
  for (j = 0; j < source->written->NbRows; j++)
    if (CANDL_notzero_p(source->written->p[j][0]))
      {
	/* Flow-dependences analysis. */
	if (options->raw)
	  for (k = 0; k < target->read->NbRows; k++)
	    if (CANDL_eq(target->read->p[k][0], source->written->p[j][0]))
	      for (i = min_depth; i <= max_depth; i++)
		{
		  new = candl_dependence_system(source, target, context,
						source->written, target->read,
						j, k, CANDL_RAW, i);
		  candl_dependence_add(&dependence, &now, new);
		}
	/* Output-dependences analysis. */
	if (options->waw)
	  for (k = 0; k < target->written->NbRows; k++)
	    if (CANDL_eq(target->written->p[k][0], source->written->p[j][0]))
	      for (i = min_depth; i <= max_depth; i++)
		{
		  new = candl_dependence_system(source, target, context,
						source->written,
						target->written, j, k,
						CANDL_WAW, i);
		    candl_dependence_add(&dependence, &now, new);
		  }
      }

  /* Anti and input-dependences analysis. */
  for (j = 0; j < source->read->NbRows; j++)
    if (source->read->p[j][0] != 0)
      {
	/* Anti-dependences analysis. */
	if (options->war)
	  for (k = 0; k < target->written->NbRows; k++)
	    if (CANDL_eq(target->written->p[k][0], source->read->p[j][0]))
	      for (i = min_depth; i <= max_depth; i++)
		{
		  new = candl_dependence_system(source, target, context,
						source->read, target->written,
						j, k, CANDL_WAR, i);
		  candl_dependence_add(&dependence, &now, new);
		}
	/* Input-dependences analysis. */
	if (options->rar)
	  for (k = 0; k < target->read->NbRows; k++)
	    if (CANDL_eq(target->read->p[k][0], source->read->p[j][0]))
	      for (i = min_depth; i <= max_depth; i++)
		{
		  new = candl_dependence_system(source, target, context,
						source->read, target->read,
						j, k, CANDL_RAR, i);
		  candl_dependence_add(&dependence, &now, new);
		}
      }

  return dependence;
}





/**
 * candl_dependence function:
 * this function builds the dependence graph of a program (program)
 * according to some user options (options).
 * - 18/09/2003: first version.
 */
candl_dependence_p candl_dependence(candl_program_p program,
				    CandlOptions* options)
{
  int i, j;
  candl_dependence_p dependence = NULL;
  candl_dependence_p new = NULL;
  candl_dependence_p now;
  CandlStatement  ** statement;
  CandlMatrix * context;

  statement = program->statement;
  context = program->context;

  if (options->scalar_privatization)
    candl_dependence_privatize_scalars (program);

  for (i = 0; i < program->nb_statements; i++)
    { /* We add self dependence. */
      /* S->S */
      new = candl_dependence_between(statement[i], statement[i],
				     context, options);
      candl_dependence_add(&dependence, &now, new);

      for (j = i + 1; j < program->nb_statements; j++)
	{ /* And dependences with other statements. */
	  /* S1->S2 */
	  new = candl_dependence_between(statement[i], statement[j],
					 context, options);
	  candl_dependence_add(&dependence, &now, new);

	  /* S2->S1 */
	  new = candl_dependence_between(statement[j], statement[i],
					 context, options);
	  candl_dependence_add(&dependence, &now, new);
	}
    }

  return dependence;
}


/******************************************************************************
 *                          Scalar analysis functions                         *
 ******************************************************************************/

/**
 * candl_dependence_var_is_scalar function:
 * This function returns true if the variable indexed by 'var_index'
 * is a scalar in the whole program.
 * This function holds only if scalar privatization has not been yet
 * performed.
 */
static
int
candl_dependence_var_is_scalar (candl_program_p program, int var_index)
{
  CandlMatrix* m;
  int i, j, k, cpt;

  for (i = 0; i < program->nb_statements; ++i)
    for (m = program->statement[i]->read, cpt = 0; cpt < 2; ++cpt,
	   m = program->statement[i]->written)
      for (j = 0; j < m->NbRows; ++j)
	if (CANDL_get_si(m->p[j][0]) == var_index)
	  for (k = 1; k < m->NbColumns; ++k)
	    if (CANDL_get_si(m->p[j][k]) != 0)
	      return 0;
  return 1;
}


/**
 *
 *
 */
static
void
candl_dependence_rename_variable(CandlStatement** stmts, int var_index,
				 int new_var_index)
{
  int i, j, cpt;
  CandlMatrix* m;

  for (i = 0; stmts && stmts[i]; ++i)
    for (m = stmts[i]->read, cpt = 0; cpt < 2; ++cpt, m = stmts[i]->written)
      for (j = 0; j < m->NbRows; ++j)
	if (CANDL_get_si(m->p[j][0]) == var_index)
	  CANDL_set_si(m->p[j][0], new_var_index);
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
				int scalar_idx)
{
  CandlMatrix* m;
  int i, l, n, j;

  // Iterate on all statements of the list.
  for (i = 0; sl[i] != NULL; ++i)
    {
      // Check if the scalar is referenced in the 'read' access function.
      for (j = 0; j < sl[i]->read->NbRows &&
	     CANDL_get_si(sl[i]->read->p[j][0]) != scalar_idx; ++j)
	;
      // It is.
      if (j < sl[i]->read->NbRows)
	{
	  // Add a row to the 'read' matrix, just after the reference
	  // to 'scalar_idx'.
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
	     CANDL_get_si(sl[i]->written->p[j][0]) != scalar_idx;++j)
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
 * candl_dependence_use_def_graph function:
 * This function returns a def-use chain as a feshly allocated array
 * of pointer on statements, of all statements reading or writing the
 * variable 'var_index', dominated by the 'level' common loops of 'dom'.
 * Output is a NULL-terminated array.
 */
CandlStatement**
candl_dependence_refvar_chain (candl_program_p program, CandlStatement* dom,
				int var_index, int level)
{
  /* No or empty program -> no chain! */
  if (program == NULL || program->nb_statements == 0)
    return NULL;

  int buffer_size = 64;
  CandlStatement** res =
    (CandlStatement**) malloc(buffer_size * sizeof(CandlStatement*));
  int i, j, count = 0;
  CandlStatement* s;

  /* If no dominator is provided, assume we start with the first statement. */
  if (dom == NULL)
    dom = program->statement[0];
  for (i = 0; i < program->nb_statements && program->statement[i] != dom; ++i)
    ;
  /* The dominator is not in the list of statements. */
  if (i == program->nb_statements)
    return NULL;
  for (; i < program->nb_statements; ++i)
    {
      s = program->statement[i];
      /* Ensure it has 'level' common loop(s) with the dominator. */
      for (j = 0; j < level && j < s->depth && s->index[j] == dom->index[j];
	   ++j)
	;
      if (j < level && j < s->depth)
	continue;
      /* Ensure the variable is referenced. */
      if (candl_dependence_var_is_ref (s, var_index)
	  != CANDL_VAR_UNDEF)
	{
	  res[count++] = s;
	  if (count == buffer_size)
	    res = realloc(res, (buffer_size*=2) * sizeof(CandlStatement*));
	}
    }

  res = realloc(res, (count + 1) * sizeof(CandlStatement*));
  res[count] = NULL;

  return res;
}


/**
 * candl_dependence_var_is_ref function:
 * This function checks if a var 'var_index' is referenced (DEF or
 * USE) by the statement.
 */
int
candl_dependence_var_is_ref (CandlStatement* s, int var_index)
{
  int j;

  if (s)
    {
      for (j = 0; s->read && j < s->read->NbRows; ++j)
	if (CANDL_get_si(s->read->p[j][0]) == var_index)
	  return CANDL_VAR_IS_USED;
      for (j = 0; s->written && j < s->written->NbRows; ++j)
	if (CANDL_get_si(s->written->p[j][0]) == var_index)
	  return CANDL_VAR_IS_DEF;
    }
  return CANDL_VAR_UNDEF;
}


/**
 * candl_dependence_check_domain_is_included function:
 * This function checks if the 'level'-first iterators of 2 domains
 * are in such a way that s1 is larger or equal to s2, for the
 * considered iterator dimensions.
 *
 */
int
candl_dependence_check_domain_is_included (CandlStatement* s1,
					  CandlStatement* s2,
					  int level)
{
  int max = level;
  max = s1->depth < max ? s1->depth : max;
  max = s2->depth < max ? s2->depth : max;

  CandlMatrix* m = candl_matrix_malloc(s2->domain->NbRows + s2->depth - max +1,
				       s2->domain->NbColumns);
  int i, j;
  /* Duplicate s2 to the dest matrix. */
  for (i = 0; i < s2->domain->NbRows; ++i)
    for (j = 0; j < s2->domain->NbColumns; ++j)
      CANDL_assign(m->p[i][j], s2->domain->p[i][j]);
  /* Make useless dimensions equal to 0. */
  for (j = 0; j < s2->depth - max; ++j)
    CANDL_set_si(m->p[i++][j + 1 + max], 0);
  /* Iterate on all constraints of s1, and check them. */
  for (i = 0; i < s1->domain->NbRows; ++i)
    {
      /* Invert the constraint, and add it to m. */
      for (j = 0; j <= max; ++j)
	{
	  CANDL_assign(m->p[m->NbRows - 1][j], s1->domain->p[i][j]);
	  CANDL_oppose(m->p[m->NbRows - 1][j], m->p[m->NbRows - 1][j]);
	}
      for (j = s1->depth + 1; j < s1->domain->NbColumns; ++j)
	{
	  CANDL_assign(m->p[m->NbRows - 1][j - s1->depth + s2->depth],
		       s1->domain->p[i][j]);
	  CANDL_oppose(m->p[m->NbRows - 1][j - s1->depth + s2->depth],
		       m->p[m->NbRows - 1][j - s1->depth + s2->depth]);
	}
      /* Make the inequality strict. */
      CANDL_decrement(m->p[m->NbRows - 1][m->NbColumns - 1],
		      m->p[m->NbRows - 1][m->NbColumns - 1]);
      if (candl_matrix_check_point (NULL, m))
	{
	  /* There is a point. dom(s1) - dom(s2) > 0. */
	  candl_matrix_free(m);
	  return 0;
	}
    }

  candl_matrix_free(m);

  return 1;
}


/**
 * candl_dependence_privatize_scalars function:
 * This function checks, for all scalar variables of the program, and
 * all loop levels, if the scalar can be privatized at that level. If
 * so, the scalar is expanded in the array access functions to remove
 * false dependencies. When possible, the scalar is renamed to a
 * unique variable id.
 */
int
candl_dependence_privatize_scalars (candl_program_p program)
{
  int scalars[1024];
  int checked[1024];
  int commit_names[1024];
  CandlStatement** commit_chains[1024];
  int count_cn = 0;
  int count_cc = 0;
  int count_s = 0;
  int count_c = 0;
  CandlStatement** stmts;
  CandlStatement** fullchain = NULL;
  int i, j, k, l, n;
  CandlMatrix* m;
  int idx, max, is_priv, cpt;
  CandlStatement* curlast;
  CandlStatement* last;

  fprintf (stderr, "[Candl] Info: Perform scalar privatization...\n");
  /* Detect all scalar variables. */
  for (i = 0; i < program->nb_statements; ++i)
    for (m = program->statement[i]->read, cpt = 0; cpt < 2; ++cpt,
	   m = program->statement[i]->written)
      for (j = 0; j < m->NbRows; ++j)
	{
	  idx = CANDL_get_si(m->p[j][0]);
	  if (idx != 0)
	    {
	      for (k = 0; k < count_s && scalars[k] != idx; ++k)
		;
	      if (k == count_s)
		{
		  for (k = 0; k < count_c && checked[k] != idx; ++k)
		    ;
		  if (k == count_c)
		    if (candl_dependence_var_is_scalar(program, idx))
		      scalars[count_s++] = idx;
		}
	    }
	}
  /* For each of those, detect (at any level) if it is privatizable. */
  for (i = 0; i < count_s; ++i)
    {
      idx = scalars[i];
      /* Go to the first statement referencing the scalar. */
      for (j = 0; j < program->nb_statements; ++j)
	if (candl_dependence_var_is_ref (program->statement[j], scalars[i])
	    != CANDL_VAR_UNDEF)
	  break;
      if (j < program->nb_statements)
	{
	  /* Take all statements referencing the scalar. */
	  fullchain = candl_dependence_refvar_chain (program,
						program->statement[j],
						scalars[i], 0);
	  /* Compute the maximum loop depth of the chain. */
	  for (k = 0, max = 0; fullchain[k]; ++k)
	    max = max < fullchain[k]->depth ? fullchain[k]->depth : max;
	  last = fullchain[k - 1];
	  /* Initialize the offset. */
	  int offset = 0;
	  int was_priv = 0;
	  /* Iterate on all possible depth for privatization. */
	  for (k = 1; k < max; ++k)
	    {
	      CandlStatement* s = fullchain[0];
	      if (was_priv)
		{
		  ++offset;
		  was_priv = 0;
		}
	      do
		{
		  /* Take all statements dominated by s referencing
		     the current scalar variable. */
		  stmts = candl_dependence_refvar_chain (program, s,
							 scalars[i], k);
		  int c = 0;
		  is_priv = 1;
		  /* Check for privatization, while the entry of the
		     chain is a DEF. */
		  while (stmts[c] &&
			 candl_dependence_var_is_ref (stmts[c], scalars[i])
		      == CANDL_VAR_IS_DEF)
		    {
		      /* From the current DEF node, ensure the rest of
			 the chain covers not more than the
			 iteration domain of the DEF. */
		      for (l = c + 1; stmts[l - 1] && stmts[l]; ++l)
			/* FIXME: we should deal with
			   def_1->use->def_2->use chains where
			   dom(def_2) > dom(def_1). */
			if (! candl_dependence_check_domain_is_included
			    (stmts[c], stmts[l], k))
			    {
			      /* dom(use) - dom(def) > 0. Check if there
				 is another DEF to test at the entry
				 of the block. */
			      if (stmts[c + 1] && candl_dependence_var_is_ref
				  (stmts[c + 1], scalars[i])
				  != CANDL_VAR_IS_DEF)
				/* No. The variable is not privatizable. */
				is_priv = 0;
			      break;
			    }
		      if (! is_priv || ! stmts[l])
			break;
		      /* The chain dominated by stmts[c] is not
			 privatizable. Go for the next DEF at the
			 beginning of the block, if any. */
		      ++c;
		    }
		  if (is_priv)
		    {
		      /* Perform the privatization. */
		      fprintf
			(stderr,
			 "[Candl] Info: The variable %d can be privatized\n",
			 scalars[i]);
		      /* Traverse all statements in the chain. */
		      for (l = c; stmts[l]; ++l)
			{
			  /* It's not the first expansion of the
			     scalar, we need to increate its dimension
			     all along the program. */
			  if (offset && !was_priv)
			    candl_dependence_expand_scalar (fullchain,
							    scalars[i]);
			  /* Perform scalar expansion in the array
			     access functions. */
			  for (cpt = 0, m = stmts[l]->read; cpt < 2;
			       ++cpt, m = stmts[l]->written)
			    for (n = 0; n < m->NbRows; ++n)
			      if (CANDL_get_si(m->p[n][0]) == scalars[i])
				CANDL_set_si(m->p[n + offset][k], 1);
			  was_priv = 1;
			}
		      /* Memorize the list of statements for variable
			 renaming. */
		      commit_chains[count_cc++] = stmts;
		      commit_names[count_cn++] = scalars[i];
		    }
		  /* Go to the next block, if any. */
		  for (l = 0; stmts[l]; ++l)
		    ;
		  curlast = stmts[l - 1];
		  if (curlast != last)
		    for (l = 0; fullchain[l]; ++l)
		      if (fullchain[l] == curlast)
			s = fullchain[l + 1];
		  /* Variable was not privatized, free the current chain. */
		  if (! is_priv)
		    free (stmts);
		}
	      while (curlast != last);
	    }
	}
      free (fullchain);
    }

  /* Commit name change in the program. */
  int next_var_name = 0;
  if (count_cc)
    /* Compute the next free array index. */
    for (i = 0; i < program->nb_statements; ++i)
      for (m = program->statement[i]->read, cpt = 0; cpt < 2; ++cpt,
	     m = program->statement[i]->written)
	for (j = 0; j < m->NbRows; ++j)
	  if (CANDL_get_si(m->p[j][0]) >= next_var_name)
	    next_var_name = CANDL_get_si(m->p[j][0]) + 1;

  /* Free the memorized chains. */
  for (i = 0; i < count_cc; ++i)
    free(commit_chains[i]);

  fprintf (stderr, "[Candl] Info: Scalar privatization finished\n");

  return 0;
}
