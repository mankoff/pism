#!/bin/bash

# Runs a flat-bed isothermal SIA setup using EISMINT-II experiment A surface mass balance,
# starting from zero ice thickness. Adds new isochrones every 1000 years. Uses regridding
# to double the horizontal grid resolution after 10500 years.

# number of MPI processes to use
N=${N:-8}

# horizontal grid size
M=${M:-51}

common_options="
        -output.extra.times 50
        -output.extra.vars isochrone_depth,thk
        -output.sizes.medium isochrone_depth,uvel
        -stress_balance.sia.flow_law isothermal_glen
        -stress_balance.sia.surface_gradient_method eta
        -isochrones.deposition_times 0:1000:20e3
        -energy.enabled no
        -grid.registration corner
        -Mz 21
"

mpiexec -n ${N} pism -eisII A \
        -bootstrapping.defaults.geothermal_flux 0 \
        -grid.Lz 3500 \
        -grid.Mx ${M} \
        -grid.My ${M} \
        -isochrones.bootstrapping.n_layers 0 \
        -output.extra.file ex_regrid_part1.nc \
        -output.file o_regrid_part1.nc \
        -time.end 10.5e3 \
        ${common_options}

mpiexec -n ${N} pism \
        -i o_regrid_part1.nc \
        -bootstrap \
        -Mx $(( M * 2 - 1 )) \
        -My $(( M * 2 - 1 )) \
        -regrid_file o_regrid_part1.nc \
        -regrid_vars isochronal_layer_thickness \
        -output.extra.file ex_regrid_part2.nc \
        -output.file o_regrid_part2.nc \
        -time.end 20e3 \
        ${common_options}
