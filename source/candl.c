
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                    candl.c                              **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: september 8th 2003               **
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
 * CAnDL, the Chunky Dependence Analyser                                      *
 * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                         *
 *                                                                            *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <osl/scop.h>
#include <osl/macros.h>
#include <osl/util.h>
#include <clay/beta.h>
#include <candl/candl.h>
#include <candl/dependence.h>
#include <candl/violation.h>
#include <candl/options.h>
#include <candl/usr.h>


int main(int argc, char * argv[]) {
  
  osl_scop_p scop = NULL;
  osl_scop_p orig_scop = NULL;
  candl_options_p options;
  candl_dependence_p orig_dependence = NULL;
  candl_violation_p violation = NULL;
  FILE *input, *output, *outcandl, *incandl, *input_test;

  /* Options and input/output file setting. */
  candl_options_read(argc, argv, &input, &output, &outcandl, &incandl,
                     &input_test, &options);
  
  /* Open the scop
   * If there is no original scop (given with the -test), the input file
   * is considered as the original scop
   */
  osl_interface_p registry = osl_interface_get_default_registry();
  #if defined(LINEAR_VALUE_IS_INT)
    if (input_test != NULL) {
      scop = osl_scop_pread(input, registry, OSL_PRECISION_SP);
      orig_scop = osl_scop_pread(input_test, registry, OSL_PRECISION_SP);
    } else {
      orig_scop = osl_scop_pread(input, registry, OSL_PRECISION_SP);
    }
  #elif defined(LINEAR_VALUE_IS_LONGLONG)
    if (input_test != NULL) {
      scop = osl_scop_pread(input, registry, OSL_PRECISION_DP);
      orig_scop = osl_scop_pread(input_test, registry, OSL_PRECISION_DP);
    } else {
      orig_scop = osl_scop_pread(input, registry, OSL_PRECISION_DP);
    }
  #elif defined(LINEAR_VALUE_IS_MP)
    if (input_test != NULL) {
      scop = osl_scop_pread(input, registry, OSL_PRECISION_MP);
      orig_scop = osl_scop_pread(input_test, registry, OSL_PRECISION_MP);
    } else {
      orig_scop = osl_scop_pread(input, registry, OSL_PRECISION_MP);
    }
  #endif
  osl_interface_free(registry);
  
  if (orig_scop == NULL || (scop == NULL && input_test != NULL)) {
    CANDL_FAIL("Fail to open scop");
    exit(1);
  }
  
  
  if (input_test != NULL && !candl_util_check_scop(orig_scop, scop))
    CANDL_FAIL("The two scop can't be compared");
  
  /* Add more infos (depth, label, ...)
   * Not important for the transformed scop
   */
  candl_usr_init(orig_scop);
  
  /* Calculating dependences. */
  if (incandl != NULL) {
    /* Read dependences from the candl tag */
    // TODO : make an osl extension
    char *content = NULL;
    int f_size;
    char *tmp;
    fseek(incandl, 0, SEEK_END);
    f_size = ftell(incandl);
    fseek(incandl, 0, SEEK_SET);
    content = (char*) malloc(sizeof(char) * f_size);
    if (fread(content, 1, f_size, incandl) > 0) {
      tmp = osl_util_tag_content(content, "candl");
      free(content);
      content = osl_util_tag_content(tmp, "dependence-polyhedra");
      free(tmp);
      orig_dependence = candl_dependence_read_from_scop(orig_scop, content);
    }
    free(content);
  } else {
    orig_dependence = candl_dependence(orig_scop, options);
  }

  /* Calculating legality violations. */
  if (input_test != NULL)
    violation = candl_violation(orig_scop, orig_dependence, scop, options);

  /* Printing data structures if asked. */
  if (options->structure) {
    if (input_test != NULL) {
      fprintf(output, "\033[33mORIGINAL SCOP:\033[00m\n");
      osl_scop_print(output, orig_scop);
      fprintf(output, "\n\033[33mTRANSFORMED SCOP:\033[00m\n");
      osl_scop_print(output, scop);
      fprintf(output, "\n\033[33mDEPENDENCES:\033[00m\n");
      candl_dependence_pprint(output, orig_dependence);
      fprintf(output, "\n\n\033[33mVIOLATIONS:\033[00m\n");
      candl_violation_pprint(output, violation);
    } else {
      fprintf(output, "\033[33mORIGINAL SCOP:\033[00m\n");
      osl_scop_print(output, orig_scop);
      fprintf(output, "\n\033[33mDEPENDENCES:\033[00m\n");
      candl_dependence_pprint(output, orig_dependence);
    }
    
  } else if (options->outscop) {
    /* Update the scop */
    osl_scop_print(output, orig_scop);
    if (outcandl != NULL)
      candl_dependence_print(outcandl, orig_dependence);
  } else {
    /* Printing dependence graph if asked or if there is no transformation. */
    if (input_test == NULL) {
      candl_dependence_pprint(output, orig_dependence);
      if (options->view)
        candl_dependence_view(orig_dependence);
    }
    /* Printing violation graph if asked and if there is a transformation. */
    else {
      candl_violation_pprint(output, violation);
      if (options->view)
        candl_violation_view(violation);
    }
  }

  /* Being clean. */
  if (input_test != NULL) {
    candl_violation_free(violation);
    osl_scop_free(scop);
    fclose(input_test);
  }
  
  
  candl_dependence_free(orig_dependence);
  candl_usr_cleanup(orig_scop);
  osl_scop_free(orig_scop);
  candl_options_free(options);
  pip_close();
  fclose(input);
  fclose(output);
  
  if (outcandl != stdout)
    fclose(outcandl);
  
  if (incandl)
    fclose(incandl);

  return 0;
}
