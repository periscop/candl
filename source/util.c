
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                   util.c                                **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: june 7th 2012                    **
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

/*
 * author Joel Poudroux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <candl/util.h>
#include <candl/macros.h>
#include <osl/scop.h>
#include <osl/statement.h>
#include <osl/body.h>
#include <osl/strings.h>


static int find_body(osl_body_p body, osl_statement_p *stmts, int size) {
  /* here stmts is not a chained list ! */
  char *str = body->expression->string[0];
  char *tmp;
  int i;
  for (i = 0 ; i < size ; i++) {
    tmp = ((osl_body_p) stmts[i]->body->data)->expression->string[0];
    if (strcmp(tmp, str) == 0)
      break;
  }
  return (i == size ? -1 : i);
}


/**
 * candl_util_scop_align function:
 * Align the scop s2 with the s1
 * That is to say, the first statement in the list of s1 corresponds to
 * the first statement in the list of s2
 * The alignment is based on the body string
 */
void candl_util_scop_align(osl_scop_p s1, osl_scop_p s2) {
  
  /* TODO
   * We can maybe use a better comparaison instead of the body string
   */
   
  osl_body_p body;
  osl_statement_p it1, it2, tmp;
  osl_statement_p *buff;
  int nb_stmts = 0;
  int sizebuff = 50;
  int i, curr;
  
  /* Create an array containing the statements of s2 */
  buff = (osl_statement_p*) malloc(sizeof(osl_statement_p) * sizebuff);
  for (it1 = s1->statement, it2 = s2->statement ; 
       it1 != NULL && it2 != NULL ;
       it1 = it1->next, it2 = it2->next) {
    buff[nb_stmts++] = it2;
    if (nb_stmts == sizebuff) {
      sizebuff *= 2;
      buff = (osl_statement_p*) realloc(buff, sizeof(osl_statement_p) * sizebuff);
    }
  }
  
  if (it1 != it2) {
    CANDL_FAIL("The two scop haven't the same number of statements");
  }

  /* Align s2 with s1 */
  curr = 0;
  for (it1 = s1->statement ; it1 != NULL ; it1 = it1->next) {
    body = (osl_body_p) it1->body->data;
    i = find_body(body, buff, nb_stmts);
    if (i == -1) {
      fprintf(stderr, 
        "[Candl] Error: The statement '%s' was not found in the input scop\n",
        body->expression->string[0]);
      exit(1);
    }
    if (curr != i) {
      tmp = buff[curr];
      buff[curr] = buff[i];
      buff[i] = tmp;
    }
    curr++;
  }
  
  /* Reconstruct the statements list of s2 */
  s2->statement = buff[0];
  for (i = 0 ; i < nb_stmts-1 ; i++) {
    buff[i]->next = buff[i+1];
  }
  buff[i]->next = NULL;
  
  free(buff);
}
