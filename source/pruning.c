
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                    pruning.c                            **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: July 17th 2011                   **
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
 * \file pruning.c
 * \author Louis-Noel Pouchet
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osl/scop.h>
#include <osl/statement.h>
#include <osl/relation.h>
#include <candl/candl.h>
#include <candl/usr.h>

#include <assert.h>


#if defined(CANDL_COMPILE_PRUNNING_C)


/**
 * Return true if the 2 matrices are strictly identical.
 */
static
int candl_matrix_equal(CandlMatrix* m1, CandlMatrix* m2) {
  int i, j;

  if (m1 == NULL) {
    if (m2 == NULL)
      return 1;
    return 0;
  }

  if (m1->NbRows != m2->NbRows || m1->NbColumns != m2->NbColumns)
    return 0;

  for (i = 0; i < m1->NbRows; ++i)
    for (j = 0; j < m1->NbColumns; ++j)
      if (! CANDL_eq(m1->p[i][j], m2->p[i][j]))
        return 0;
  return 1;
}

#define BUFF_SIZE 1024

/**
 * Finds all paths in the graph represented by the list of dependences
 * 'alldeps', that start from statement label 'tgt_id' and ends at
 * statement label 'final_id', of length <= 'max_length'.
 * Paths are stored as list of lists of dependences in 'paths_list'.
 */
static
void find_paths_rec(int tgt_id, int cur_length, int max_length,
                    int final_id,
                    candl_dependence_p* alldeps,
                    candl_dependence_p* cur_path,
                    candl_dependence_p*** paths_list) {
  int i;

  for (i = 0; alldeps[i]; ++i) {
    if (alldeps[i]->usr == NULL) {
      if (alldeps[i]->source->label == tgt_id) {
        // Ensure the path flow is consistent.
        if (cur_length > 1) {
          if (cur_path[cur_length - 1]->type == CANDL_RAW ||
              cur_path[cur_length - 1]->type == CANDL_RAW_SCALPRIV ||
              cur_path[cur_length - 1]->type == CANDL_RAR) {
            // RAW or RAR.
            if (alldeps[i]->type != CANDL_RAR &&
                alldeps[i]->type != CANDL_WAR)
              continue;
          }
          else {
            // WAW or WAR.
            if (alldeps[i]->type != CANDL_WAW &&
                alldeps[i]->type != CANDL_RAW)
              continue;
          }
        }
        if (cur_length + 1 == max_length) {
          if (alldeps[i]->target->label == final_id) {
            // Found a path.
            int j, pos;
            for (pos = 0; (*paths_list)[pos]; ++pos)
              ;
            if (pos + 1 % BUFF_SIZE == 0) {
              *paths_list = (candl_dependence_p**)
                  realloc(*paths_list, sizeof(candl_dependence_p*) *
                          (BUFF_SIZE + pos + 1));
              *paths_list[pos + 1] = NULL;
            }
            (*paths_list)[pos] = (candl_dependence_p*)
                malloc((max_length + 1) * sizeof(candl_dependence_p));
            for (j = 0; j < max_length - 1; ++j)
              (*paths_list)[pos][j] = cur_path[j];
            (*paths_list)[pos][j++] = alldeps[i];
            (*paths_list)[pos][j] = NULL;
          }
        }
        else {
          // Store the current node in the path.
          cur_path[cur_length] = alldeps[i];
          // Mark the dependence as processed.
          alldeps[i]->usr = alldeps[i];
          // Look for the next node.
          find_paths_rec (alldeps[i]->target->label, cur_length + 1,
                          max_length, final_id, alldeps, cur_path,
                          paths_list);
          // Reset the dependence.
          alldeps[i]->usr = NULL;
        }
      }
    }
  }
}


/**
 * Returns a list of list of dependences containing all paths in the
 * graph represented by the list of dependences 'ardeps', that start
 * from statement label 'source->label' and ends at statement label
 * 'target->label'.
 */
