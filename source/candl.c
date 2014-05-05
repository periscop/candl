
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
#include <osl/extensions/dependence.h>
#include <candl/candl.h>
#include <candl/dependence.h>
#include <candl/violation.h>
#include <candl/options.h>
#include <candl/scop.h>
#include <candl/util.h>


int main(int argc, char * argv[]) {
  
  osl_scop_p scop = NULL;
  osl_scop_p orig_scop = NULL;
  osl_dependence_p dep = NULL;
  int num_scops, i=0;
  candl_options_p options;
  candl_violation_p *violations = NULL;
  FILE *input, *output, *input_test;
  int precision;
  #if defined(PIPLIB_INT_SP)
    precision = OSL_PRECISION_SP;
  #elif defined(PIPLIB_INT_DP)
    precision = OSL_PRECISION_DP;
  #elif defined(PIPLIB_INT_GMP)
    precision = OSL_PRECISION_MP;
  #endif

  /* Options and input/output file setting. */
  candl_options_read(argc, argv, &input, &output, &input_test, &options);
  
  /* Open the scop
   * If there is no original scop (given with the -test), the input file
   * is considered as the original scop
   */
  osl_interface_p registry = osl_interface_get_default_registry();
  if (input_test != NULL) {
    scop = osl_scop_pread(input, registry, precision);
    orig_scop = osl_scop_pread(input_test, registry, precision);
  } else {
    orig_scop = osl_scop_pread(input, registry, precision);
  }
  osl_interface_free(registry);
  
  if (orig_scop == NULL || (scop == NULL && input_test != NULL)) {
    CANDL_error("Fail to open scop");
    exit(1);
  }
  
  /* Check if we can compare the two scop
   * The function compare only domains and access arrays
   */
  if (input_test != NULL && !candl_util_check_scop_list(orig_scop, scop))
    CANDL_error("The two scop lists can't be compared");
  
  /* Add more infos (depth, label, ...)
   * Not important for the transformed scop
   * TODO : the statements must be sorted to compute the statement label
   *        the problem is if the scop is reordered, the second transformed scop
   *        must be aligned with it
   */
  candl_scop_usr_init(orig_scop);
  
  /* Calculating dependences. */
  candl_dependence_add_extension(orig_scop, options);

  osl_scop_p s1 = orig_scop;
  while (s1) {num_scops++; s1 = s1->next;}

  s1 = orig_scop;
  osl_scop_p s2 = scop;
  /* Calculating legality violations. */
  if (input_test != NULL) {
    CANDL_malloc(violations, candl_violation_p, num_scops);

    for (i=0; i< num_scops; i++, s1 = s1->next, s2 = s2->next) {
      dep = osl_generic_lookup(s1->extension, 
                                            OSL_URI_DEPENDENCE);
      violations[i] = candl_violation(s1, dep, s2, options);
    }
  }

  /* Printing data structures if asked. */
  if (options->structure) {

    if (input_test != NULL) {
      s1=orig_scop; s2=scop;
      for (i=0; i< num_scops; i++, s1=s1->next, s2=s2->next) {
        fprintf(output, "\033[33mORIGINAL SCOP:\033[00m\n");
        osl_scop_print(output, s1);
        fprintf(output, "\n\033[33mTRANSFORMED SCOP:\033[00m\n");
        osl_scop_print(output, s2);

        dep = osl_generic_lookup(s1->extension, OSL_URI_DEPENDENCE);
        fprintf(output, "\n\033[33mDEPENDENCES GRAPH:\033[00m\n");
        candl_dependence_pprint(output, dep);
        fprintf(output, "\n\n\033[33mVIOLATIONS GRAPH:\033[00m\n");
        candl_violation_pprint(output, violations[i]);
      }
    } else {
      s1=orig_scop;
      for (i=0; i< num_scops; i++, s1=s1->next) {
        fprintf(output, "\033[33mORIGINAL SCOP:\033[00m\n");
        osl_scop_print(output, s1);

        dep = osl_generic_lookup(s1->extension, OSL_URI_DEPENDENCE);
        fprintf(output, "\n\033[33mDEPENDENCES GRAPH:\033[00m\n");
        candl_dependence_pprint(output, dep);
      }
    }

  } else if (input_test != NULL) {
     /* Printing violation graph */
    for (i=0; i< num_scops; i++) {
      candl_violation_pprint(output, violations[i]);
      if (options->view)
        candl_violation_view(violations[i]);
    }
  } else if (options->outscop) {
    /* Export to the scop format */
    s1=orig_scop;
    osl_scop_print(output, s1); //prints list
  } else {
    /* Printing dependence graph if asked or if there is no transformation. */
    s1=orig_scop;
    for(i=0; i< num_scops; i++, s1=s1->next){
      dep = osl_generic_lookup(s1->extension, OSL_URI_DEPENDENCE);
      fprintf(output, "\033[33mSCOP #%d:\033[00m\n", i+1);
      candl_dependence_pprint(output, dep);
      fprintf(output, "\n");
      if (options->view)
        candl_dependence_view(dep);
    }
  }

  /* Being clean. */
  if (input_test != NULL) {
    for (i=0; i< num_scops; i++)
      candl_violation_free(violations[i]);
    osl_scop_free(scop);
    fclose(input_test);
  }
  
  // the dependence is freed with the scop
  //osl_dependence_free(orig_dependence); 
  candl_options_free(options);
  candl_scop_usr_cleanup(orig_scop);
  osl_scop_free(orig_scop);
  fclose(input);
  fclose(output);
  pip_close();
  
  return 0;
}
