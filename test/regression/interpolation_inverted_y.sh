#!/bin/bash

set -e
set -u
set -x

PISM_PATH=$1
MPIEXEC=$2
PISM_SOURCE_DIR=$3

pism=${PISM_PATH}/pism

regular=${PISM_SOURCE_DIR}/test/regression/pico_split/bedmap2_schmidtko14_50km.nc
inverted=ant_inverted.nc

# create a temporary directory and set up automatic cleanup
temp_dir=$(mktemp -d --tmpdir pism-regrid-invertlat-XXXX)
trap 'rm -rf "$temp_dir"' EXIT
cd $temp_dir

cdo invertlat ${regular} ${inverted}

common_options="
-config ${PISM_PATH}/pism_config.nc
-atmosphere uniform
-bootstrap
-energy none
-i ${regular}
-no_mass
-regrid_vars topg
-stress_balance none
-surface simple
-y 1s
"

${pism} ${common_options} -regrid_file ${regular}  -o regular_topg.nc
${pism} ${common_options} -regrid_file ${inverted} -o inverted_topg.nc

${PISM_PATH}/pism_nccmp -v topg regular_topg.nc inverted_topg.nc
