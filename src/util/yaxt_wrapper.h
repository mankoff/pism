/* Copyright (C) 2024 PISM Authors
 *
 * This file is part of PISM.
 *
 * PISM is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 *
 * PISM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PISM; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PISM_YAXT_WRAPPER_H
#define PISM_YAXT_WRAPPER_H

#include <mpi.h>

#ifdef __cplusplus
extern "C"
{
#endif

void pism_yaxt_initialize(MPI_Comm default_comm);

void pism_yaxt_finalize(void);

int pism_yaxt_initialized(void);

int pism_yaxt_finalized(void);

#ifdef __cplusplus
}
#endif

#endif /* PISM_YAXT_WRAPPER_H */