static
candl_dependence_p**
find_dep_paths(candl_dependence_p* ardeps,
               CandlStatement* source,
               CandlStatement*target) {
  int i, nb_dep;
  for (nb_dep = 0; ardeps[nb_dep]; ++nb_dep)
    ;
  if (nb_dep < 2)
    return NULL;
  candl_dependence_p cur_path[nb_dep + 1];
  candl_dependence_p** paths_list =
      (candl_dependence_p**)malloc(BUFF_SIZE * sizeof(candl_dependence_p*));
  for (i = 0; i < BUFF_SIZE; ++i)
    paths_list[i] = NULL;
  // Iterate on all possible paths length, from Sx to Sy, of length y-x.
  for (i = 2; i <= target->label - source->label; ++i)
    find_paths_rec (source->label, 0, i, target->label, ardeps, cur_path,
                    &paths_list);

  return paths_list;
}

/**
 * Return true if the 'size' first elements of 'l1' and 'l2' are equal.
 */
static
int piplist_are_equal (PipList* l1, PipList* l2, int size) {
  if (l1 == NULL && l2 == NULL)
    return 1;
  if (l1 == NULL || l2 == NULL)
    return 0;
  if (l1->vector == NULL && l2->vector == NULL)
    return 1;
  if (l1->vector == NULL || l2->vector == NULL)
    return 0;

  int count = 0;
  for (; l1 && l2 && count < size; l1 = l1->next, l2 = l2->next, ++count) {
    if (l1->vector == NULL && l2->vector == NULL)
      return 1;
    if (l1->vector == NULL || l2->vector == NULL)
      return 0;
    if (l1->vector->nb_elements != l2->vector->nb_elements)
      return 0;
    int j;
    for (j = 0; j < l1->vector->nb_elements; ++j)
      if (! CANDL_eq(l1->vector->the_vector[j],
                     l2->vector->the_vector[j]) ||
          ! CANDL_eq(l1->vector->the_deno[j],
                     l2->vector->the_deno[j]))
        return 0;
  }

  return 1;
}

/**
 * Return true if the 'size' first variables in a quast are strictly
 * equal.
 */
static
int
quast_are_equal (PipQuast* q1, PipQuast* q2, int size) {
  if (q1 == NULL && q2 == NULL)
    return 1;
  if (q1 == NULL || q2 == NULL)
    return 0;

  // Inspect conditions.
  if (q1->condition != NULL && q2->condition != NULL) {
    PipList c1; c1.next = NULL; c1.vector = q1->condition;
    PipList c2; c2.next = NULL; c2.vector = q2->condition;
    if (! piplist_are_equal(&c1, &c2, size))
      return 0;
    return quast_are_equal (q1->next_then, q2->next_then, size) &&
        quast_are_equal (q1->next_else, q2->next_else, size);
  }
  if (q1->condition != NULL || q2->condition != NULL)
    return 0;
  return piplist_are_equal (q1->list, q2->list, size);
}

/**
 * Return true if 'dep' is fully covered by the transitive dependences
 * in 'path'. This is a conservative functions (works only on
 * unimodular access functions + iteration domains for the sub-matrix
 * corresponding to loop iterators).
 */
