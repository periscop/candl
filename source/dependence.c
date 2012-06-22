
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
 * Copyright (C) 2003-2008 Cedric Bastoul                                     *
 *                                                                            *
 * This is free software; you can redistribute it and/or modify it under the  *
 * terms of the GNU Lesser General Public License as published by the Free    *
 * Software Foundation; either version 3 of the License, or (at your option)  *
 * any later version.                                                         *
 *                                                                            *
 * This software is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * for more details.                                                          *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with software; if not, write to the Free Software Foundation, Inc.,  *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                     *
 *                                                                            *
 * CAnDL, the Chunky Dependence Analyzer                                      *
 * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                         *
 *                                                                            *
 ******************************************************************************/
/**
 * \file dependence.c
 * \author Cedric Bastoul and Louis-Noel Pouchet
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <candl/candl.h>
#include <candl/usr.h>
#include <candl/matrix.h>
#include <candl/piplib-wrapper.h>
#include <osl/macros.h>
#include <osl/scop.h>
#include <osl/statement.h>
#include <osl/relation.h>
#include <clay/beta.h>

#include <assert.h>

#ifdef CANDL_SUPPORTS_ISL
# undef Q // Thank you polylib...
# include <isl/int.h>
# include <isl/constraint.h>
# include <isl/ctx.h>
# include <isl/set.h>
#endif



/**
 * candl_dependence_get_relation_refs_in_dep function:
 * This function return the corresponding osl_relation_p of
 * the ref_source and ref_target
 */
void candl_dependence_get_relation_refs_in_dep(candl_dependence_p tmp, 
              osl_relation_p *src, osl_relation_p *targ) {
  
  osl_relation_list_p access;
  osl_relation_p elt = NULL;
  int count;
  
  /* Source ref */
  count = 0;
  access = tmp->source->access;
  for (; access != NULL ; access = access->next) {
    elt = access->elt;
    if (count == tmp->ref_source)
      break;
    count++;
  }
  *src = elt;

  /* Target ref */
  count = 0;
  access = tmp->target->access;
  for (; access != NULL ; access = access->next) {
    elt = access->elt;
    if (count == tmp->ref_target)
      break;
    count++;
  }
  *targ = elt;
}


/**
 * candl_dependence_get_array_refs_in_dep function:
 * This function return the array indices referenced in the
 * dependence.
 */
void candl_dependence_get_array_refs_in_dep(candl_dependence_p tmp,
                                            int* refs, int* reft) {
  if (! tmp)
    return;
  
  /* TODO : optimization
   * Maybe we can add a pointer to access to arrays directly
   */
  
  osl_relation_p src, targ;
  int row;
  int precision = tmp->domain->precision;
  
  candl_dependence_get_relation_refs_in_dep(tmp, &src, &targ);
    
  row = clay_relation_get_line(src, 0);
  *refs = osl_int_get_si(precision,
                         src->m[row],
                         src->nb_columns-1);

  row = clay_relation_get_line(targ, 0);
  *reft = osl_int_get_si(precision,
                         targ->m[row],
                         targ->nb_columns-1);
}


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/


/**
 * candl_dependence_idump function:
 * Displays a CandlDependence structure (dependence) into a file (file,
 * possibly stdout) in a way that trends to be understandable without falling
 * in a deep depression or, for the lucky ones, getting a headache... It
 * includes an indentation level (level) in order to work with others
 * idump functions.
 * - 18/09/2003: first version.
 */
void candl_dependence_idump(FILE* file,
                            candl_dependence_p dependence,
                            int level) {
  int j, first = 1;
  osl_statement_p tmp;
  candl_statement_usr_p usr;

  if (dependence != NULL) { /* Go to the right level. */
    for (j=0; j<level; j++)
      fprintf(file, "|\t");
    fprintf(file, "+-- CandlDependence\n");
  } else {
    for (j=0; j<level; j++)
    fprintf(file, "|\t");
    fprintf(file, "+-- NULL dependence\n");
  }

  while (dependence != NULL) {
    if (!first) { /* Go to the right level. */
      for (j=0; j<level; j++)
        fprintf(file, "|\t");
      fprintf(file, "|   CandlDependence\n");
    } else {
      first = 0;
    }

    /* A blank line. */
    for (j=0; j<=level+1; j++)
      fprintf(file, "|\t");
    fprintf(file, "\n");

    /* Go to the right level and print the type. */
    for (j=0; j<=level; j++)
      fprintf(file, "|\t");
    fprintf(file, "Type: ");
    switch (dependence->type) {
      case CANDL_UNSET : fprintf(file, "UNSET\n");        break;
      case CANDL_RAW   : fprintf(file, "RAW (flow)\n");   break;
      case CANDL_WAR   : fprintf(file, "WAR (anti)\n");   break;
      case CANDL_WAW   : fprintf(file, "WAW (output)\n"); break;
      case CANDL_RAR   : fprintf(file, "RAR (input)\n");  break;
      case CANDL_RAW_SCALPRIV   : fprintf(file, "RAW_SCALPRIV (scalar priv)\n");  break;
      default : fprintf(file, "unknown\n"); break;
    }

    /* A blank line. */
    for (j=0; j<=level+1; j++)
      fprintf(file, "|\t");
    fprintf(file, "\n");

    /* Go to the right level and print the depth. */
    for (j=0; j<=level; j++)
      fprintf(file, "|\t");
    fprintf(file, "Depth: %d\n", dependence->depth);

    /* A blank line. */
    for (j=0; j<=level+1; j++)
      fprintf(file, "|\t");
    fprintf(file, "\n");
    
    /* Ref source and target */
    for (j=0; j<=level; j++)
      fprintf(file, "|\t");
    fprintf(file, "Ref source: %d, Ref target: %d\n",
            dependence->ref_source, dependence->ref_target);
    
    /* A blank line. */
    for (j=0; j<=level+1; j++)
      fprintf(file, "|\t");
    fprintf(file, "\n");

    /* Print the source statement. */
    usr = dependence->source->usr;
    for (j=0; j<=level; j++)
      fprintf(file, "|\t");
    fprintf(file, "Statement label: %d\n", usr->label);
    tmp = dependence->source->next;
    dependence->source->next = NULL;
    osl_statement_idump(file, dependence->source, level+1);
    dependence->source->next = tmp;

    /* Print the target statement. */
    usr = dependence->target->usr;
    for (j=0; j<=level; j++)
      fprintf(file, "|\t");
    fprintf(file, "Statement label: %d\n", usr->label);
    usr = dependence->target->usr;
    tmp = dependence->target->next;
    dependence->target->next = NULL;
    osl_statement_idump(file, dependence->target, level+1);
    dependence->target->next = tmp;

    /* Print the dependence polyhedron. */
    osl_relation_idump(file, dependence->domain, level+1);

    dependence = dependence->next;

    /* Next line. */
    if (dependence != NULL) {
      for (j=0; j<=level; j++)
        fprintf(file, "|\t");
      fprintf(file, "V\n");
    }
  }

  /* The last line. */
  for (j=0; j<=level; j++)
    fprintf(file, "|\t");
  fprintf(file, "\n");
}


/* candl_dependence_print function:
 * This function prints the content of a CandlDependence structure (dependence)
 * into a file (file, possibly stdout).
 */
void candl_dependence_dump(FILE * file, candl_dependence_p dependence) {
  candl_dependence_idump(file, dependence, 0);
}


/* candl_dependence_print_graph function:
 * This function prints the content of a CandlDependence structure (dependence)
 * into a file (file, possibly stdout) as a Graphviz input file.
 * See http://www.graphviz.org
 * - 08/12/2005: first version.
 */
