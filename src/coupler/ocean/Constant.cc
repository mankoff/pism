// Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 PISM Authors
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

#include "Constant.hh"
#include "pism/util/Vars.hh"
#include "pism/util/ConfigInterface.hh"
#include "pism/util/IceGrid.hh"
#include "pism/util/pism_options.hh"
#include "pism/util/iceModelVec.hh"
#include "pism/util/error_handling.hh"
#include "pism/util/io/io_helpers.hh"
#include "pism/util/MaxTimestep.hh"
#include "pism/util/pism_utilities.hh"

namespace pism {
namespace ocean {
Constant::Constant(IceGrid::ConstPtr g)
  : OceanModel(g) {

  m_meltrate = m_config->get_double("ocean.constant.melt_rate", "m second-1");

  // convert to [kg m-2 s-1] = [m s-1] * [kg m-3]
  double ice_density = m_config->get_double("constants.ice.density");
  m_meltrate = m_meltrate * ice_density;
}

Constant::~Constant() {
  // empty
}

void Constant::update_impl(double my_t, double my_dt) {
  melting_point_temperature(m_shelf_base_temperature);

  m_shelf_base_mass_flux.set(m_meltrate);
}

void Constant::init_impl() {

  if (not m_config->get_boolean("ocean.always_grounded")) {
    m_log->message(2, "* Initializing the constant ocean model...\n");
    m_log->message(2, "  Sub-shelf melt rate set to %f m/year.\n",
                   units::convert(m_sys, m_meltrate, "m second-1", "m year-1"));

  }
}

MaxTimestep Constant::max_timestep_impl(double t) const {
  (void) t;
  return MaxTimestep("ocean constant");
}

void Constant::melting_point_temperature(IceModelVec2S &result) const {
  const double T0 = m_config->get_double("constants.fresh_water.melting_point_temperature"), // K
    beta_CC       = m_config->get_double("constants.ice.beta_Clausius_Clapeyron"),
    g             = m_config->get_double("constants.standard_gravity"),
    ice_density   = m_config->get_double("constants.ice.density");

  const IceModelVec2S *ice_thickness = m_grid->variables().get_2d_scalar("land_ice_thickness");

  IceModelVec::AccessList list{ice_thickness, &result};

  for (Points p(*m_grid); p; p.next()) {
    const int i = p.i(), j = p.j();
    const double pressure = ice_density * g * (*ice_thickness)(i,j); // FIXME issue #15
    // result is set to melting point at depth
    result(i,j) = T0 - beta_CC * pressure;
  }
}

} // end of namespape ocean
} // end of namespace pism
