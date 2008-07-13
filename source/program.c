
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                   program.c                             **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: september 9th 2003               **
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


#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <candl/candl.h>


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/

/**
 * candl_program_print_structure function:
 * Displays a candl_program_t structure (program) into a file (file,
 * possibly stdout) in a way that trends to be understandable without falling
 * in a deep depression or, for the lucky ones, getting a headache... It
 * includes an indentation level (level) in order to work with others
 * print_structure functions.
 * - 09/09/2003: first version.
 */
void candl_program_print_structure(FILE* file, candl_program_p program,
				   int level)
{
  int i, j;

  if (program != NULL)
  {
    /* Go to the right level. */
    for (j = 0; j < level; j++)
      fprintf(file,"|\t");
    fprintf(file,"+-- candl_program_t\n");

    /* A blank line. */
    for (j = 0; j <= level + 1; j++)
      fprintf(file,"|\t");
    fprintf(file,"\n");

    /* Print the context. */
    candl_matrix_print_structure(file, program->context, level+1);

    /* A blank line. */
    for (j = 0; j <= level+1; j++)
      fprintf(file, "|\t");
    fprintf(file, "\n");

    /* Go to the right level and print the statement number. */
    for (j = 0; j <= level; j++)
      fprintf(file, "|\t");
    fprintf(file, "Statement number: %d\n", program->nb_statements);

    /* A blank line. */
    for (j = 0; j <= level+1; j++)
      fprintf(file, "|\t");
    fprintf(file, "\n");

    /* Print the statements. */
    for (i = 0; i < program->nb_statements; ++i)
      candl_statement_print_structure(file, program->statement[i], level+1);

    /* Print the transformation candidate. */
    if (program->transformation != NULL)
      for (i = 0; i < program->nb_statements; i++)
	candl_matrix_print_structure(file, program->transformation[i], level+1);
    else
      {
	/* Go to the right level. */
	for (j = 0; j <= level; j++)
	  fprintf(file, "|\t");
	fprintf(file, "+-- No transformation candidate\n");

	/* A blank line. */
	for (j = 0; j <= level+1; j++)
	  fprintf(file, "|\t");
	fprintf(file, "\n");
      }
  }
  else
    {
      /* Go to the right level. */
      for (j = 0; j < level; j++)
	fprintf(file, "|\t");
      fprintf(file, "+-- NULL candl_program_t\n");
    }

  /* The last line. */
  for (j = 0; j <= level; j++)
    fprintf(file, "|\t");
  fprintf(file, "\n");
}


/**
 * candl_program_print function:
 * This function prints the content of a candl_program_t structure
 * (program) into a file (file, possibly stdout).
 */
void candl_program_print(FILE * file,  candl_program_p program)
{
  candl_program_print_structure(file, program,0);
}


/******************************************************************************
 *                         Memory alloc/dealloc function                      *
 ******************************************************************************/


/**
 * candl_program_malloc function:
 * This function allocates the memory space for a candl_program_t structure and
 * sets its fields with default values. Then it returns a pointer to the
 * allocated space.
 * - 09/12/2005: first version.
 */
candl_program_p candl_program_malloc()
{
  candl_program_p program;

  /* Memory allocation for the candl_program_t structure. */
  program = (candl_program_p)malloc(sizeof(candl_program_t));
  if (program == NULL)
    {
      fprintf(stderr, "[Candl]ERROR: memory overflow.\n");
      exit(1);
    }

  /* We set the various fields with default values. */
  program->context        = NULL;
  program->nb_statements  = 0;
  program->statement      = NULL;
  program->transformation = NULL;

  return program;
}


/**
 * candl_program_free function:
 * This function frees the allocated memory for a candl_program_t structure, it
 * recursively frees everything inside.
 */
void candl_program_free(candl_program_p program)
{
  int i;

  candl_matrix_free(program->context);

  if (program->statement != NULL)
    {
      for (i = 0; i < program->nb_statements; i++)
	candl_statement_free(program->statement[i]);
      free(program->statement);
    }

  if (program->transformation != NULL)
    {
      for (i = 0; i < program->nb_statements; i++)
	candl_matrix_free(program->transformation[i]);
      free(program->transformation);
    }

  free(program);
}


/******************************************************************************
 *                               Reading function                             *
 ******************************************************************************/


/**
 * candl_program_read function:
 * This function reads the informations to put in a candl_program_t
 * structure from a file (file, possibly stdin). It returns a pointer
 * to a candl_program_t structure containing the read informations.
 * September 10th 2003: first version.
 */
