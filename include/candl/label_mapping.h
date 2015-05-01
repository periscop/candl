
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                label_mapping.h                          **
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

#define CANDL_LABEL_UNDEFINED (-1)

struct candl_label_mapping {
  int original;
  int mapped;
  struct candl_label_mapping * next;
};

typedef struct candl_label_mapping   candl_label_mapping_t;
typedef struct candl_label_mapping * candl_label_mapping_p;

candl_label_mapping_p candl_label_mapping_malloc();
void candl_label_mapping_free(candl_label_mapping_p);
void candl_label_mapping_add(candl_label_mapping_p *, candl_label_mapping_p);
void candl_label_mapping_add_map(candl_label_mapping_p * , int, int);
int candl_label_mapping_find_original(candl_label_mapping_p, int);
void candl_label_mapping_print(FILE *, candl_label_mapping_p);

