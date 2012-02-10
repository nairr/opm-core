/*
  Copyright 2012 SINTEF ICT, Applied Mathematics.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <opm/core/transport/reorder/TransportModelInterface.hpp>
#include <opm/core/transport/reorder/reordersequence.h>
#include <opm/core/grid.h>

#include <vector>
#include <cassert>


void Opm::TransportModelInterface::reorderAndTransport(const UnstructuredGrid& grid, const double* darcyflux)
{
    // Compute sequence of single-cell problems
    std::vector<int> sequence(grid.number_of_cells);
    std::vector<int> components(grid.number_of_cells + 1);
    int ncomponents;
    compute_sequence(&grid, darcyflux, &sequence[0], &components[0], &ncomponents);

    // Assume all strong components are single-cell domains.
    assert(ncomponents == grid.number_of_cells);
    for (int i = 0; i < grid.number_of_cells; ++i) {
#ifdef MATLAB_MEX_FILE
        if (interrupt_signal) {
            mexPrintf("Reorder loop interrupted by user: %d of %d "
                      "cells finished.\n", i, grid.number_of_cells);
            break;
        }
#endif
        solveSingleCell(sequence[i]);
    }
}