candl_program_p candl_program_read(FILE * file)
{
  int i,  nb_statements,  nb_parameters,  nb_functions;
  char s[CANDL_MAX_STRING];
  CandlStatement ** statement;
  CandlMatrix    ** transformation;
  candl_program_p program;

  /* Memory allocation for the candl_program_t structure. */
  program = candl_program_malloc();

  /* First of all,  we read the context data. */
  program->context  = candl_matrix_read(file);
  nb_parameters = program->context->NbColumns - 2;

  /* We read the number of statements. */
  while (fgets(s, CANDL_MAX_STRING, file) == 0)
    ;
  while ((*s=='#'||*s=='\n') || (sscanf(s, " %d", &nb_statements) < 1))
    fgets(s, CANDL_MAX_STRING, file);

  program->nb_statements = nb_statements;

  /* Reading of each statement. */
  if (nb_statements > 0)
    {
      /* Memory allocation for the array of pointers to the statements. */
      statement = (CandlStatement**) malloc(nb_statements *
					    sizeof(CandlStatement*));
      if (statement == NULL)
	{
	  fprintf(stderr,  "[Candl]ERROR: memory overflow.\n");
	  exit(1);
	}

      for (i = 0; i < nb_statements; i++)
	statement[i] = candl_statement_read(file, i, nb_parameters);

      program->statement = statement;
    }

  /* We read the number of transformation functions. */
  while (fgets(s, CANDL_MAX_STRING, file) == 0)
    ;
  while ((*s=='#' || *s=='\n') || (sscanf(s, " %d", &nb_functions) < 1))
    fgets(s, CANDL_MAX_STRING, file);

  /* Reading of each transformation function. */
  if (nb_functions > 0)
    {
      /* The function number must be the same as statement number. */
      if (nb_functions != nb_statements)
	{
	  fprintf(stderr,
		  "[Candl]ERROR: the numbers of transformations (%d) and "
		  "statements (%d) differ.\n", nb_functions, nb_statements);
	  exit(1);
	}

      /* Memory allocation for the array of pointers to the functions. */
      transformation = (CandlMatrix **)malloc(nb_functions *
					      sizeof(CandlMatrix *));
      if (transformation == NULL)
	{
	  fprintf(stderr,  "[Candl]ERROR: memory overflow.\n");
	  exit(1);
	}

      for (i = 0; i < nb_functions; i++)
	transformation[i] = candl_matrix_read(file);

      program->transformation = transformation;
    }

  return(program);
}


/**
 * candl_program_read_scop function:
 * This function reads the informations to put in a candl_program_t
 * structure from a file (file, possibly stdin) following the .scop
 * format.  It returns a pointer to a candl_program_t structure
 * containing the read informations.
 * This function is built only if candl was configured with clan support.
 *
 */
#ifdef CANDL_SUPPORTS_CLAN
candl_program_p candl_program_read_scop(FILE * file)
{
  /* Read the scop. */
  clan_scop_p scop = clan_scop_read(file, NULL);
  /* Check for the <Candl> tag in the options of the .scop file. */
  int** indices = NULL;
  /* Convert the scop. */
  candl_program_p res = candl_program_convert_scop(scop, indices);
  clan_scop_free(scop);

  return res;
}
#endif

/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/


/**
 * candl_program_convert_scop function:
 * This function extracts the useful information of a clan_scop_t
 * structure to a fresh, independent candl_program_t structure.
 * This function is built only if candl was configured with clan support.
 *
 */
#ifdef CANDL_SUPPORTS_CLAN
candl_program_p candl_program_convert_scop(clan_scop_p scop, int** indices)
{
  int i, j, k;
  candl_program_p res = candl_program_malloc();
  clan_statement_p s = scop->statement;

  /* Duplicate the context. */
  res->context = (CandlMatrix*) clan_matrix_copy(scop->context);

  /* Count the number of statements. */
  for (res->nb_statements = 0; s; s = s->next, res->nb_statements++)
    ;

  /* Allocate the statements array. */
  res->statement = (CandlStatement**) malloc(res->nb_statements *
					     sizeof(CandlStatement*));

  /* Initialize structures used in iterator indices computation. */
  int max = 0;
  int max_loop_depth = 128;
  int cur_index[max_loop_depth];
  int last[max_loop_depth];
  for (i = 0; i < max_loop_depth; ++i)
    {
      cur_index[i] = i;
      last[i] = 0;
    }
  /* Create the statements. */
  for (i = 0, s = scop->statement; s; s = s->next, ++i)
    {
      CandlStatement* statement = candl_statement_malloc();
      statement->label = i;
      statement->ref = s;
      if (s->domain->next != NULL)
	{
	  fprintf(stderr, "[Candl]Error: union of domains not supported.\n");
	  exit(1);
	}
      statement->domain = (CandlMatrix*) clan_matrix_copy(s->domain->elt);
      /* For the moment, we do not parse the statement to extract its type. */
      statement->type = CANDL_AFFECTATION;
      statement->depth = statement->domain->NbColumns - 2 - scop->nb_parameters;
      statement->written = (CandlMatrix*) clan_matrix_copy(s->write);
      statement->read = (CandlMatrix*) clan_matrix_copy(s->read);
      statement->index = (int*) malloc(statement->depth * sizeof(int));
      if (indices != NULL)
	/* Iterator indices are provided. */
	for (j = 0; j < statement->depth; ++j)
	  statement->index[j] = indices[i][j];
      else
	{
	  /* Iterator indices must be computed from the scattering matrix. */
	  /* It must be a 2d+1 identity scheduling matrix, and
	     statement must be sorted in their execution order. */
	  /* FIXME: Sort the statements in their execution order. */
	  clan_matrix_p m = s->schedule;
	  if (m->NbRows != 2 * statement->depth + 1)
	    {
	      fprintf(stderr, "[Candl]Error: schedule is not 2d+1 shaped.\n"
		      "Consider using the <candl> option tag to declare iterator indices\n");
	      exit(1);
	    }
	  /* Compute the value of the iterator indices. */
	  for (j = 0; j < statement->depth; ++j)
	    {
	      int val = CANDL_get_si(m->p[2 * j][m->NbColumns - 1]);
	      if (last[j] < val)
		{
		  last[j] = val;
		  for (k = j; k < max_loop_depth; ++k)
		    cur_index[k] = max + (k - j) + 1;
		  break;
		}
	    }
	  for (j = 0; j < statement->depth; ++j)
	    statement->index[j] = cur_index[j];
	  max = max < cur_index[j - 1] ? cur_index[j - 1] : max;
	}
      /* Store the statement. */
      res->statement[i] = statement;
    }

  return res;
}
#endif