void candl_dependence_pprint(FILE * file, candl_dependence_p dependence) {
  int i = 0;
  candl_statement_usr_p s_usr;
  candl_statement_usr_p t_usr;
  
  fprintf(file, "digraph G {\n");

  fprintf(file, "# Data Dependence Graph\n");
  fprintf(file, "# Generated by Candl "CANDL_RELEASE" "CANDL_VERSION" bits\n");
  while (dependence != NULL) {
    s_usr = dependence->source->usr;
    t_usr = dependence->target->usr;
    fprintf(file, "  S%d -> S%d [label=\" ", s_usr->label, t_usr->label);
    switch (dependence->type) {
      case CANDL_UNSET : fprintf(file, "UNSET"); break;
      case CANDL_RAW   : fprintf(file, "RAW") ; break;
      case CANDL_WAR   : fprintf(file, "WAR") ; break;
      case CANDL_WAW   : fprintf(file, "WAW") ; break;
      case CANDL_RAR   : fprintf(file, "RAR") ; break;
      case CANDL_RAW_SCALPRIV   : fprintf(file, "RAW_SCALPRIV (scalar-priv)") ; break;
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
void candl_dependence_view(candl_dependence_p dependence) {
  FILE * temp_output;

  temp_output = fopen(CANDL_TEMP_OUTPUT, "w+");
  candl_dependence_pprint(temp_output, dependence);
  fclose(temp_output);
  /* check the return to suppress the warning compilation */
  if(system("dot -Tps "CANDL_TEMP_OUTPUT" | gv - && rm -f "CANDL_TEMP_OUTPUT" &"))
    ;
}


/**
 * Print the dependence, formatted to fit the .scop representation.
 *
 */
void candl_dependence_print(FILE *file, candl_dependence_p dependence) {
  char *string;
  candl_dependence_sprint(&string, dependence);
  fprintf(file, "%s\n", string);
}


/**
 * Returns a string containing the dependence, formatted to fit the
 * .scop representation.
 *
 */
void candl_dependence_sprint(char **string, candl_dependence_p dependence) {
  
  candl_dependence_p tmp = dependence;
  candl_statement_usr_p s_usr;
  candl_statement_usr_p t_usr;
  int refs = 0, reft = 0;
  int i, j, k;
  int nb_deps;
  int buffer_size = 2048;
  int szbuff = 0;
  int precision;
  char* buffer = (char*) malloc(buffer_size * sizeof(char));
  char buff[OSL_MAX_STRING];
  char* type;
  char* pbuffer;
  
  sprintf(buffer, "<candl>\n<dependence-polyhedra>\n");
  
  for (tmp = dependence, nb_deps = 0; tmp; tmp = tmp->next, ++nb_deps)
   ;
  snprintf(buff, OSL_MAX_STRING, "# Number of dependences\n%d\n", nb_deps);
  strcat(buffer, buff);
  
  if (nb_deps) {
    precision = dependence->domain->precision;
    
    for (tmp = dependence, nb_deps = 1; tmp; tmp = tmp->next, ++nb_deps) {
      /* Compute the type of the dependence, and the array id
         accessed. */
      candl_dependence_get_array_refs_in_dep(tmp, &refs, &reft);
      
      switch (tmp->type) {
        case CANDL_UNSET:
          type = "UNSET";
          break;
        case CANDL_RAW:
          type = "RAW #(flow)";
          break;
        case CANDL_WAR:
          type = "WAR #(anti)";
          break;
        case CANDL_WAW:
          type = "WAW #(output)";
          break;
        case CANDL_RAR:
          type = "RAR #(input)";
          break;
        case CANDL_RAW_SCALPRIV:
          type = "RAW_SCALPRIV #(scalar priv)";
          break;
        default:
          exit(1);
          break;
      }
      /* Quick consistency check. */
      if (refs != reft)
        CANDL_FAIL("Internal error. refs != reft\n");
      /* Output dependence information. */
      s_usr = tmp->source->usr;
      t_usr = tmp->target->usr;
      snprintf(buff, OSL_MAX_STRING, "# Description of dependence %d\n"
              "# type\n%s\n# From statement id\n%d\n"
              "# To statement id\n%d\n# Depth \n%d\n# Array id\n%d\n"
              "# Dependence domain\n%d %d\n", nb_deps, type,
              s_usr->label,
              t_usr->label, tmp->depth,
              refs, tmp->domain->nb_rows, tmp->domain->nb_columns);
      strcat(buffer, buff);
      /* Output dependence domain. */
      pbuffer = buffer + strlen(buffer);
      for (i = 0; i < tmp->domain->nb_rows; ++i) {
        for (j = 0; j < tmp->domain->nb_columns; ++j) {
          osl_int_sprint(buff, precision, tmp->domain->m[i], j);
          szbuff = strlen(buff);
          if (szbuff == 2)
            *(pbuffer++) = ' ';
          for (k = 0; k < szbuff; ++k)
            *(pbuffer++) = buff[k];
        }
        *(pbuffer++) = '\n';
      }
      *(pbuffer++) = '\0';
      /* Increase the buffer size if needed. Conservatively assume a
         dependence is never larger than 2k. */
      szbuff = strlen(buffer);
      while (szbuff + strlen(pbuffer) >= buffer_size) {
        buffer_size *= 2;
        OSL_realloc(buffer, char *, buffer_size * sizeof(char));
        if (buffer == NULL)
          CANDL_FAIL("Error: memory overflow");
        buffer[szbuff] = '\0';
      }
    }
  }
  
  while (szbuff + strlen("</dependence-polyhedra>\n</candl>") >= buffer_size) {
    buffer_size += 2048;
    OSL_realloc(buffer, char *, buffer_size * sizeof(char));
  }
  strcat(buffer, "</dependence-polyhedra>\n</candl>");

  *string = buffer;
}

#ifdef CANDL_SUPPORTS_ISL

struct isl_set* isl_set_from_piplib_matrix(struct isl_ctx* ctx,
                                           PipMatrix* matrix,
                                           int nparam);
PipMatrix* isl_set_to_piplib_matrix(struct isl_ctx* ctx,
                                    struct isl_set* set,
                                    int nparam);
/**
 * candl_dependence_isl_simplify function:
 *
 * This function uses ISL to simplify the dependence polyhedra.
 * Useful for polyhedra that contain large coefficient values.
 *
 */
candl_dependence_p candl_dependence_isl_simplify(candl_dependence_p dep,
                                                 osl_scop_p scop) {
  /*if (dep == NULL || scop == NULL)
    return dep;

  candl_dependence_p tmp;
  PipMatrix* context = (PipMatrix*) scop->context;
  int nparam = context->nb_parameters;

  struct isl_ctx* ctx = isl_ctx_alloc ();

  for (tmp = dep; tmp; tmp = tmp->next) {
    // 1- Convert the dependence polyhedron into ISL set.
    struct isl_set* set =
        isl_set_from_piplib_matrix(ctx, tmp->domain, nparam);

    // 2- Simplify the ISL set.
    set = isl_set_detect_equalities(set);

    // 3- Convert back into Candl matrix representation.
    PipMatrix* newdom = isl_set_to_piplib_matrix(ctx, set, nparam);
    isl_set_free (set);
    candl_matrix_free (tmp->domain);
    tmp->domain = (CandlMatrix*) newdom;
  }

  /// FIXME: Some dead ref.
  //isl_ctx_free (ctx);

  return dep;*/
  return NULL;
}

#endif



/**
 * Read one dependence from a string.
 *
 */
candl_dependence_p candl_dependence_read_one_dep(char* str, char** next,
                                                 osl_scop_p scop) {
  candl_dependence_p dep = candl_dependence_malloc();
  osl_statement_p iter;
  osl_relation_p elt;
  osl_relation_list_p access;
  candl_statement_usr_p usr;
  char buffer[1024];
  int tmp;
  int i, j, k;
  int id;
  int id2;
  int precision = scop->context->precision;
  int row;
  
  /* # Description of dependence xxx */
  for (; *str != '\n'; ++str)
   ;
  ++str;

  /* # type */
  for (; *str != '\n'; ++str)
   ;
  ++str;

  /* {RAW,RAR,WAR,WAW} #(type) */
  for (i = 0; *str != ' '; ++str, ++i)
    buffer[i] = *str;
  
  buffer[i] = '\0';
  if (! strcmp(buffer, "RAW"))
    dep->type = CANDL_RAW;
  else if (! strcmp(buffer, "RAR"))
    dep->type = CANDL_RAR;
  else if (! strcmp(buffer, "WAR"))
    dep->type = CANDL_WAR;
  else if (! strcmp(buffer, "WAW"))
    dep->type = CANDL_WAW;
  else if (! strcmp(buffer, "RAW_SCALPRIV"))
    dep->type = CANDL_RAW_SCALPRIV;
  for (; *str != '\n'; ++str);
  ++str;

  /* # From statement xxx */
  for (; *str != '\n'; ++str);
  ++str;
  /* stmt-id */
  for (i = 0; *str != '\n'; ++str, ++i)
    buffer[i] = *str;
  ++str;
  buffer[i] = '\0';
  id = atoi(buffer);
  
  iter = scop->statement;
  for (; iter != NULL ; iter = iter->next) {
    usr = iter->usr;
    if (usr->label == id) {
      dep->source = iter;
      break;
    }
  }

  /* # To statement xxx */
  for (; *str != '\n'; ++str);
  ++str;
  /* stmt-id */
  for (i = 0; *str != '\n'; ++str, ++i)
    buffer[i] = *str;
  ++str;
  buffer[i] = '\0';
  id = atoi(buffer);
  
  iter = scop->statement;
  for (; iter != NULL ; iter = iter->next) {
    usr = iter->usr;
    if (usr->label == id) {
      dep->target = iter;
      break;
    }
  }

  /* # Depth */
  for (; *str != '\n'; ++str)
    ;
  ++str;
  /* depth */
  for (i = 0; *str != '\n'; ++str, ++i)
    buffer[i] = *str;
  ++str;
  buffer[i] = '\0';
  dep->depth = atoi(buffer);

  /* # Array id */
  for (; *str != '\n'; ++str)
    ;
  ++str;
  /* array-id */
  for (i = 0; *str != '\n'; ++str, ++i)
    buffer[i] = *str;
  ++str;
  buffer[i] = '\0';
  id = atoi(buffer);
  
  /* Search the ref in the source access */
  access = dep->source->access;
  i = 0;
  for (; access != NULL ; access = access->next) {
    elt = access->elt;
    row = clay_relation_get_line(elt, 0);
    tmp = osl_int_get_si(precision,
                         elt->m[row],
                         elt->nb_columns-1);
    if (tmp == id)
      break;
    i++;
  }
  if (access != NULL)
    dep->ref_source = i;
  
  /* Search the ref in the target access */
  access = dep->target->access;
  i = 0;
  for (; access != NULL ; access = access->next) {
    elt = access->elt;
    row = clay_relation_get_line(elt, 0);
    tmp = osl_int_get_si(precision,
                         elt->m[row],
                         elt->nb_columns-1);
    if (tmp == id)
      break;
    i++;
  }
  if (access != NULL)
    dep->ref_target = i;
  
  /* # Dependence domain */
  for (; *str != '\n'; ++str);
  ++str;

  /* nb-row nb-col */
  for (i = 0; *str != ' '; ++str, ++i)
    buffer[i] = *str;
  ++str;
  buffer[i] = '\0';
  id = atoi(buffer);
  for (i = 0; *str != '\n'; ++str, ++i)
    buffer[i] = *str;
  ++str;
  buffer[i] = '\0';
  id2 = atoi(buffer);

  dep->domain = osl_relation_pmalloc(precision, id, id2);
  /* Read matrix elements. */
  for (j = 0; j < id; ++j) {
    for (k = 0; k < id2; ++k) {
      while (*str && *str == ' ')
        str++;
      for (i = 0; *str != '\n' && *str != ' '; ++str, ++i)
        buffer[i] = *str;
      buffer[i] = '\0';
      ++str;
      osl_int_set_si(precision,
                     dep->domain->m[j], k,
                     atoi(buffer));
    }
    if (*(str - 1) != '\n') {
      for (; *str != '\n'; ++str);
      ++str;
    }
  }
  /* Store the next character in the string to be analyzed. */
  *next = str;

  return dep;
}

/**
 * Retrieve a CandlDependence list from the option tag in the scop.
 *
 */
candl_dependence_p candl_dependence_read_from_scop(osl_scop_p scop,
                                                   char *deps) {
  candl_dependence_p first = NULL;
  candl_dependence_p currdep = NULL;

  int i;
  int depcount;
  /* Get the number of dependences. */
  char buffer_nb[32];
  /* # Number of dependences */
  for (; *deps != '\n'; ++deps)
    ;
  ++deps;
  for (i = 0; *deps != '\n'; ++i, ++deps)
    buffer_nb[i] = *deps;
  buffer_nb[i] = '\0';
  ++deps;
  int nbdeps = atoi(buffer_nb);
  char* next;

  /* For each of them, read 1 and shift of the read size. */
  for (depcount = 0; depcount < nbdeps; ++depcount) {
    candl_dependence_p adep = candl_dependence_read_one_dep(deps, &next, scop);
    if (first == NULL) {
      currdep = first = adep;
    } else {
      currdep->next = adep;
      currdep = currdep->next;
    }
    deps = next;
  }

  return first;
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/* candl_dependence_free function:
 * This function frees the allocated memory for a CandlDependence structure.
 * - 18/09/2003: first version.
 */
void candl_dependence_free(candl_dependence_p dependence) {
  candl_dependence_p next;

  while (dependence != NULL) {
    next = dependence->next;
    osl_relation_free(dependence->domain);
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
candl_dependence_p candl_dependence_malloc() {
  candl_dependence_p dependence;

  /* Memory allocation for the CandlDependence structure. */
  dependence = (candl_dependence_p) malloc(sizeof(candl_dependence_t));
  if (dependence == NULL)
    CANDL_FAIL(" Error: memory overflow");

  /* We set the various fields with default values. */
  dependence->source     = NULL;
  dependence->target     = NULL;
  dependence->depth      = CANDL_UNSET;
  dependence->type       = CANDL_UNSET;
  dependence->ref_source = CANDL_UNSET;
  dependence->ref_target = CANDL_UNSET;
  dependence->domain     = NULL;
  dependence->next       = NULL;
  dependence->usr	       = NULL;
  dependence->source_nb_output_dims_domain = -1;
  dependence->source_nb_output_dims_access = -1;
  dependence->target_nb_output_dims_domain = -1;
  dependence->target_nb_output_dims_access = -1;
  dependence->source_nb_local_dims_domain  = -1;
  dependence->source_nb_local_dims_access  = -1;
  dependence->target_nb_local_dims_domain  = -1;
  dependence->target_nb_local_dims_access  = -1;

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
                          candl_dependence_p dependence) {
  if (dependence != NULL) {
    if (*start == NULL) {
      *start = dependence;
      *now = *start;
    } else {
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
static int candl_dependence_gcd(int a, int b) {
  int z = 1;

  if (a < 0)
    a *= -1;
  if (b < 0)
    b *= -1;
  if (a == 0)
    return b;
  if (b == 0)
    return a;
  if (b > a) {
    int temp = a;
    a = b;
    b = temp;
  }

  while (z != 0) {
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
static int candl_dependence_gcd_test_context(osl_relation_p system, int id) {
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
int candl_dependence_gcd_test(osl_statement_p source,
                              osl_statement_p target,
                              osl_relation_p system,
                              int level) {
  int i;
  int gcd;
  int id;
  int value;
  int null_iter, null_param, null_cst, pos_iter, neg_iter;
  int precision = source->domain->precision;
  candl_statement_usr_p s_usr = source->usr;
  candl_statement_usr_p t_usr = target->usr;

  /* Check that the precedence constraint, if any, is not strict in a
     self-dependence. */
  /* int strict_pred; */
  /* if (source == target && */
  /*     CANDL_get_si(system->p[system->NbRows - 1][0]) == 1 && */
  /*     CANDL_get_si(system->p[system->NbRows - 1][system->NbColumns - 1]) == -1) */
  /*   strict_pred = 1; */
  /* else */
  /*   strict_pred = 0; */

  /* Inspect the array access function equalities. */
  for (id = source->domain->nb_rows + target->domain->nb_rows;
       id < system->nb_rows && 
       osl_int_zero(precision, system->m[id], 0);
       ++id) {
    /* Inspect which parts of the access function equality are null,
       positive or negative. */
    null_iter = null_param = null_cst = pos_iter = neg_iter = 0;
    
    for (i = 1; i < s_usr->depth + t_usr->depth + 1 &&
         osl_int_zero(precision, system->m[id], i); ++i)
     ;
     
    if (i == s_usr->depth + t_usr->depth + 1)
      null_iter = 1;
    else
      for (pos_iter = 1, neg_iter = 1;
           i < s_usr->depth + t_usr->depth + 1; ++i) {
        if (osl_int_neg(precision, system->m[id], i))
          pos_iter = 0;
        else if (osl_int_pos(precision, system->m[id], i))
          neg_iter = 0;
      }
    for (; i < system->nb_columns - 1 && 
         osl_int_zero(precision, system->m[id], i) == 0; ++i)
     ;
    if (i == system->nb_columns - 1)
      null_param = 1;
    null_cst = osl_int_zero(precision, system->m[id], system->nb_columns - 1);

    /* Some useful ZIV/SIV/MIV tests. */
    if (null_iter && null_param && !null_cst)
      return 0;
    if (null_iter)
      if (! candl_dependence_gcd_test_context(system, id))
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
    for (i = 1, gcd = osl_int_get_si(precision, system->m[id], i);
         i < s_usr->depth + t_usr->depth; ++i)
      gcd = candl_dependence_gcd(gcd, osl_int_get_si(precision,
                                                     system->m[id],
                                                     i + 1));

    value = osl_int_get_si(precision, system->m[id], system->nb_columns - 1);
    value = value < 0 ? -value : value;
    if ((gcd == 0 && value != 0) || value % gcd)
      return 0;
  }

  return 1;
}


/**
 * candl_dependence_build_system function:
 * this function builds the constraint system corresponding to a data
 * dependence, for a given statement couple (with iteration domains "source"
 * and "target"), for a given reference couple (the source reference is array
 * "ref_s" in "array_s" and the target reference is the array "ref_t" in
 * "array_t"), at a given depth "depth" and knowing if the source is textually
 * before the target (boolean "before"). The system is built... as always !
 * See the [bastoul and Feautrier, PPL 2005] paper for details !
 * - source is the source iteration domain,
 * - target is the target iteration domain,
 * - array_s is the access array for the source,
 * - array_t is the access array for the target,
 * - depth is the dependence depth,
 * - before is 1 if the source is textually before the target, 0 otherwise,
 ***
 * - 13/12/2005: first version (extracted from candl_dependence_system).
 * - 23/02/2006: a stupid bug corrected in the subscript equality.
 * - 07/04/2007: fix the precedence condition to respect C. Bastoul PhD thesis
 */
static candl_dependence_p candl_dependence_build_system(
                            osl_statement_p source, osl_statement_p target,
                            osl_relation_p array_s, osl_relation_p array_t,
                            int depth, int before) {
  candl_dependence_p dependence;
  osl_relation_p system;
  candl_statement_usr_p s_usr = source->usr;
  candl_statement_usr_p t_usr = target->usr;
  int i, j, k, c;
  int constraint = 0;
  int precision = source->domain->precision;
  int nb_output_dims; // source column
  int nb_input_dims;  // target column
  int nb_local_dims;
  int nb_par;
  int nb_rows, nb_columns;
  int ind_source_local_domain;
  int ind_source_local_access;
  int ind_target_local_domain;
  int ind_target_local_access;
  int ind_params;
  int min_depth = 0;
  
  /* Create a new dependence structure */
  dependence = candl_dependence_malloc();
  
  dependence->source_nb_output_dims_domain = source->domain->nb_output_dims;
  dependence->source_nb_output_dims_access = array_s->nb_output_dims;
  
  dependence->target_nb_output_dims_domain = target->domain->nb_output_dims;
  dependence->target_nb_output_dims_access = array_t->nb_output_dims;
  
  dependence->source_nb_local_dims_domain  = source->domain->nb_local_dims;
  dependence->source_nb_local_dims_access  = array_s->nb_local_dims;
  dependence->target_nb_local_dims_domain  = target->domain->nb_local_dims;
  dependence->target_nb_local_dims_access  = array_t->nb_local_dims;
  
  dependence->next = NULL;
  
  /* Compute the maximal common depth. */
  while (min_depth < s_usr->depth && min_depth < t_usr->depth &&
         s_usr->index[min_depth] == t_usr->index[min_depth])
    ++min_depth;
  
  /* Compute the system size */
  nb_par         = source->domain->nb_parameters;
  nb_local_dims  = dependence->source_nb_local_dims_domain +
                   dependence->source_nb_local_dims_access +
                   dependence->target_nb_local_dims_domain +
                   dependence->target_nb_local_dims_access;
  nb_output_dims = dependence->source_nb_output_dims_domain +
                   dependence->source_nb_output_dims_access;
  nb_input_dims  = dependence->target_nb_output_dims_domain +
                   dependence->target_nb_output_dims_access;

  nb_columns = nb_output_dims + nb_input_dims + nb_local_dims + nb_par + 2;
  nb_rows    = source->domain->nb_rows + target->domain->nb_rows +
               array_s->nb_rows + array_t->nb_rows +
               min_depth + 1 + // equality between the output access
               depth;

  system = osl_relation_pmalloc(precision, nb_rows, nb_columns);
  
  /* Compute some indexes */
  ind_source_local_domain = 1 + nb_output_dims + nb_input_dims;
  ind_source_local_access = ind_source_local_domain + dependence->source_nb_local_dims_domain;
  ind_target_local_domain = ind_source_local_access + dependence->target_nb_local_dims_access;
  ind_target_local_access = ind_target_local_domain + dependence->target_nb_local_dims_domain;
  ind_params              = ind_target_local_access + dependence->target_nb_local_dims_access;
  
  /* 1. Copy the source domain */
  for (i = 0 ; i < source->domain->nb_rows ; i++) {
    /* eq/in */
    osl_int_assign(precision,
                   system->m[constraint], 0,
                   source->domain->m[i], 0);
    /* output dims */
    k = 1;
    j = 1;
    for (c = source->domain->nb_output_dims ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     source->domain->m[i], j);
    /* local dims (no input in domain, so j is the same) */
    k = ind_source_local_domain;
    for (c = source->domain->nb_local_dims ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     source->domain->m[i], j);
    /* params + const */
    k = ind_params;
    for (c = nb_par+1 ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     source->domain->m[i], j);
    constraint++;
  }
  
  /* 2. Copy the target domain */
  for (i = 0 ; i < target->domain->nb_rows ; i++) {
    /* eq/in */
    osl_int_assign(precision,
                   system->m[constraint], 0,
                   target->domain->m[i], 0);
    /* output dims */
    k = 1 + nb_output_dims;
    j = 1;
    for (c = target->domain->nb_output_dims ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     target->domain->m[i], j);
    /* local dims (no input in domain, so j is the same) */
    k = ind_target_local_domain;
    for (c = target->domain->nb_local_dims ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     target->domain->m[i], j);
    /* params + const */
    k = ind_params;
    for (c = nb_par+1 ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     target->domain->m[i], j);
    constraint++;
  }
  
  /* 3. Copy the source access */
  for (i = 0 ; i < array_s->nb_rows ; i++) {
    /* eq/in */
    osl_int_assign(precision,
                   system->m[constraint], 0,
                   array_s->m[i], 0);
    /* output dims */
    k = 1 + source->domain->nb_output_dims;
    j = 1;
    for (c = array_s->nb_output_dims ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_s->m[i], j);
    /* link input dims access to the output dims domain */
    k = 1;
    for (c = array_s->nb_input_dims ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_s->m[i], j);
    /* local dims */
    k = ind_source_local_access;
    for (c = array_s->nb_local_dims ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_s->m[i], j);
    /* params + const */
    k = ind_params;
    for (c = nb_par+1 ; c > 0 ; c--, k++, j++)
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_s->m[i], j);
    
    constraint++;
  }
  
  /* 4. Copy the target access */
  for (i = 0 ; i < array_t->nb_rows ; i++) {
    /* eq/in */
    osl_int_assign(precision,
                   system->m[constraint], 0,
                   array_t->m[i], 0);
    /* output dims */
    k = 1 + nb_output_dims + target->domain->nb_output_dims;
    j = 1;
    for (c = array_t->nb_output_dims ; c > 0 ; c--, k++, j++) {
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_t->m[i], j);
      osl_int_oppose(precision,
                     system->m[constraint], k,
                     system->m[constraint], k);
    }
    /* link input dims access to the output dims domain */
    k = 1 + nb_output_dims;
    for (c = array_t->nb_input_dims ; c > 0 ; c--, k++, j++) {
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_t->m[i], j);
      osl_int_oppose(precision,
                     system->m[constraint], k,
                     system->m[constraint], k);
    }
    /* local dims */
    k = ind_target_local_access;
    for (c = array_t->nb_local_dims ; c > 0 ; c--, k++, j++) {
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_t->m[i], j);
      osl_int_oppose(precision,
                     system->m[constraint], k,
                     system->m[constraint], k);
    }
    /* params + const */
    k = ind_params;
    for (c = nb_par+1 ; c > 0 ; c--, k++, j++) {
      osl_int_assign(precision,
                     system->m[constraint], k,
                     array_t->m[i], j);
      osl_int_oppose(precision,
                     system->m[constraint], k,
                     system->m[constraint], k);
    }
    constraint++;
  }
  
  
  /* 5. Set the equality between the output access */
  /* Note here that the equality between the 2 Arr are necessarily equal */
  k = 1 + source->domain->nb_output_dims;
  j = 1 + nb_output_dims + target->domain->nb_output_dims;
  for (i = 0 ; i < min_depth+1 ; i++, k++, j++) { /* plus 1 for the Arr */
    osl_int_set_si(precision, system->m[constraint], k, -1);
    osl_int_set_si(precision, system->m[constraint], j, 1);
    constraint++;
  }
  
  /* 6. The precedence constraints */
  k = 1;
  j = 1 + nb_output_dims;
  for (i = 0; i < depth; i++, k++, j++) {
    /* i = i' for all dimension less than depth. */
    osl_int_set_si(precision, system->m[constraint], k, -1);
    osl_int_set_si(precision, system->m[constraint], j, 1);
    if (i == depth - 1) {
      /* i <= i' at dimension depth if source is textually before target. */
      osl_int_set_si(precision, system->m[constraint], 0, 1);
      /* If source is textually after target, this is obviously i < i'. */
      if (before || depth < min_depth)
        osl_int_set_si(precision, system->m[constraint], nb_columns - 1, -1);
    }

    constraint++;
  }
  
  system->nb_output_dims = nb_output_dims;
  system->nb_input_dims = nb_input_dims;
  system->nb_parameters = nb_par;
  system->nb_local_dims = nb_local_dims;
  system->type = OSL_TYPE_DOMAIN;
  
  dependence->domain = system;
  
  return dependence; 
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
candl_dependence_p candl_dependence_system(osl_statement_p source,
                                           osl_statement_p target,
                                           osl_relation_p context,
                                           osl_relation_p array_s,
                                           osl_relation_p array_t,
                                           int ref_s, int ref_t,
                                           int type, int depth) {
  candl_statement_usr_p s_usr = source->usr;
  candl_statement_usr_p t_usr = target->usr;
  candl_dependence_p dependence = NULL;

  /* First, a trivial case: for two different statements at depth 0, there is
   * a dependence only if the source is textually before the target.
   */
  if ((source != target) && (depth == 0) && 
      (s_usr->label > t_usr->label))
    return NULL;
  
  /* We build the system of constraints. */
  dependence = candl_dependence_build_system(source,  target,
                                             array_s, array_t, 
                                             depth,
                                             (s_usr->label >= 
                                              t_usr->label));

  /* We start by simple SIV/ZIV/GCD tests. */
  if (!candl_dependence_gcd_test(source, target, dependence->domain, depth)) {
    candl_dependence_free(dependence);
    return NULL;
  }

  if (pip_has_rational_point(dependence->domain, context, 1)) {
    /* We set the various fields with corresponding values. */
    dependence->type       = type;
    dependence->depth      = depth;
    dependence->source     = source;
    dependence->target     = target;
    dependence->ref_source = ref_s;
    dependence->ref_target = ref_t;
  } else {
    candl_dependence_free(dependence);
    dependence = NULL;
  }

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
candl_dependence_p candl_dependence_between(osl_statement_p source,
                                            osl_statement_p target,
                                            osl_relation_p context,
                                            candl_options_p options) {
  candl_dependence_p new;
  candl_dependence_p dependence = NULL;
  candl_dependence_p now;
  osl_relation_list_p access_src, access_targ;
  osl_relation_p elt_src, elt_targ;
  candl_statement_usr_p s_usr = source->usr;
  candl_statement_usr_p t_usr = target->usr;
  int i, min_depth, max_depth;
  int precision = source->scattering->precision;
  int row_src, row_targ;
  int ref_s, ref_t;
  
  /* If the statements commute and the user asks to use this information to
   * simplify the dependence graph, we return no dependences.
   */
  // TODO
  //if (options->commute && candl_statement_commute(source, target))
  //  return NULL;

  /* In the case of a self-dependence, the dependence depth can be as low as 1
   * (not 0 because at depth 0 there is no loop, thus there is only one
   * instance of the statement !) and as high as the statement depth.
   * In the case of different statements, the dependence depth can be as low
   * as 0 and as high as the number of shared loops.
   */
  if (source == target) {
    min_depth = 1;
    max_depth = s_usr->depth;
  } else {
    /* Depth 0 is for statements that don't share any loop. */
    if (s_usr->depth > 0 && t_usr->depth > 0)
      min_depth = (s_usr->index[0] == t_usr->index[0]) ? 1 : 0;
    else
      min_depth = 0;

    max_depth = 0;
    while ((max_depth < s_usr->depth) &&
           (max_depth < t_usr->depth) &&
           (s_usr->index[max_depth] == t_usr->index[max_depth]))
      max_depth++;
  }
  
  ref_s = 0;
  access_src = source->access;
  for (; access_src != NULL; access_src = access_src->next, ref_s++) {
    elt_src = access_src->elt;
    
    row_src = clay_relation_get_line(elt_src, 0);
    
    switch(elt_src->type) {
    
      /* Anti and input-dependences analysis. */
      case OSL_TYPE_READ: /* source READ */
        if (!options->war && !options->rar)
          break;
        access_targ = target->access;
        ref_t = 0;
        for (; access_targ != NULL; access_targ = access_targ->next, ref_t++) {
          elt_targ = access_targ->elt;
          row_targ = clay_relation_get_line(elt_targ, 0);
          
          /* Anti-dependences analysis. */
          if (elt_targ->type != OSL_TYPE_READ) { /* target WRITE | MAY_WRITE */
            if (options->war && 
                osl_int_eq(precision,
                           elt_src->m[row_src],   elt_src->nb_columns-1,
                           elt_targ->m[row_targ], elt_targ->nb_columns-1)) {
              for (i = min_depth; i <= max_depth; i++) {
                new = candl_dependence_system(source, target, context,
                                              elt_src, elt_targ,
                                              ref_s, ref_t,
                                              CANDL_WAR, i);
                candl_dependence_add(&dependence, &now, new);
              }
            }
          }
          /* Input-dependences analysis. */
          else { /* target READ */
            if (options->rar && 
                osl_int_eq(precision,
                           elt_src->m[row_src],   elt_src->nb_columns-1,
                           elt_targ->m[row_targ], elt_targ->nb_columns-1)) {
              for (i = min_depth; i <= max_depth; i++) {
                new = candl_dependence_system(source, target, context,
                                              elt_src, elt_targ,
                                              ref_s, ref_t,
                                              CANDL_RAR, i);
                candl_dependence_add(&dependence, &now, new);
              }
            }
          }
        }
        break;
     
      default: /* source WRITE | MAY-WRITE */
        if (!options->raw && !options->waw)
          break;
        access_targ = target->access;
        ref_t = 0;
        for (; access_targ != NULL; access_targ = access_targ->next, ref_t++) {
          elt_targ = access_targ->elt;
          row_targ = clay_relation_get_line(elt_targ, 0);
          
          /* Anti-dependences analysis. */
          if (elt_targ->type != OSL_TYPE_READ) { /* target WRITE | MAY_WRITE */
            if (options->waw && 
                osl_int_eq(precision,
                           elt_src->m[row_src],   elt_src->nb_columns-1,
                           elt_targ->m[row_targ], elt_targ->nb_columns-1)) {
              for (i = min_depth; i <= max_depth; i++) {
                new = candl_dependence_system(source, target, context,
                                              elt_src, elt_targ,
                                              ref_s, ref_t,
                                              CANDL_WAW, i);
                candl_dependence_add(&dependence, &now, new);
              }
            }
          }
          /* Input-dependences analysis. */
          else { /* target READ */
            if (options->raw && 
                osl_int_eq(precision,
                           elt_src->m[row_src],   elt_src->nb_columns-1,
                           elt_targ->m[row_targ], elt_targ->nb_columns-1)) {
              for (i = min_depth; i <= max_depth; i++) {
                new = candl_dependence_system(source, target, context,
                                              elt_src, elt_targ,
                                              ref_s, ref_t,
                                              CANDL_RAW, i);
                candl_dependence_add(&dependence, &now, new);
              }
            }
          }
        }
        break;
    }
  }
  
  return dependence;
}


/**
 * candl_dependence function:
 * this function builds the dependence graph of a scop
 * according to some user options (options).
 * - 18/09/2003: first version.
 */
candl_dependence_p candl_dependence(osl_scop_p scop, candl_options_p options) {
  candl_dependence_p dependence = NULL;
  candl_dependence_p new = NULL;
  candl_dependence_p now;
  osl_statement_p stmt_i, stmt_j;
  osl_relation_p context = scop->context;
  
  if (options->scalar_privatization || options->scalar_expansion)
    candl_dependence_analyze_scalars(scop, options);

  stmt_i = scop->statement;
  for (; stmt_i != NULL; stmt_i = stmt_i->next) {
    /* We add self dependence. */
    /* S->S */
    new = candl_dependence_between(stmt_i, stmt_i, context, options);
    candl_dependence_add(&dependence, &now, new);
    
    stmt_j = stmt_i->next;
    for (; stmt_j != NULL; stmt_j = stmt_j ->next) {
      /* And dependences with other statements. */
      /* S1->S2 */
      new = candl_dependence_between(stmt_i, stmt_j, context, options);
      candl_dependence_add(&dependence, &now, new);

      /* S2->S1 */
      new = candl_dependence_between(stmt_j, stmt_i, context, options);
      candl_dependence_add(&dependence, &now, new);
    }
  }
  
  /* If scalar analysis is called, remove some useless dependences. */
  /* LNP: This is subsubmed by the updated prune-with-privatization function. */
   if (options->scalar_privatization || options->scalar_expansion || 
       options->scalar_renaming) 
     candl_dependence_prune_scalar_waw(scop, options, &dependence); 

  /* Final treatment for scalar analysis. */
  int check = 0;
  if (options->scalar_renaming)
    check = candl_dependence_scalar_renaming(scop, options, &dependence);

  if (! check && options->scalar_privatization)
    candl_dependence_prune_with_privatization(scop, options, &dependence);

  /* Compute the last writer */
  if (options->lastwriter)  {
    candl_compute_last_writer(dependence, scop);
  }

  #if defined(CANDL_COMPILE_PRUNNING_C)
  /* Remove some transitively covered dependences (experimental). */
  if (options->prune_dups)
    dependence = candl_dependence_prune_transitively_covered(dependence);
  #endif

  return dependence;
}


/******************************************************************************
 *                          Scalar analysis functions                         *
 ******************************************************************************/

/**
 * candl_dependence_var_is_scalar function:
 * This function returns true if the variable indexed by 'var_index'
 * is a scalar in the whole scop.
 */
int candl_dependence_var_is_scalar(osl_scop_p scop, int var_index) {
  osl_statement_p statement;
  osl_relation_list_p access;
  osl_relation_p elt;
  int precision = scop->context->precision;
  int i;
  int row;

  statement = scop->statement;
  while (statement != NULL) {
    access = statement->access;
    while (access != NULL) {
      elt = access->elt;
      row = clay_relation_get_line(elt, 0);
      if (osl_int_get_si(precision, elt->m[row], elt->nb_columns-1) ==
          var_index) {
        /* Ensure it is not an array. */
        if (elt->nb_output_dims > 1)
          return 0;
        /* Ensure the access function is '0'. */
        if (!osl_int_zero(precision, elt->m[row], 0))
          return 0;
        for (i = 2; i < elt->nb_columns-2; ++i) /* jmp the 'Arr' */
          if (!osl_int_zero(precision, elt->m[row], i))
            return 0;
      }
      access = access->next;
    }
    statement = statement->next;
  }
  
  return 1;
}


/**
 * candl_dependence_expand_scalar function:
 * Expand the variable of index 'scalar_idx' by adding one
 * dimension (x becomes x[0]) to each access matrix refering this
 * variable in the statement list.
 *
 */
static void candl_dependence_expand_scalar(osl_statement_p* sl,
                                           int scalar_idx) {
  osl_relation_list_p access;
  osl_relation_p elt;
  int tmp;
  int row;
  int precision = sl[0]->scattering->precision;
  int i;

  /* Iterate on all statements of the list. */
  for (i = 0; sl[i] != NULL; ++i) {
    
    /* Check if the scalar is referenced in the 'read' access
       function. */
    access = sl[i]->access;
    for (; access != NULL ; access = access->next) {
      elt = access->elt;
      if (elt->type == OSL_TYPE_READ) {
        row = clay_relation_get_line(elt, 0);
        tmp = osl_int_get_si(precision,
                             elt->m[row],
                             elt->nb_columns-1);
        if (tmp == scalar_idx)
          break;
      }
    }
    /* It is. */
    if (access != NULL) {
      /* Add a row to the 'read' matrix, just after the reference
         to 'scalar_idx'. */
      osl_relation_insert_blank_row(elt, elt->nb_rows);
    }

    /* Same for 'written' access function. */
    access = sl[i]->access;
    for (; access != NULL ; access = access->next) {
      elt = access->elt;
      if (elt->type != OSL_TYPE_READ) { /* WRITE | MAY_WRITE */ 
        row = clay_relation_get_line(elt, 0);
        tmp = osl_int_get_si(precision,
                             elt->m[row],
                             elt->nb_columns-1);
        if (tmp == scalar_idx)
          break;
      }
    }
    /* It is. */
    if (access != NULL) {
      /* Add a row to the 'read' matrix, just after the reference
         to 'scalar_idx'. */
      osl_relation_insert_blank_row(elt, elt->nb_rows);
    }
  }
}


/**
 * candl_dependence_refvar_chain function:
 * This function returns a chain of statements as a feshly allocated
 * array of pointer on statements, of all statements reading or
 * writing the variable 'var_index', surrounded by the 'level' common
 * loops of 'dom'. 
 * Output is a NULL-terminated array. We don't create a chained list,
 * because it demands to clone every statements each time, and we need
 * to clone the field usr too.
 */
osl_statement_p* 
candl_dependence_refvar_chain(osl_scop_p scop, osl_statement_p dom,
                              int var_index, int level) {
  /* No or empty scop -> no chain! */
  if (scop == NULL || scop->statement == NULL)
    return NULL;

  osl_statement_p* res; /* not a chained list, but an array of statement */
  osl_statement_p statement;
  osl_relation_p scattering, scattering_dom;
  candl_statement_usr_p dom_usr;
  candl_statement_usr_p stmt_usr;
  int i;
  int buffer_size = 64;
  int count = 0;

  /* If no dominator is provided, assume we start with the first statement. */
  if (dom == NULL)
    dom = scop->statement;
  
  dom_usr = dom->usr;
  
  statement = scop->statement;
  while (statement != NULL && statement != dom)
    statement = statement->next;
  
  /* The dominator is not in the list of statements. */
  if (statement == NULL)
    return NULL;
  
  res = (osl_statement_p*) malloc(sizeof(osl_statement_p) * buffer_size);
  scattering_dom = dom->scattering;
  
  for (; statement != NULL; statement = statement->next) {
    stmt_usr = statement->usr;
    
    /* We look for exactly 'level' common loops. */
    if (stmt_usr->depth < level)
      continue;
      
    /* Ensure it has 'level' common loop(s) with the dominator. */
    scattering = statement->scattering;
    for (i = 0; i < level && 
         stmt_usr->index[i] == dom_usr->index[i];
         ++i)
      ;
         
    if (i < level)
      continue;
   
    /* Ensure the variable is referenced. */
    if (candl_dependence_var_is_ref(statement, var_index) != CANDL_VAR_UNDEF) {
      if (count == buffer_size) {
        buffer_size *= 2;
        res = realloc(res, sizeof(osl_statement_p) * buffer_size);
      }
      res[count++] = statement;
    }
  }
  
  res = realloc(res, sizeof(osl_statement_p) * (count+1));
  res[count] = NULL;

  return res;
}


/**
 * candl_dependence_var_is_ref function:
 * This function checks if a var 'var_index' is referenced (DEF or
 * USE) by the statement.
 */
int candl_dependence_var_is_ref(osl_statement_p s, int var_index) {
  osl_relation_list_p access;
  osl_relation_p elt;
  int row;
  int ret = CANDL_VAR_UNDEF;

  if (s) {
    /* read access */
    access = s->access;
    while (access != NULL) {
      elt = access->elt;
      if (elt->type == OSL_TYPE_READ) {
        row = clay_relation_get_line(elt, 0);
        if (osl_int_get_si(elt->precision, elt->m[row], elt->nb_columns-1) ==
            var_index) {
          ret = CANDL_VAR_IS_USED;
          break;
        }
      }
      access = access->next;
    }
    
    /* right access */
    access = s->access;
    while (access != NULL) {
      elt = access->elt;
      if (elt->type != OSL_TYPE_READ) {
        row = clay_relation_get_line(elt, 0);
        if (osl_int_get_si(elt->precision, elt->m[row], elt->nb_columns-1) ==
            var_index) {
          if (ret == CANDL_VAR_IS_USED)
            ret = CANDL_VAR_IS_USED;
          else
            ret = CANDL_VAR_IS_DEF_USED;
          break;
        }
      }
      access = access->next;
    }
  }

  return ret;
}


/**
 * candl_dependence_compute_lb function:
 * This function assigns to the Entier 'lb' the lexmin of variable
 * 'col'-1 in the polyhedron 'm'.
 */
static void candl_dependence_compute_lb(osl_relation_p m, Entier* lb, int col) {
  PipOptions* options;
  PipQuast* solution;
  PipList* l;
  options = pip_options_init();
  options->Simplify = 1;
  options->Urs_parms = -1;
  options->Urs_unknowns = -1;
  /* Compute lexmin. */
  solution = pip_solve_osl(m, NULL, -1, options);
  
  if ((solution != NULL) &&
      ((solution->list != NULL) || (solution->condition != NULL))) {
    l = solution->list;
    while (col-- > 1)
      l = l->next;
    CANDL_assign(*lb, l->vector->the_vector[0]);
  }
  pip_options_free(options);
  pip_quast_free(solution);
}


/**
 * candl_dependence_check_domain_is_included function:
 * This function checks if the 'level'-first iterators of 2 domains
 * are in such a way that s1 is larger or equal to s2, for the
 * considered iterator dimensions.
 *
 */
int candl_dependence_check_domain_is_included(osl_statement_p s1,
                                              osl_statement_p s2,
                                              osl_relation_p context,
                                              int level) {
  candl_statement_usr_p s1_usr = s1->usr;
  candl_statement_usr_p s2_usr = s2->usr;
  osl_relation_p matrix;
  int max = level;
  int i, j;
  int precision = s2->domain->precision;
  Entier lb;
  CANDL_init(lb);
  
  if (s1_usr->depth < max) max = s1_usr->depth;
  if (s2_usr->depth < max) max = s2_usr->depth;
  
  matrix = osl_relation_pmalloc(precision,
                                s2->domain->nb_rows + s2_usr->depth - max + 1,
                                s2->domain->nb_columns);

  /* Duplicate s2 to the dest matrix. */
  for (i = 0; i < s2->domain->nb_rows; ++i) {
    for (j = 0; j < s2->domain->nb_columns; ++j)
      osl_int_assign(precision,
                     matrix->m[i], j,
                     s2->domain->m[i], j);
  }
  
  /* Make useless dimensions equal to 1. */
  for (j = 0; j < s2_usr->depth - max; ++j) {
    candl_dependence_compute_lb(s2->domain, &lb, j + 1 + max);
    osl_int_assign(precision,
                   matrix->m[i], matrix->nb_columns-1,
                   (void*) &lb, 0);
    osl_int_set_si(precision,
                   matrix->m[i++], j+1+max,
                   -1);
  }
  
  /* Iterate on all constraints of s1, and check them. */
  for (i = 0; i < s1->domain->nb_rows; ++i) {
    /* Skip constraints defining other iterators. */
    for (j = max + 1; j <= s1_usr->depth; ++j) {
      if (!osl_int_zero(precision, s1->domain->m[i], j))
        break;
    }
    if (j <= s1_usr->depth)
      continue;
    /* Invert the constraint, and add it to matrix. */
    for (j = 0; j <= max; ++j) {
      osl_int_assign(precision,
                     matrix->m[matrix->nb_rows-1], j,
                     s1->domain->m[i], j);
      osl_int_oppose(precision,
                     matrix->m[matrix->nb_rows-1], j,
                     matrix->m[matrix->nb_rows-1], j);
    }
    for (j = s1_usr->depth + 1; j < s1->domain->nb_columns; ++j) {
      osl_int_assign(precision,
                     matrix->m[matrix->nb_rows-1],
                     j - s1_usr->depth + s2_usr->depth,
                     s1->domain->m[i], j);
      osl_int_oppose(precision,
                     matrix->m[matrix->nb_rows-1],
                     j - s1_usr->depth + s2_usr->depth,
                     matrix->m[matrix->nb_rows-1],
                     j - s1_usr->depth + s2_usr->depth);
    }
    /* Make the inequality strict. */
    osl_int_decrement(precision,
                      matrix->m[matrix->nb_rows-1], matrix->nb_columns-1,
                      matrix->m[matrix->nb_rows-1], matrix->nb_columns-1);

    if (candl_matrix_check_point(matrix, context)) {
      /* There is a point. dom(s1) - dom(s2) > 0. */
      CANDL_clear(lb);
      osl_relation_free(matrix);
      return 0;
    }
  }

  CANDL_clear(lb);
  osl_relation_free(matrix);

  return 1;
}


/**
 * candl_dependence_extract_scalar_variables function:
 * This functions returns a -1-terminated array of the scop scalar
 * variables.
 */
int* candl_dependence_extract_scalar_variables(osl_scop_p scop) {
  osl_statement_p statement;
  osl_relation_p elt;
  osl_relation_list_p access;
  int scalars[1024]; /* FIXME: implement a real buffer. */
  int checked[1024];
  int i, idx;
  int row;
  int count_s = 0, count_c = 0;
  
  /* Detect all scalar variables. */
  statement = scop->statement;
  while (statement != NULL) {
    access = statement->access;
    while (access != NULL) {
      elt = access->elt;
      row = clay_relation_get_line(elt, 0);
      idx = osl_int_get_si(elt->precision,
                           elt->m[row],
                           elt->nb_columns-1);
      
      for (i = 0; i < count_s && scalars[i] != idx; ++i)
       ;
      if (i == count_s) {
        for (i = 0; i < count_c && checked[i] != idx; ++i)
         ;
        if (i == count_c) {
          if (candl_dependence_var_is_scalar(scop, idx))
            scalars[count_s++] = idx;
          else
            checked[count_c++] = idx;
        }
        if (count_s == 1024 || count_c == 1024)
          CANDL_FAIL("Error: Buffer size too small");
      }
      access = access->next;
    }
    statement = statement->next;
  }
  
  /* Rearrange the array to the exact size. */
  int* res = (int*) malloc((count_s + 1) * sizeof(int));
  for (i = 0; i < count_s; ++i)
    res[i] = scalars[i];
  res[i] = -1;

  return res;
}


/**
 * candl_dependence_prune_scalar_waw function:
 * This function removes all WAW dependences between the same scalar
 * (they are useless dependences).
 */
void candl_dependence_prune_scalar_waw(osl_scop_p scop,
                                       candl_options_p options,
                                       candl_dependence_p* deps) {
  int* scalars;
  int i;
  int refs, reft;
  candl_dependence_p tmp;
  candl_dependence_p next;
  candl_dependence_p pred = NULL;

  if (options->verbose)
    fprintf (stderr, "[Candl] Scalar Analysis: Remove all WAW between the same"
             " scalar\n");

  scalars = candl_dependence_extract_scalar_variables(scop);

  for (tmp = *deps; tmp; ) {
    candl_dependence_get_array_refs_in_dep(tmp, &refs, &reft);
    if (refs == reft && tmp->type == CANDL_WAW) {
      for (i = 0; scalars[i] != -1 && scalars[i] != refs; ++i)
       ;
      if (scalars[i] != -1) {
        osl_relation_free(tmp->domain);
        next = tmp->next;
        if (pred == NULL)
          *deps = next;
        else
          pred->next = next;
        free(tmp);
        tmp = next;
        continue;
      }
    }
    pred = tmp;
    tmp = tmp->next;
  }

  free(scalars);
}


/**
 * candl_dependence_scalar_renaming function:
 * This function renames scalars in the program. In case scalars have
 * been renamed, the dependence analysis is re-run.
 */
int candl_dependence_scalar_renaming(osl_scop_p scop,
                                     candl_options_p options,
                                     candl_dependence_p* deps) {
  osl_statement_p *statement; /* not a chained list */
  osl_statement_p iter;
  osl_statement_p defs[1024];
  osl_statement_p uses[1024];
  osl_statement_p last_def;
  osl_statement_p *current; /* not a chained list */
  osl_relation_p elt;
  osl_relation_list_p access;
  candl_statement_usr_p usr;
  candl_statement_usr_p last_usr;
  int i, j, k;
  int nb_statements = 0;
  int *parts;
  int defs_c, uses_c;
  int *scalars;
  int precision = scop->context->precision;
  int row;
  int tmp, has_changed = 0;
  int newvar = 0;

  if (options->verbose)
    fprintf (stderr, "[Candl] Scalar Analysis: Perform scalar renaming\n");
  
  /* Compute the first free variable index seed. */
  for (iter = scop->statement; iter != NULL; iter = iter->next) {
    access = iter->access;
    for (; access != NULL; access = access->next) {
      elt = access->elt;
      row = clay_relation_get_line(elt, 0);
      tmp = osl_int_get_si(precision,
                           elt->m[row],
                           elt->nb_columns-1);
      if (tmp >= newvar)
        newvar = tmp + 1;
    }
    nb_statements++;
  }

  /* Init */
  current = (osl_statement_p*) malloc(sizeof(osl_statement_p) * nb_statements);
  parts = (int*) malloc(sizeof(int) * nb_statements);
  
  /* Get the list of scalars. */
  scalars = candl_dependence_extract_scalar_variables(scop);

  /* Iterate on all scalars. */
  for (i = 0; scalars[i] != -1; ++i) {
    /* Get all statements referencing the scalar. */
    statement = candl_dependence_refvar_chain(scop, NULL, scalars[i], 0);

    /* If the chain starts by a USE, we can't do anything. */
    if (statement[0] == NULL ||
        candl_dependence_var_is_ref(statement[0], scalars[i])
        != CANDL_VAR_IS_DEF) {
      free(statement);
      continue;
    }

    /* Get all defs and all uses. */
    defs_c = 0;
    uses_c = 0;
    for (j = 0; statement[j]; ++j) {
      tmp = candl_dependence_var_is_ref(statement[j], scalars[i]);
      switch (tmp) {
        case CANDL_VAR_IS_USED:
        case CANDL_VAR_IS_DEF_USED:
          uses[uses_c++] = statement[j];
          break;
        case CANDL_VAR_IS_DEF:
          defs[defs_c++] = statement[j];
          break;
      }
    }

    /* Clean buffer. */
    j = 0;
    for (iter = scop->statement; iter != NULL; iter = iter->next)
      current[j++] = NULL;

    free(statement);

    /* Iterate on all DEFs. */
    last_def = NULL;
    for (j = 0; j < defs_c; ++j) {
      if (last_def == NULL) {
        last_def = defs[j];
      } else {
        /* Ensure the current DEF covers all iterations covered
           by the last checked one. */
        last_usr = last_def->usr;
        usr = defs[j]->usr;
        for (k = 0; k < last_usr->depth && k < usr->depth &&
             last_usr->index[k] == usr->index[k];
             ++k)
          ;
        /* We only need to check when there are common loops. */
        if (k && ! candl_dependence_check_domain_is_included
            (last_def, defs[j], scop->context, k + 1)) {
          usr = defs[j]->usr;
          current[usr->label] = last_def;
          continue;
        } else {
          last_def = defs[j];
        }
      }
      /* Create DEF-USE table. */
      for (k = 0; k < uses_c; ++k) {
        usr = uses[k]->usr;
        if (usr->label > ((candl_statement_usr_p)defs[j]->usr)->label)
          current[usr->label] = defs[j];
      }
    }

    /* Initialize partition table. */
    for (j = 0; j < nb_statements; ++j)
      parts[j] = -1;

    /* Create partitions. */
    for (j = 0; j < defs_c; ++j) {
      usr = defs[j]->usr;
      for (k = 0; k < nb_statements; ++k)
        if ((current[k] && current[k] == defs[j]) ||
            (k == usr->label && current[usr->label] == NULL))
          parts[k] = j;
    }

    /* Check if it is needed to rename the scalar. */
    tmp = -1;
    for (j = 0; j < nb_statements; ++j)
      if (tmp == -1) {
        if (parts[j] != -1)
          tmp = parts[j];
      } else {
        if (parts[j] != -1 && tmp != parts[j])
          break;
      }

    /* Rename scalar variable. */
    if (j != nb_statements) {
      tmp = -1;
      j = 0;
      for (iter = scop->statement ; iter != NULL ; iter = iter->next) {
        if (parts[j] != -1) {
          if (tmp == -1)
            tmp = parts[j];
          else if (tmp != parts[j])
            has_changed = 1;
          
          access = iter->access;
          for (; access != NULL; access = access->next) {
            elt = access->elt;
            row = clay_relation_get_line(elt, 0);
            tmp = osl_int_get_si(precision,
                                 elt->m[row],
                                 elt->nb_columns-1);
            if (tmp == scalars[i]) {
              if (options->verbose)
                fprintf (stderr, "[Candl] Scalar analysis: Renamed "
                         "variable %d to %d at statement S%d\n",
                         scalars[i], newvar + parts[j], j);
              osl_int_set_si(precision,
                             elt->m[row],
                             elt->nb_columns-1,
                             newvar + parts[j]);
            }
          }
        }
        j++;
      }
    }
  } /* end Iterate on all scalars */

  /* Redo the full dependence analysis, if needed. */
  if (has_changed) {
    int bopt = options->scalar_renaming;
    options->scalar_renaming = 0;
    if (options->scalar_privatization)
      free(((candl_scop_usr_p)scop->usr)->scalars_privatizable);
    candl_dependence_free(*deps);
    *deps = candl_dependence(scop, options);
    options->scalar_renaming = bopt;
  }
  
  free(scalars);
  free(current);
  free(parts);

  return has_changed;
}


/**
 * candl_dependence_is_loop_carried function:
 * This function returns true if the dependence 'dep' is loop-carried
 * for loop 'loop_index', false otherwise.
 */
int candl_dependence_is_loop_carried(osl_scop_p scop,
                                     candl_dependence_p dep,
                                     int loop_index) {
  osl_relation_p msrc = NULL, mtarg = NULL;
  candl_statement_usr_p s_usr = dep->source->usr;
  candl_statement_usr_p t_usr = dep->target->usr;
  int i, j;
  int precision = scop->context->precision;
  
  /* Ensure source and sink share common loop 'loop_index', and that
     dependence depth is consistent with the considered loop. */
  for (i = 0; i < s_usr->depth; ++i)
    if (s_usr->index[i] == loop_index)
      break;
  if (i != dep->depth - 1 || i >= t_usr->depth)
    return 0;
  for (j = 0; j < t_usr->depth; ++j)
    if (t_usr->index[j] == loop_index)
      break;
  if (j != i)
    return 0;
  
  candl_dependence_get_relation_refs_in_dep(dep, &msrc, &mtarg);
  
  /* plus one for i and j for the Arr output dim */
  int src_ref_iter = (clay_relation_get_line(msrc, i+1) != -1);
  int dst_ref_iter = (clay_relation_get_line(mtarg,j+1) != -1);

  /* one for the inequ column and one for the Arr */
  i += 2;
  j += 2;

  /* Ensure it is not a basic loop-independent dependence (pure
     equality of the access functions for the surrounding iterators +
     parameters + constant, no occurence of the inner loop iterators,
     and contain the current loop iterator. */
     
     
     
     
  // TODO 
     
     
  /*   
     
     
  int must_test = 0;
  k = 0;
  do {
    // Ensure the reference do reference the current loop iterator
    // to be tested.
    if (!osl_int_zero(precision, msrc->m[k], i)) {
      int kp = 0;
      do {
        if (!osl_int_zero(precision, mtarg->m[kp], j)) {
          // Ensure the access functions are equal for the
          // surrounding loop iterators, and no inner iterator
          // is referenced.
          for (l = 0; l <= i; ++l)
            if (!osl_int_eq(precision,
                            msrc->m[k], l,
                            mtarg->m[kp], l)) {
              must_test = 1;
              break;
            }
          int m = l;
          if (!must_test)
            for (; l <= s_usr->depth+1; ++l)
              if (!osl_int_zero(precision, msrc->m[k], l)) {
                must_test = 1;
                break;
              }
          if (!must_test)
            for (; m <= t_usr->depth+1; ++m)
              if (!osl_int_zero(precision, mtarg->m[kp], m)) {
                must_test = 1;
                break;
              }
          if (!must_test)
            for (; l < msrc->nb_columns-1; ++l, ++m)
            if (!osl_int_eq(precision,
                            msrc->m[k], l,
                            mtarg->m[kp], l)) {
              if (CANDL_get_si(mats->p[dep->ref_source + k][l]) !=
                  CANDL_get_si(matt->p[dep->ref_target + kp][m])) {
                must_test = 1;
                break;
              }
        }
        ++kp;
      } while (!must_test &&
               dep->ref_target + kp < matt->NbRows &&
               CANDL_get_si(matt->p[dep->ref_target + kp][0]) == 0);
    }
    ++k;
  } while (!must_test &&
           dep->ref_source + k < mats->NbRows &&
           CANDL_get_si(mats->p[dep->ref_source + k][0]) == 0);
  
  if (src_ref_iter && dst_ref_iter && !must_test)
    return 0;
*/

  /* Final check. For loop i, the dependence is loop carried if there exists
     x_i^R != x_i^S in the dependence polyhedron, with
     x_{1..i-1}^R = x_{1..i-1}^S
   */
/*  int pos;
  osl_relation_p mat = dep->domain;
  osl_statement_p src = dep->source;
  
  osl_relation_p testsyst;
  osl_relation_clone();
      osl_relation_pmalloc(precision, mat->nb_rows + 1 + s_usr->depth,
                           mat->NbColumns);
  for (pos = 0; pos < mat->nb_rows; ++pos)
    for (j = 0; j < mat->nb_columns; ++j)
      CANDL_assign(testsyst->p[pos][j], mat->p[pos][j]);
  for (j = 0; j < i; ++j) {
    CANDL_set_si(testsyst->p[pos + j + 1][0], 0);
    CANDL_set_si(testsyst->p[pos + j + 1][1 + j], -1);
    CANDL_set_si(testsyst->p[pos + j + 1][1 + j + s_usr->depth], 1);
  }

  int has_pt = 0;
  // Test for '>'.
  CANDL_set_si(testsyst->p[pos][0], 1);
  CANDL_set_si(testsyst->p[pos][testsyst->NbColumns - 1], -1);
  CANDL_set_si(testsyst->p[pos][1 + i], 1);
  CANDL_set_si(testsyst->p[pos][1 + i + s_usr->depth], -1);
  has_pt = pip_has_rational_point (testsyst, NULL, 1);
  if (! has_pt) {
    // Test for '<'.
    CANDL_set_si(testsyst->p[pos][1 + i], -1);
    CANDL_set_si(testsyst->p[pos][1 + i + s_usr->depth], 1);
    has_pt = pip_has_rational_point (testsyst, NULL, 1);
  }

  candl_matrix_free (testsyst);

  return has_pt;*/
  return NULL;

  /* LNP: OLD VERSION (not compatible with Openscop !).
  /* The above is more robust. */
  /*   /\* Final check. The dependence exists only because the loop */
  /*      iterates. Make the loop not iterate and check if there's still */
  /*      dependent iterations. *\/ */
  /*   CandlMatrix* m = candl_matrix_malloc(dep->domain->NbRows + 2, */
  /* 				       dep->domain->NbColumns); */
  /*   CANDL_set_si(m->p[m->NbRows - 2][i + 1], -1); */
  /*   CANDL_set_si(m->p[m->NbRows - 1][dep->source->depth + 1 + j], -1); */
  /*   /\* Copy the rest of the matrix. *\/ */
  /*   int ii, jj; */
  /*   for (ii = 0; ii < dep->domain->NbRows; ++ii) */
  /*     for (jj = 0; jj < dep->domain->NbColumns; ++jj) */
  /*       CANDL_assign(m->p[ii][jj], dep->domain->p[ii][jj]); */
  /*   /\* Compute real lb of loops. *\/ */
  /*   Entier lb; CANDL_init(lb); */
  /*   candl_dependence_compute_lb (m, &lb, i + 1); */
  /*   CANDL_assign(m->p[m->NbRows - 2][m->NbColumns - 1], lb); */
  /*   candl_dependence_compute_lb (m, &lb, dep->source->depth + 1 + j); */
  /*   CANDL_assign(m->p[m->NbRows - 1][m->NbColumns - 1], lb); */
  /*   int ret = candl_matrix_check_point(m, program->context); */
  /*   CANDL_clear(lb); */

  /*   /\* Be clean. *\/ */
  /*   candl_matrix_free(m); */

  /*   return !ret; */
}


/**
 * candl_dependence_prune_with_privatization function: This function
 * prunes the dependence graph 'deps' by removing loop-carried
 * dependences involving a scalar variable privatizable for that loop.
 */
void candl_dependence_prune_with_privatization(osl_scop_p scop,
                                               candl_options_p options,
                                               candl_dependence_p* deps) {
  candl_dependence_p next;
  candl_dependence_p tmp;
  candl_dependence_p pred = NULL;
  candl_statement_usr_p s_usr;
  candl_statement_usr_p t_usr;
  candl_scop_usr_p scop_usr = scop->usr;
  int is_priv;
  int i;
  int row;
  int loop_idx = 0;
  int refs, reft;
  int loop_pos_priv;
  int precision = scop->context->precision;

  if (options->verbose)
    fprintf (stderr, "[Candl] Scalar Analysis: Remove loop-carried dependences"
             " on privatizable scalars\n");

  if (scop->statement == NULL)
    return;
  
  /* Perform the scalar analysis, if not done before. */
  if (scop_usr->scalars_privatizable == NULL) {
    candl_options_p options = candl_options_malloc();
    options->scalar_privatization = 1;
    candl_dependence_analyze_scalars(scop, options);
    candl_options_free(options);
  }

  for (tmp = *deps; tmp; ) {
    s_usr = tmp->source->usr;
    t_usr = tmp->target->usr;
    /* Check if the dependence is involving a privatizable scalar. */
    is_priv = 1;
    candl_dependence_get_array_refs_in_dep(tmp, &refs, &reft);
    for (i = 0; i < s_usr->depth; ++i)
      if (candl_dependence_scalar_is_privatizable_at(scop, refs,
                                                     s_usr->index[i]))
        break;
    if (i == s_usr->depth) {
      for (i = 0; i < t_usr->depth; ++i)
        if (candl_dependence_scalar_is_privatizable_at
            (scop, reft, t_usr->index[i]))
          break;
      if (i == t_usr->depth)
        is_priv = 0;
      else
        loop_idx = t_usr->index[i];
    } else {
      loop_idx = s_usr->index[i];
    }
    loop_pos_priv = i;
    
    /* Check if the dependence is loop-carried at loop i. */
    if (is_priv && candl_dependence_is_loop_carried(scop, tmp, loop_idx)) {
      
      /* If so, make the dependence loop-independent. */
      row = tmp->domain->nb_rows;
      osl_relation_insert_blank_row(tmp->domain, row);
      osl_int_set_si(precision,
                     tmp->domain->m[row], 1 + loop_pos_priv,
                     1);
      osl_int_set_si(precision,
                     tmp->domain->m[row], 1 + loop_pos_priv + 
                     s_usr->depth,
                     -1);

      /* Set the type of the dependence as special
         scalar-privatization one. */
      if (tmp->type == CANDL_RAW)
        tmp->type = CANDL_RAW_SCALPRIV;
      next = tmp->next;
      if (!candl_matrix_check_point(tmp->domain, NULL)) {
        /* It is, the dependence can be removed. */
        osl_relation_free(tmp->domain);
        if (pred == NULL)
          *deps = next;
        else
          pred->next = next;
        free(tmp);
      }
      pred = tmp;
      tmp = next;

      continue;
    }
    /* Go to the next victim. */
    pred = tmp;
    tmp = tmp->next;
  }
}


/**
 * candl_dependence_is_privatizable function:
 * This function checks if a given scalar 'var_index' is privatizable
 * for loop 'loop_index'.
 */
int candl_dependence_scalar_is_privatizable_at(osl_scop_p scop,
                                               int var_index,
                                               int loop_index) {
  candl_scop_usr_p scop_usr = scop->usr;
  int i;

  /* If the scalar analysis wasn't performed yet, do it. */
  if (scop_usr->scalars_privatizable == NULL) {
    candl_options_p options = candl_options_malloc();
    options->scalar_privatization = 1;
    candl_dependence_analyze_scalars(scop, options);
    candl_options_free(options);
  }

  /* Check in the array of privatizable scalar variables for the tuple
     (var,loop). */
  for (i = 0; scop_usr->scalars_privatizable[i] != -1; i += 2)
    if (scop_usr->scalars_privatizable[i] == var_index &&
        scop_usr->scalars_privatizable[i + 1] == loop_index)
      return 1;

  return 0;
}


/**
 * candl_dependence_analyze_scalars function:
 * This function checks, for all scalar variables of the scop, and
 * all loop levels, if the scalar can be privatized at that level.
 */
int candl_dependence_analyze_scalars(osl_scop_p scop,
                                     candl_options_p options) {
  int* scalars;
  osl_statement_p* statement; /* not a chained list, but an array of */
  osl_statement_p* fullchain; /* statement to not realloc the usr field */
  osl_statement_p s;
  osl_statement_p curlast;
  osl_statement_p last;
  osl_statement_p iter; /* used to iterate on the scop */
  osl_relation_list_p access;
  osl_relation_p elt;
  candl_scop_usr_p scop_usr = scop->usr;
  candl_statement_usr_p stmt_usr;
  int i, j, k;
  int max, is_priv, offset, was_priv;
  int nb_priv = 0;
  int priv_buff_size = 64;
  int row;

  /* Initialize the list of privatizable scalars to empty. */
  if (options->scalar_privatization) {
    scop_usr->scalars_privatizable = (int*) malloc(2 * sizeof(int));
    scop_usr->scalars_privatizable[0] = -1;
    scop_usr->scalars_privatizable[1] = -1;
  }

  /* Retrieve all scalar variables. */
  scalars = candl_dependence_extract_scalar_variables(scop);

  /* For each of those, detect (at any level) if it can be privatized
     / expanded / renamed. */
  for (i = 0; scalars[i] != -1; ++i) {
    
    /* Go to the first statement referencing the scalar in the scop. */
    for (iter = scop->statement; iter != NULL; iter = iter->next) {
      if (candl_dependence_var_is_ref(iter, scalars[i])
          != CANDL_VAR_UNDEF)
        break;
    }
    
    /* A weird error occured. */
    if (iter == NULL)
      continue;
    
    /* Take all statements referencing the scalar. */
    fullchain = candl_dependence_refvar_chain(scop, iter, scalars[i], 0);

    /* Compute the maximum loop depth of the chain. */
    max = 0;
    for (k = 0; fullchain[k]; ++k) {
      stmt_usr = fullchain[k]->usr;
      if (max < stmt_usr->depth)
        max = stmt_usr->depth;
    }
    last = fullchain[k-1];
    
    /* Initialize the offset for expansion. */
    offset = 0;
    was_priv = 0;
    
    /* Iterate on all possible depth for analysis. */
    for (j = 1; j <= max; ++j) {
      s = fullchain[0];
      
      if (was_priv) {
        ++offset;
        was_priv = 0;
      }
      
      do {
        /* Take all statements dominated by s referencing the
           current scalar variable. */
        statement = candl_dependence_refvar_chain(scop, s, scalars[i], j);
        
        /* No more statement in the chain, exit. */
        if (statement[0] == NULL) {
          free(statement);
          break;
        }
        
        int c = 0;
        
        is_priv = candl_dependence_var_is_ref(statement[0], scalars[i]) ==
                  CANDL_VAR_IS_DEF;
                  
        /* Ensure we have a use in the chain. */
        /* here statement[c] is not NULL */
        for (k = c + 1; statement[k]; ++k) {
          if (candl_dependence_var_is_ref(statement[k], scalars[i]) ==
              CANDL_VAR_IS_USED)
            break;
        }
        
        if (statement[k] == NULL) {
          is_priv = 0;
          
        } else {
          
          /* Check for privatization, while the entry of the chain
             is a DEF. */
          while (statement[c] && candl_dependence_var_is_ref
                 (statement[c], scalars[i]) == CANDL_VAR_IS_DEF) {
            
            /* From the current DEF node, ensure the rest of the
               chain covers not more than the iteration domain
               of the DEF. */
            for (k = c + 1; statement[k]; ++k) {
              /* FIXME: we should deal with
                 def_1->use->def_2->use chains where dom(def_2)
                 > dom(def_1). */
              if (! candl_dependence_check_domain_is_included
                  (statement[c], statement[k], scop->context, j)) {
                /* dom(use) - dom(def) > 0. Check if there is
                   another DEF to test at the entry of the
                   block. */
                if (candl_dependence_var_is_ref
                    (statement[c+1], scalars[i]) != CANDL_VAR_IS_DEF)
                  /* No. The variable is not privatizable. */
                  is_priv = 0;
                break;
              }
            }

            if (! is_priv || ! statement[k])
              break;

            /* The chain dominated by statement is not
               privatizable. Go for the next DEF at the
               beginning of the block, if any. */
            ++c;
          }
          
          if (is_priv) {
            /* Perform the privatization / expansion. */
            if (options->verbose)
              fprintf(stderr, "[Candl] Scalar Analysis: The variable %d"
                       " can be privatized at loop %d\n",
                       scalars[i], 
                       ((candl_statement_usr_p)statement[0]->usr)->index[j-1]);

            if (options->scalar_expansion) {
              int precision = scop->context->precision;
              /* Traverse all statements in the chain. */
              for (k = c; statement[k]; ++k) {
                /* It's not the first expansion of the scalar,
                   we need to increase its dimension all along
                   the program. */
                if (offset && !was_priv)
                  candl_dependence_expand_scalar(fullchain,
                                                 scalars[i]);
                /* Perform scalar expansion in the array
                   access functions. */
                access = statement[k]->access;
                for (; access != NULL; access = access->next) {
                  elt = access->elt;
                  row = clay_relation_get_line(elt, 0);
                  if (scalars[i] ==
                      osl_int_get_si(precision, elt->m[row], elt->nb_columns-1)) {
                    /* plus one to jump the Arr */
                    row = clay_relation_get_line(elt, offset+1);
                    osl_int_set_si(precision, elt->m[row], j+1, 1);
                  }
                }
                was_priv = 1;
              }
            }
            
            if (options->scalar_privatization) {
              /* Memory management for the array of
                 privatizable scalars. */
              if (nb_priv == 0) {
                free(scop_usr->scalars_privatizable);
                scop_usr->scalars_privatizable =
                    (int*)malloc(priv_buff_size * sizeof(int));
                for (k = 0; k < priv_buff_size; ++k)
                  scop_usr->scalars_privatizable[k] = -1;
              }

              if (nb_priv == priv_buff_size) {
                scop_usr->scalars_privatizable =
                    realloc(scop_usr->scalars_privatizable,
                            (priv_buff_size *= 2) * sizeof(int));
                for (k = nb_priv; k < priv_buff_size; ++k)
                  scop_usr->scalars_privatizable[k] = -1;
              }
            
              /* Memorize the scalar information in the
                 privatizable list. */
              scop_usr->scalars_privatizable[nb_priv++] = scalars[i];
              scop_usr->scalars_privatizable[nb_priv++] =
                  ((candl_statement_usr_p)statement[0]->usr)->index[j - 1];
            }
            
          } // end is_priv

        } // end else
        
        /* Go to the next block, if any. */
        for (k = 0; statement[k]; ++k)
          ;
        curlast = statement[k-1];

        if (curlast != last) {
          for (k = 0; fullchain[k]; ++k) {
            if (fullchain[k] == curlast) {
              s = fullchain[k+1];
              break;
            }
          }
        }
        free(statement);
        
      } while (curlast != last);
      
    } // end iterate all possible depth
    
    free(fullchain);
    
  } // end iterate scalars

  return 0;
}


/**
 * candl_num_dependences function:
 * This function returns the number of dependences in the dependence
 * list
 * \param dependence The first dependence of the dependence list.
 **
 */
int candl_num_dependences(candl_dependence_p candl_deps) {
  candl_dependence_p candl_dep = candl_deps;

  int num = 0;
  while (candl_dep != NULL) {
    num++;
    candl_dep = candl_dep->next;
  }
  return num;
}


/*
 * Convert a PIP quast to a union of polyhedra (Pip matrices)
 *
 * num: number of Pip matrices returned
 *
 **/
static PipMatrix **quast_to_polyhedra(PipQuast *quast, int *num,
                                       int nvar, int npar) {
  int num1, num2;
  PipMatrix** ep;
  PipMatrix** tp;
  PipMatrix** qp;
  int i, j;
  if (quast == NULL) {
    *num = 0;
    return NULL;
  }

  if (quast->condition != NULL) {
    tp = quast_to_polyhedra(quast->next_then, &num1, nvar, npar);
    ep = quast_to_polyhedra(quast->next_else, &num2, nvar, npar);

    /* Each of the matrices in the then tree needs to be augmented with
     * the condition */
    for (i = 0; i < num1; i++) {
      int nrows = tp[i]->NbRows;
      CANDL_set_si(tp[i]->p[nrows][0], 1);
      for (j = 1; j < 1 + nvar; j++)
        CANDL_set_si(tp[i]->p[nrows][j], 0);
      for (j = 0; j < npar + 1; j++)
        CANDL_assign(tp[i]->p[nrows][1+nvar+j],
                     quast->condition->the_vector[j]);
      (tp[i]->NbRows)++;
    }

    for (i = 0; i < num2; i++) {
      int nrows = ep[i]->NbRows;
      /* Inequality */
      CANDL_set_si(ep[i]->p[nrows][0], 1);
      for (j = 1; j < 1 + nvar; j++)
        CANDL_set_si(ep[i]->p[nrows][j], 0);
      for (j = 0; j < npar + 1; j++)
        CANDL_set_si(ep[i]->p[nrows][1+nvar+j],
                     -CANDL_get_si(quast->condition->the_vector[j]));
      (ep[i]->NbRows)++;
    }

    qp = (PipMatrix **) malloc((num1 + num2) * sizeof(PipMatrix*));
    for (i = 0; i < num1; ++i)
      qp[i] = tp[i];
    for (i = 0; i < num2; ++i)
      qp[i + num1] = ep[i];

    *num = num1 + num2;

    return qp;

  }
  else {
    /* quast condition is NULL */
    PipMatrix *lwmatrix = pip_matrix_alloc(nvar+npar+1, nvar+npar+2);

    PipList *vecList = quast->list;

    int count=0;
    while (vecList != NULL) {
      /* Equality */
      CANDL_set_si(lwmatrix->p[count][0], 0);
      for (j=0; j<nvar; j++)
        if (j == count)
          CANDL_set_si(lwmatrix->p[count][j+1], 1);
        else
          CANDL_set_si(lwmatrix->p[count][j+1], 0);

      for (j=0; j<npar; j++)
        CANDL_set_si(lwmatrix->p[count][j+1+nvar],
                     -CANDL_get_si(vecList->vector->the_vector[j]));
      /* Constant portion */
      if (quast->newparm != NULL)
        /* Don't handle newparm for now */
        CANDL_set_si(lwmatrix->p[count][npar+1+nvar],
                     -CANDL_get_si(vecList->vector->the_vector[npar+1]));
      else
        CANDL_set_si(lwmatrix->p[count][npar+1+nvar],
                     -CANDL_get_si(vecList->vector->the_vector[npar]));

      count++;

      vecList = vecList->next;
    }
    lwmatrix->NbRows = count;

    if (count > 0)
      *num = 1;
    else
      *num = 0;

    PipMatrix** ret = (PipMatrix**) malloc(sizeof(PipMatrix*));
    ret[0] = lwmatrix;
    return ret;
  }
}


/*
 * Compute last writer for a given dependence; does not make sense if the
 * supplied dependence is not a RAW or WAW dependence
 *
 * Returns 0 if lastwriter is computed successfully and dep domain updated,
 * returns 1 otherwise
 */
static 
int candl_dep_compute_lastwriter(candl_dependence_p *dep, osl_scop_p scop) {
  PipQuast *lexmax;
  PipOptions *pipOptions = pip_options_init();
  candl_statement_usr_p s_usr = (*dep)->source->usr;
  candl_statement_usr_p t_usr = (*dep)->target->usr;
  osl_relation_p new_domain;
  int i, j;
  int npar = scop->context->nb_parameters;
  int precision;
  
  #if defined(LINEAR_VALUE_IS_INT)
    precision = OSL_PRECISION_SP;
  #elif defined(LINEAR_VALUE_IS_LONGLONG)
    precision = OSL_PRECISION_DP;
  #elif defined(LINEAR_VALUE_IS_MP)
    precision =OSL_PRECISION_MP;
  #endif
  
  if (precision != scop->context->precision) {
    CANDL_FAIL("Precision not compatible with piplib ! (pip_relation2matrix)");
  }

  /* We do a parametric lexmax on the source iterators
   * keeping the target iterators as parameters */
  pipOptions->Maximize = 1;
  pipOptions->Simplify = 1;
  // pipOptions->Deepest_cut = 1;
  // pipOptions->Urs_unknowns = -1;
  // pipOptions->Urs_parms = -1;

  /* Build a context with equalities /inequalities only on the target
   * variables */
  osl_relation_p context = osl_relation_pmalloc(precision, 
                                          (*dep)->domain->nb_rows,
                                          (*dep)->target->domain->nb_columns);

  int nrows = 0;
  for (i = 0; i < (*dep)->domain->nb_rows; i++) {
    for (j = 1; j < s_usr->depth+1; j++) {
      if (!osl_int_zero(precision, (*dep)->domain->m[i], j))
        break;
    }
    
    if (j == t_usr->depth+1) {
      /* Include this in the context */
      osl_int_assign(precision,
                     context->m[nrows], 0,
                     (*dep)->domain->m[i], 0);
      
      for (j = 1; j < (*dep)->target->domain->nb_columns; j++)
        osl_int_assign(precision,
                       context->m[nrows], j,
                       (*dep)->domain->m[i], s_usr->depth+j);

      nrows++;
    }
  }
  context->nb_rows = nrows;

  /* Parameteric lexmax */
  lexmax = pip_solve_osl((*dep)->domain, context, -1, pipOptions);

  pip_options_free(pipOptions);

  if (lexmax == NULL) {
    printf("WARNING: last writer failed (mostly invalid dependence): %s\n",
           "bailing out safely without modification");
    osl_relation_print(stdout, (*dep)->domain);
    osl_relation_print(stdout, context);
    return 1;
  }

  int num;
  PipMatrix **qp = quast_to_polyhedra(lexmax, &num, s_usr->depth,
                                      t_usr->depth + npar);

  /* Update the dependence domains */
  if (num > 0) {
    int it_mat;
    osl_relation_p original_domain = (*dep)->domain;
    for (it_mat = 0; it_mat < num; ++it_mat) {
      new_domain = osl_relation_pmalloc(precision,
                                    original_domain->nb_rows +
                                    qp[it_mat]->NbRows,
                                    original_domain->nb_columns);

      for (i = 0; i < original_domain->nb_rows; i++)
        for (j = 0; j < original_domain->nb_columns; j++)
          osl_int_assign(precision,
                         new_domain->m[i], j,
                         original_domain->m[i], j);

      for (i = 0; i < qp[it_mat]->NbRows; i++)
        for (j = 0; j < original_domain->nb_columns; j++)
          osl_int_assign(precision,
                         new_domain->m[i+original_domain->nb_rows], j,
                         qp[it_mat]->p[i], j);

      (*dep)->domain = new_domain;
      /* More than 1 pipmatrix from the quast, we need to insert
         new dependences to have the union of domains. */
      if (it_mat < num - 1) {
        candl_dependence_p newdep = candl_dependence_malloc();
        newdep->source = (*dep)->source;
        newdep->target = (*dep)->target;
        newdep->depth = (*dep)->depth;
        newdep->type = (*dep)->type;
        newdep->ref_source = (*dep)->ref_source;
        newdep->ref_target = (*dep)->ref_target;
        newdep->usr = (*dep)->usr;
        newdep->source_nb_output_dims_domain = (*dep)->source_nb_output_dims_domain;
        newdep->source_nb_output_dims_access = (*dep)->source_nb_output_dims_access;
        newdep->target_nb_output_dims_domain = (*dep)->target_nb_output_dims_domain;
        newdep->target_nb_output_dims_access = (*dep)->target_nb_output_dims_access;
        newdep->source_nb_local_dims_domain  = (*dep)->source_nb_local_dims_domain;
        newdep->source_nb_local_dims_access  = (*dep)->source_nb_local_dims_access;
        newdep->target_nb_local_dims_domain  = (*dep)->target_nb_local_dims_domain;
        newdep->target_nb_local_dims_access  = (*dep)->target_nb_local_dims_access;
        newdep->next = (*dep)->next;
        (*dep)->next = newdep;
        *dep = newdep;
      }
    }

    osl_relation_free(original_domain);
    for (i = 0; i < num; ++i)
      pip_matrix_free(qp[i]);
  }

  if (qp) free(qp);
  pip_quast_free(lexmax);
  osl_relation_free(context);

  return 0;
}

/**
 * Compute the last writer for each RAW, WAW, and RAR dependence. This will
 * modify the dependence polyhedra. Be careful of any references to the old
 * dependence polyhedra. They are freed and new ones allocated.
 */
void candl_compute_last_writer(candl_dependence_p dep, osl_scop_p scop) {
  // int count=0;
  while (dep != NULL)    {
    if (dep->type != CANDL_WAR)   {
      // printf("Last writer for dep %d: %d %d\n", count++, dep->source->usr->depth, dep->target->usr->depth);
      // candl_matrix_print(stdout, dep->domain);
      candl_dep_compute_lastwriter(&dep, scop);
      // candl_matrix_print(stdout, dep->domain);
    }
    dep = dep->next;
  }
}
