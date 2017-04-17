// Copyright (C) 2008-2016 PISM Authors
//
// This file is part of PISM.
//
// PISM is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.
//
// PISM is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License
// along with PISM; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <gsl/gsl_math.h>

#include "base/util/IceGrid.hh"
#include "base/util/MaxTimestep.hh"
#include "base/util/PISMVars.hh"
#include "base/util/io/PIO.hh"
#include "base/util/pism_const.hh"
#include "base/util/pism_utilities.hh"
#include "icebin/IBSurfaceModel.hh"

using namespace std;

namespace pism {
namespace icebin {

///// Constant-in-time surface model for accumulation,
///// ice surface temperature parameterized as in PISM-IBSurfaceModel dependent on latitude and surface elevation


IBSurfaceModel::IBSurfaceModel(IceGrid::ConstPtr g) : SurfaceModel(g) {
  printf("BEGIN IBSurfaceModel::allocate_IBSurfaceModel()\n");

  create(icebin_massxfer, "icebin_massxfer");
  icebin_massxfer.set_attrs("climate_state",
    "Mass of ice being transferred Stieglitz --> Icebin",
    "kg m-2 s-1", "land_ice_surface_specific_mass_balance");
  icebin_massxfer.metadata().set_string("glaciological_units", "kg m-2 year-1");
  icebin_massxfer.write_in_glaciological_units = true;

  create(icebin_enthxfer, "icebin_enthxfer");
  icebin_enthxfer.set_attrs("climate_state",
    "Enthalpy of ice being transferred Stieglitz --> Icebin",
    "W m-2", "land_ice_surface_specific_mass_balance");

  // ------- Dirichlet Bondary condition derived from icebin_deltah

  create(icebin_bc_temp, "icebin_bc_temp");
  icebin_bc_temp.set_attrs("climate_state",
    "Temperature of the Dirichlet B.C.",
    "K", "");

  create(icebin_bc_waterfraction, "icebin_bc_waterfraction");
  icebin_bc_waterfraction.set_attrs("climate_state",
    "Water content of the Dirichlet B.C.",
    "1", "");


  printf("END IBSurfaceModel::allocate_IBSurfaceModel()\n");
}

void IBSurfaceModel::attach_atmosphere_model_impl(atmosphere::AtmosphereModel *input) {
  delete input;
}

void IBSurfaceModel::init_impl() {
  m_t = m_dt = GSL_NAN; // every re-init restarts the clock

  m_log->message(2, "* Initializing the IceBin interface surface model IBSurfaceModel.\n"
                    "  IceBin changes its state when surface conditions change.\n");

  // find PISM input file to read data from:
  m_input_file = process_input_options(m_grid->com).filename;

  // It doesn't matter what we set this to, it will be re-set later.
  for (auto vec=vecs.begin(); vec != vecs.end(); ++vec) {
    vec->second->set(0.0);
  }

  _initialized = true;
}

MaxTimestep IBSurfaceModel::max_timestep_impl(double t) {
  (void)t;
  return MaxTimestep();
}

void IBSurfaceModel::update_impl(double my_t, double my_dt) {
  if ((fabs(my_t - m_t) < 1e-12) && (fabs(my_dt - m_dt) < 1e-12)) {
    return;
  }

  m_t  = my_t;
  m_dt = my_dt;
}

void IBSurfaceModel::get_diagnostics_impl(std::map<std::string, Diagnostic::Ptr> & /*dict*/,
                                          std::map<std::string, TSDiagnostic::Ptr> & /*ts_dict*/) {
  // empty (does not have an atmosphere model)
}

void IBSurfaceModel::ice_surface_mass_flux_impl(IceModelVec2S &result) {
  result.copy_from(icebin_massxfer);
}

void IBSurfaceModel::ice_surface_temperature_impl(IceModelVec2S &result) {
  result.copy_from(icebin_bc_temp);
}

void IBSurfaceModel::ice_surface_liquid_water_fraction(IceModelVec2S &result) {
  result.copy_from(icebin_bc_waterfraction);
}

void IBSurfaceModel::add_vars_to_output_impl(const std::string & /*keyword*/, std::set<std::string> &result) {
  for (auto vec=vecs.begin(); vec != vecs.end(); ++vec) {
    result.insert(vec->first);
  }
  // does not call atmosphere->add_vars_to_output().
}

void IBSurfaceModel::define_variables_impl(const std::set<std::string> &vars, const PIO &nc, IO_Type nctype) {
  SurfaceModel::define_variables_impl(vars, nc, nctype);

  for (auto vec=vecs.begin(); vec != vecs.end(); ++vec) {
    if (set_contains(vars, vec->first))
      vec->second->define(nc, nctype);
  }
}

void IBSurfaceModel::write_variables_impl(const std::set<std::string> &vars, const PIO &nc) {
  for (auto vec=vecs.begin(); vec != vecs.end(); ++vec) {
    if (set_contains(vars, vec->first))
      vec->second->write(nc, nctype);
    }
  }
}

// ================================================================
/** Assumes icebin_deltah has been set.  Sets icebin_bc_xxx variables accordingly. */
void IBSurfaceModel::construct_dirichlet_bc(pism::IceModelVec2S &surface_senth)
{
    AccessList access{
        &icebin_deltah, &surface_senth,    // IN
        &icebin_bc_senth, &icebin_bc_temp, &icebin_bc_waterfraction
    };


}

} // end of namespace surface
} // end of namespace pism
