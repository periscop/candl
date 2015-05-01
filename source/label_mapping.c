
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                label_mapping.c                          **
    **----  \#/  --------------------------------------------------------**
    **    .-"#'-.        First version: may 1st 2015                     **
    **--- |"-.-"| -------------------------------------------------------**
    |     |
    |     |
 ******** |     | *************************************************************
 * CAnDL  '-._,-' the Chunky Analyzer for Dependences in Loops (experimental) *
 ******************************************************************************
 *                                                                            *
 * Copyright (C) 2003-2015 Cedric Bastoul                                     *
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
 * File written by Oleksandr Zinenko, Oleksandr.Zinenko@inria.fr              *
 *                                                                            *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <candl/label_mapping.h>
#include <candl/macros.h>
#include <candl/scop.h>
#include <candl/statement.h>

candl_label_mapping_p candl_label_mapping_malloc() {
  candl_label_mapping_p mapping;
  CANDL_malloc(mapping, candl_label_mapping_p, sizeof(candl_label_mapping_t));
  mapping->original = CANDL_LABEL_UNDEFINED;
  mapping->mapped = CANDL_LABEL_UNDEFINED;
  mapping->next = NULL;
  return mapping;
}

void candl_label_mapping_free(candl_label_mapping_p mapping) {
  candl_label_mapping_p ptr;
  while (mapping != NULL) {
    ptr = mapping->next;
    free(mapping);
    mapping = ptr;
  }
}

void candl_label_mapping_add(candl_label_mapping_p * mapping,
                             candl_label_mapping_p instance) {
  candl_label_mapping_p ptr;
  if (*mapping == NULL) {
    *mapping = instance;
  } else {
    for (ptr = *mapping; ptr->next != NULL; ptr = ptr->next)
      ;
    ptr->next = instance;
  }
}

void candl_label_mapping_add_map(candl_label_mapping_p * mapping,
                                 int original, int mapped) {
  candl_label_mapping_p instance;

  instance = candl_label_mapping_malloc();
  instance->original = original;
  instance->mapped = mapped;

  candl_label_mapping_add(mapping, instance);
}

int candl_label_mapping_find_original(candl_label_mapping_p mapping,
                                      int mapped) {
  for ( ; mapping != NULL; mapping = mapping->next) {
    if (mapping->mapped == mapped)
      return mapping->original;
  }
  return CANDL_LABEL_UNDEFINED;
}

void candl_label_mapping_print(FILE *f, candl_label_mapping_p mapping) {
  for ( ; mapping != NULL; mapping = mapping->next) {
    fprintf(f, "%d -> %d\n", mapping->original, mapping->mapped);
  }
}