static
int
is_covering(candl_dependence_p dep, candl_dependence_p*  path) {
  int i, path_length;

  for (path_length = 0; path[path_length]; ++path_length)
    ;

  /// FIXME: This may be overly conservative.
  // Check the path extremal points type.
  if (dep->type == CANDL_RAW || dep->type == CANDL_RAW_SCALPRIV
      || dep->type == CANDL_WAW) {
    // RAW or WAW.
    if (path[0]->type != CANDL_RAW && path[0]->type != CANDL_RAW_SCALPRIV &&
        path[0]->type != CANDL_WAW)
      return 0;
    if (dep->type == CANDL_RAW || dep->type == CANDL_RAW_SCALPRIV)
      if (path[path_length - 1]->type != CANDL_RAW &&
          path[path_length - 1]->type != CANDL_RAW_SCALPRIV &&
          path[path_length - 1]->type != CANDL_RAR)
        return 0;
    if (dep->type == CANDL_WAW)
      if (path[path_length - 1]->type != CANDL_WAR &&
          path[path_length - 1]->type != CANDL_WAW)
        return 0;
  }
  else {
    // WAR or RAR.
    if (path[0]->type != CANDL_WAR && path[0]->type != CANDL_RAR)
      return 0;
    if (dep->type == CANDL_WAR)
      if (path[path_length - 1]->type != CANDL_WAW &&
          path[path_length - 1]->type != CANDL_WAR)
        return 0;
    if (dep->type == CANDL_RAR)
      if (path[path_length - 1]->type != CANDL_RAR &&
          path[path_length - 1]->type != CANDL_RAW_SCALPRIV &&
          path[path_length - 1]->type != CANDL_RAW)
        return 0;
  }

  // a- Fast check. Ensure the dependence depth is consistent across
  // the path. We may correctly cover _more_ points and still have a
  // perfect transitive cover.
  /// FIXME: ambiguous test?
  /*   for (i = 0; i < path_length; ++i) */
  /*     if (path[i]->depth > dep->depth) */
  /*       return 0; */

  // b- Check the covering property. Works only if
  // the iterator part of iteration domains and access functions are
  // unimodular matrices.
  // Build a large system with all dependences.
  int nb_par = path[0]->source->domain->NbColumns - path[0]->source->depth - 2;
  int nb_iters = 0;
  int nb_rows = 0;
  for (i = 0; i < path_length; ++i) {
    nb_iters += path[i]->domain->NbColumns - nb_par - 2;
    nb_rows += path[i]->domain->NbRows;
    if (i > 0)
      nb_iters -= path[i]->source->depth;
  }
  CandlMatrix* syst = candl_matrix_malloc(nb_rows, nb_iters + nb_par + 2);
  int pos = 0;
  int j, k;
  int iter_off = 0;
  for (k = 0; k < path_length; ++k) {
    for (i = 0; i < path[k]->domain->NbRows; ++i) {
      CANDL_assign(syst->p[pos][0], path[k]->domain->p[i][0]);
      for (j = 1; j < path[k]->domain->NbColumns - 1 - nb_par; ++j)
        CANDL_assign(syst->p[pos][j + iter_off], path[k]->domain->p[i][j]);
      for (; j < path[k]->domain->NbColumns; ++j) {
        int parpos = j - (path[k]->domain->NbColumns - 1 - nb_par) +
            syst->NbColumns - nb_par - 1;
        CANDL_assign(syst->p[pos][parpos], path[k]->domain->p[i][j]);
      }
      ++pos;
    }
    iter_off += path[k]->source->depth;
  }

  // Algo:
  // lexmin(dep, R) == lexmin(path, R) && lexmax(dep, R) == lexmax(path, R) &&
  // lexmin(dep, S) == lexmin(path, S) && lexmax(dep, S) == lexmax(path, S)
  PipOptions* options;
  options = pip_options_init ();
  options->Simplify = 1;
  options->Urs_parms = -1;
  options->Urs_unknowns = -1;
  CandlMatrix* context = candl_matrix_malloc (0, nb_par + 2);
  PipQuast* qpath = pip_solve (syst, context, -1, options);
  PipQuast* qdep = pip_solve (dep->domain, context, -1, options);
  int are_equal = quast_are_equal (qpath, qdep, dep->source->depth);
  if (are_equal) {
    pip_quast_free (qpath);
    pip_quast_free (qdep);
    options->Maximize = 1;
    qpath = pip_solve (syst, context, -1, options);
    qdep = pip_solve (dep->domain, context, -1, options);
    are_equal = quast_are_equal (qpath, qdep, dep->source->depth);
  }
  if (are_equal) {
    pip_quast_free (qpath);
    pip_quast_free (qdep);
    options->Maximize = 0;
    // Permute columns for first and last iterators.
    for (i = 0; i < dep->target->depth; ++i) {
      for (j = 0; j < syst->NbRows; ++j) {
        int tmp = CANDL_get_si(syst->p[j][1]);
        int pos = syst->NbColumns - 1 - nb_par - dep->target->depth + i;
        CANDL_assign(syst->p[j][1], syst->p[j][pos]);
        CANDL_set_si(syst->p[j][pos], tmp);
      }
    }
    qpath = pip_solve (syst, context, -1, options);
    qdep = pip_solve (dep->domain, context, -1, options);
    are_equal = quast_are_equal (qpath, qdep, dep->target->depth);
  }
  if (are_equal) {
    pip_quast_free (qpath);
    pip_quast_free (qdep);
    options->Maximize = 1;
    qpath = pip_solve (syst, context, -1, options);
    qdep = pip_solve (dep->domain, context, -1, options);
    are_equal = quast_are_equal (qpath, qdep, dep->target->depth);
  }

  pip_options_free (options);
  pip_quast_free (qpath);
  pip_quast_free (qdep);
  pip_matrix_free (syst);

  return are_equal;
}

