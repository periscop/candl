

   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                   scop.c                                **
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

#include <stdlib.h>
#include <osl/scop.h>
#include <osl/statement.h>
#include <osl/extensions/dependence.h>
#include <osl/relation.h>
#include <candl/macros.h>
#include <candl/scop.h>
#include <candl/statement.h>
#include <candl/util.h>

/**
 * candl_scop_usr_init function:
 * initialize a candl_scop_usr structure
 *
 * May, 2013 extended to each scop in the list,
 */
void candl_scop_usr_init(osl_scop_p scop) {

  while (scop) {
    candl_scop_usr_p scop_usr;
    
    /* Init the scop_usr structure */
    scop_usr = (candl_scop_usr_p) malloc(sizeof(candl_scop_usr_t));
    scop_usr->scalars_privatizable = NULL;
    scop_usr->usr_backup           = scop->usr;
    scop->usr = scop_usr;
    
    candl_statement_usr_init_all(scop);

    scop = scop->next;
  }
}


/**
 * candl_scop_usr_cleanup function:
 */
void candl_scop_usr_cleanup(osl_scop_p scop) {

  while (scop) {
    osl_statement_p statement = scop->statement;
    candl_scop_usr_p scop_usr;
    while (statement != NULL) {
      candl_statement_usr_cleanup(statement);
      statement = statement->next;
    }
    scop_usr = scop->usr;
    if (scop_usr) {
      if (scop_usr->scalars_privatizable)
        free(scop_usr->scalars_privatizable);
      scop->usr = scop_usr->usr_backup;
      free(scop_usr);
    }
   
    scop = scop->next;
  }
}
