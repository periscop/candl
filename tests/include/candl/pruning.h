
   /**------ ( ----------------------------------------------------------**
    **       )\                      CAnDL                               **
    **----- /  ) --------------------------------------------------------**
    **     ( * (                    pruning.h                            **
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

/**
 * \file pruning.h
 * \author Louis-Noel Pouchet
 */

#ifndef CANDL_PRUNING_H
# define CANDL_PRUNING_H


# include <stdio.h>
# include <candl/matrix.h>
# include <candl/options.h>
# include <candl/matrix.h>


# if defined(__cplusplus)
extern "C"
  {
# endif

candl_dependence_p
candl_dependence_prune_transitively_covered(candl_dependence_p deps);

# if defined(__cplusplus)
  }
# endif
#endif /* define CANDL_PRUNING_H */