static
int
is_iter_unimodular (candl_dependence_p dep) {
  // Check unimodular on the iterator part.
  int i, j;
  for (i = 0; i < dep->domain->NbRows; ++i) {
    if (i < dep->source->domain->NbRows ||
        i > dep->source->domain->NbRows + dep->target->domain->NbRows) {
      for (j = 1; j <= dep->source->depth; ++j) {
        int val = CANDL_get_si(dep->domain->p[i][j]);
        if (val < -1 || val > 1)
          return 0;
      }
    }
    else if (i < dep->source->domain->NbRows +
             dep->target->domain->NbRows  ||
             i > dep->source->domain->NbRows + dep->target->domain->NbRows) {
      for (j = 1; j <= dep->source->depth; ++j) {
        int val = CANDL_get_si(dep->domain->p[i][j + dep->source->depth]);
        if (val < -1 || val > 1)
          return 0;
      }
    }
  }

  return 1;
}

/**
 * Remove somes dependences that are duplicates under transitive closure.
 * In-place modification of the list of dependence polyhedra.
 *
 */
candl_dependence_p
candl_dependence_prune_transitively_covered(candl_dependence_p deps) {
  candl_dependence_p tmp;
  candl_statement_usr_p s_usr, t_usr;
  osl_relation_p srcmat;

  // 1- Collect all arrays that occur in dependences.
  int cnt, i, j, k, l;
  int nb_stmts = 0;
  for (tmp = deps, cnt = 0; tmp; tmp = tmp->next) {
    s_usr = tmp->source->usr;
    t_usr = tmp->target->usr;
    nb_stmts = s_usr->label > nb_stmts ? s_usr->label : nb_stmts;
    nb_stmts = t_usr->label > nb_stmts ? t_usr->label : nb_stmts;
    ++cnt;
  }
  ++nb_stmts;
  int nb_deps = cnt;
  int *allarrays = (int*) malloc(sizeof(int) * cnt);
  
  for (tmp = deps, cnt = 0; tmp; tmp = tmp->next) {
    if (tmp->type == CANDL_RAW || tmp->type == CANDL_RAW_SCALPRIV
        || tmp->type == CANDL_WAW)
      srcmat = tmp->source->written;
    else
      srcmat = tmp->source->read;
    for (i = 0; i < cnt; ++i)
      if (allarrays[i] == CANDL_get_si(srcmat->p[tmp->ref_source][0]))
        break;
    if (i == cnt)
      allarrays[cnt++] = CANDL_get_si(srcmat->p[tmp->ref_source][0]);
  }
  allarrays[cnt] = -1;

  // 2- Iterate on all arrays.
  for (i = 0; allarrays[i] != -1; ++i) {
    // a- Collect all dependences to this array.
    candl_dependence_p ardeps[nb_deps + 1];
    for (tmp = deps, cnt = 0; tmp; tmp = tmp->next) {
      CandlMatrix* srcmat;
      if (tmp->type == CANDL_RAW || tmp->type == CANDL_RAW_SCALPRIV
          || tmp->type == CANDL_WAW)
        srcmat = tmp->source->written;
      else
        srcmat = tmp->source->read;
      if (allarrays[i] == CANDL_get_si(srcmat->p[tmp->ref_source][0]))
        ardeps[cnt++] = tmp;
    }
    ardeps[cnt] = NULL;

    // b- First pruning. Remove all dependence polyhedra that are equal.
    for (j = 0; ardeps[j]; ++j) {
      for (k = j + 1; ardeps[k]; ++k)
        if (ardeps[j]->source == ardeps[k]->source &&
            ardeps[j]->target == ardeps[k]->target &&
            ardeps[j]->depth == ardeps[k]->depth &&
            candl_matrix_equal (ardeps[j]->domain, ardeps[k]->domain)) {
          ardeps[k - 1]->next = ardeps[k+1];
          for (l = k; ardeps[l + 1]; ++l)
            ardeps[l] = ardeps[l + 1];
          ardeps[l] = NULL;
          --k;
        }
    }

    // c- Local pruning. Remove all self-dependences (1-cycles)
    // and all backward-dependences.
    for (j = 0; ardeps[j]; ++j)
      if (ardeps[j]->source >= ardeps[j]->target) {
        for (k = j; ardeps[k + 1]; ++k)
          ardeps[k] = ardeps[k + 1];
        ardeps[k] = NULL;
        --j;
      }

    // d- Local pruning. Remove all dependences where source/target
    // are not unimodular in the iterator part of the access function and
    // iteration domain.
    for (j = 0; ardeps[j]; ++j)
      if (! is_iter_unimodular (ardeps[j])) {
        for (k = j; ardeps[k + 1]; ++k)
          ardeps[k] = ardeps[k + 1];
        ardeps[k] = NULL;
        --j;
      }

    // d- Given a pair of statements, check if there is a dependence
    // path from its source to its target, of same type of a found
    // direct dependence.
    int stmts, stmtt;
    for (stmts = 0; stmts < nb_stmts - 2; ++stmts)
      for (stmtt = stmts + 2; stmtt < nb_stmts; ++stmtt) {
        // Ensure there exists a direct dependence between stmts
        // and stmtt.
        //printf ("consider S%d -> S%d\n", stmts, stmtt);
        for (j = 0; ardeps[j]; ++j)
          if (ardeps[j]->source->label == stmts &&
              ardeps[j]->target->label == stmtt)
            break;
        if (ardeps[j]) {
          CandlStatement* source = ardeps[j]->source;
          CandlStatement* target = ardeps[j]->target;

          // Subset of deps that can be on the path.
          candl_dependence_p curardeps[nb_deps + 1];
          for (k = 0, l = 0; ardeps[k]; ++k)
            if (ardeps[k]->source->label >= source->label &&
                ardeps[k]->source->label <= target->label &&
                ardeps[k]->target->label >= source->label &&
                ardeps[k]->target->label <= target->label)
              curardeps[l++] = ardeps[k];
          curardeps[l] = NULL;
          candl_dependence_p** paths =
              find_dep_paths(curardeps, source, target);

          if (paths) {
            for (j = 0; ardeps[j]; ++j)
              if (ardeps[j]->source->label == stmts &&
                  ardeps[j]->target->label == stmtt) {
                // Inspect all paths. If there is a path
                // that respect the transitive cover prop,
                // then discard the dependence.
                for (k = 0; paths[k] &&
                     ! is_covering (ardeps[j], paths[k]); ++k)
                  ;
                if (paths[k]) {
                  candl_matrix_free (ardeps[j]->domain);
                  free (ardeps[j]);
                  if (j == 0)
                    deps = ardeps[j + 1];
                  else
                    ardeps[j - 1]->next = ardeps[j + 1];
                }
              }
            for (k = 0; paths[k]; ++k)
              free (paths[k]);
            free (paths);
          }
        }
      }
  }
  
  free(allarrays);

  return deps;
}

#endif
