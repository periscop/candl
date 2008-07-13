
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


# include <sys/types.h>
# include <sys/time.h>
# include <sys/resource.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include "../include/candl/candl.h"


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/

/**
 * candl_program_print_structure function:
 * Displays a CandlProgram structure (program) into a file (file,
 * possibly stdout) in a way that trends to be understandable without falling
 * in a deep depression or, for the lucky ones, getting a headache... It
 * includes an indentation level (level) in order to work with others
 * print_structure functions.
 * - 09/09/2003: first version.
 */
void candl_program_print_structure(file, program, level)
FILE * file ;
CandlProgram * program ;
int level ;
{ int j, i ;

  if (program != NULL)
  { /* Go to the right level. */
    for(j=0; j<level; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"+-- CandlProgram\n") ;
  
    /* A blank line. */
    for(j=0; j<=level+1; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"\n") ;

    /* Print the context. */
    candl_matrix_print_structure(file,program->context,level+1) ;
    
    /* A blank line. */
    for(j=0; j<=level+1; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"\n") ;

    /* Go to the right level and print the statement number. */
    for(j=0; j<=level; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"Statement number: %d\n",program->nb_statements) ;

    /* A blank line. */
    for(j=0; j<=level+1; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"\n") ;

    /* Print the statements. */
    i= 0 ;
    do
    { candl_statement_print_structure(file,program->statement[i],level+1) ;
      i++ ;
    }
    while (i < program->nb_statements) ;
        
    /* Print the transformation candidate. */
    if (program->transformation != NULL)
    { for(i=0; i<program->nb_statements; i++)
      candl_matrix_print_structure(file,program->transformation[i],level+1) ;
    }
    else
    { /* Go to the right level. */
      for(j=0; j<=level; j++)
      fprintf(file,"|\t") ;
      fprintf(file,"+-- No transformation candidate\n") ;
      
      /* A blank line. */
      for(j=0; j<=level+1; j++)
      fprintf(file,"|\t") ;
      fprintf(file,"\n") ;  
    }
  }
  else
  { /* Go to the right level. */
    for(j=0; j<level; j++)
    fprintf(file,"|\t") ;
    fprintf(file,"+-- NULL CandlProgram\n") ;
  }
  
  /* The last line. */
  for(j=0; j<=level; j++)
  fprintf(file,"|\t") ;
  fprintf(file,"\n") ;
}


/* candl_program_print function:
 * This function prints the content of a CandlProgram structure (program) into a
 * file (file, possibly stdout).
 */
void candl_program_print(FILE * file, CandlProgram * program)
{ candl_program_print_structure(file,program,0) ;
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/* candl_program_free function:
 * This function frees the allocated memory for a CandlProgram structure, it
 * recursively frees everything inside.
 */
void candl_program_free(CandlProgram * program)
{ int i ;

  candl_matrix_free(program->context) ;
  
  if (program->statement != NULL)
  { for (i=0; i<program->nb_statements; i++)
    candl_statement_free(program->statement[i]) ;
    free(program->statement) ;
  }

  if (program->transformation != NULL)
  { for (i=0; i<program->nb_statements; i++)
    candl_matrix_free(program->transformation[i]) ;
    free(program->transformation) ;
  }

  free(program) ;
}


/******************************************************************************
 *                               Reading function                             *
 ******************************************************************************/


/* candl_program_read function:
 * This function read the informations to put in a CandlProgram structure from
 * a file (file, possibly stdin). It returns a pointer to a CandlProgram
 * structure containing the read informations.
 * September 10th 2003: first version.
 */
CandlProgram * candl_program_read(FILE * file)
{ int i, nb_statements, nb_parameters, nb_functions ;
  char s[CANDL_MAX_STRING] ;
  CandlStatement ** statement ;
  CandlMatrix    ** transformation ;
  CandlProgram    * program ;
  
  /* Memory allocation for the CandlProgram structure. */
  program = candl_program_malloc() ;

  /* First of all, we read the context data. */
  program->context  = candl_matrix_read(file) ;
  nb_parameters = program->context->NbColumns - 2 ;

  /* We read the number of statements. */
  while (fgets(s,CANDL_MAX_STRING,file) == 0) ;
  while ((*s=='#'||*s=='\n') || (sscanf(s," %d",&nb_statements)<1))
  fgets(s,CANDL_MAX_STRING,file) ;  
  
  program->nb_statements = nb_statements ;
  
  /* Reading of each statement. */
  if (nb_statements > 0)
  { /* Memory allocation for the array of pointers to the statements. */
    statement = (CandlStatement**)malloc(nb_statements*sizeof(CandlStatement*));
    if (statement == NULL) 
    { fprintf(stderr, "[Candl]ERROR: memory overflow.\n") ;
      exit(1) ;
    }

    for (i=0; i<nb_statements; i++)
    statement[i] = candl_statement_read(file,i,nb_parameters) ;
    
    program->statement = statement ;
  }
  
  /* We read the number of transformation functions. */
  while (fgets(s,CANDL_MAX_STRING,file) == 0) ;
  while ((*s=='#' || *s=='\n') || (sscanf(s," %d",&nb_functions)<1))
  fgets(s,CANDL_MAX_STRING,file) ;
  
  /* Reading of each transformation function. */
  if (nb_functions > 0)
  { /* The function number must be the same as statement number. */
    if (nb_functions != nb_statements)
    { fprintf(stderr, "[Candl]ERROR: the numbers of transformations (%d) and "
                      "statements (%d) differ.\n",nb_functions,nb_statements) ;
      exit(1) ;
    }
  
    /* Memory allocation for the array of pointers to the functions. */
    transformation = (CandlMatrix **)malloc(nb_functions*sizeof(CandlMatrix *));
    if (transformation == NULL) 
    { fprintf(stderr, "[Candl]ERROR: memory overflow.\n") ;
      exit(1) ;
    }

    for (i=0; i<nb_functions; i++)
    transformation[i] = candl_matrix_read(file) ;

    program->transformation = transformation ;
  }
 
  return(program) ;
}


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/


/**
 * candl_program_malloc function:
 * This function allocates the memory space for a CandlProgram structure and
 * sets its fields with default values. Then it returns a pointer to the
 * allocated space.
 * - 09/12/2005: first version.
 */
CandlProgram * candl_program_malloc()
{ CandlProgram * program ;

  /* Memory allocation for the CandlProgram structure. */
  program = (CandlProgram *)malloc(sizeof(CandlProgram)) ;
  if (program == NULL) 
  { fprintf(stderr, "[Candl]ERROR: memory overflow.\n") ;
    exit(1) ;
  }
  
  /* We set the various fields with default values. */
  program->context        = NULL ;
  program->nb_statements  = 0 ;
  program->statement      = NULL ;
  program->transformation = NULL ;
  
  return program ;
}
