// Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2021 PISM Authors
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

#include "GivenClimate.hh"

#include "pism/util/IceGrid.hh"
#include "pism/util/Time.hh"

#include "pism/coupler/util/options.hh"

namespace pism {
namespace ocean {

Given::Given(IceGrid::ConstPtr g)
  : OceanModel(g, std::shared_ptr<OceanModel>()) {

  m_shelf_base_temperature = allocate_shelf_base_temperature(g);
  m_shelf_base_mass_flux   = allocate_shelf_base_mass_flux(g);

  ForcingOptions opt(*m_grid->ctx(), "ocean.given");

  {
    unsigned int buffer_size = m_config->get_number("input.forcing.buffer_size");

    File file(m_grid->com, opt.filename, PISM_NETCDF3, PISM_READONLY);

    m_shelfbtemp = IceModelVec2T::ForcingField(m_grid,
                                               file,
                                               "shelfbtemp",
                                               "", // no standard name
                                               buffer_size,
                                               opt.periodic,
                                               LINEAR);

    m_shelfbmassflux = IceModelVec2T::ForcingField(m_grid,
                                                   file,
                                                   "shelfbmassflux",
                                                   "", // no standard name
                                                   buffer_size,
                                                   opt.periodic);
  }

  m_shelfbtemp->set_attrs("climate_forcing",
                          "absolute temperature at ice shelf base",
                          "Kelvin", "Kelvin", "", 0);
  m_shelfbmassflux->set_attrs("climate_forcing",
                              "ice mass flux from ice shelf base (positive flux is loss from ice shelf)",
                              "kg m-2 s-1", "kg m-2 year-1", "", 0);
}

void Given::init_impl(const Geometry &geometry) {

  m_log->message(2,
             "* Initializing the ocean model reading base of the shelf temperature\n"
             "  and sub-shelf mass flux from a file...\n");

  ForcingOptions opt(*m_grid->ctx(), "ocean.given");

  m_shelfbtemp->init(opt.filename, opt.periodic);
  m_shelfbmassflux->init(opt.filename, opt.periodic);

  // read time-independent data right away:
  if (m_shelfbtemp->buffer_size() == 1 && m_shelfbmassflux->buffer_size() == 1) {
    update(geometry, m_grid->ctx()->time()->current(), 0); // dt is irrelevant
  }

  const double
    ice_density   = m_config->get_number("constants.ice.density"),
    water_density = m_config->get_number("constants.sea_water.density"),
    g             = m_config->get_number("constants.standard_gravity");

  compute_average_water_column_pressure(geometry, ice_density, water_density, g,
                                           *m_water_column_pressure);
}

void Given::update_impl(const Geometry &geometry, double t, double dt) {
  (void) geometry;

  m_shelfbmassflux->update(t, dt);
  m_shelfbtemp->update(t, dt);

  m_shelfbmassflux->average(t, dt);
  m_shelfbtemp->average(t, dt);

  m_shelf_base_temperature->copy_from(*m_shelfbtemp);
  m_shelf_base_mass_flux->copy_from(*m_shelfbmassflux);

  const double
    ice_density   = m_config->get_number("constants.ice.density"),
    water_density = m_config->get_number("constants.sea_water.density"),
    g             = m_config->get_number("constants.standard_gravity");

  compute_average_water_column_pressure(geometry, ice_density, water_density, g,
                                           *m_water_column_pressure);
}

MaxTimestep Given::max_timestep_impl(double t) const {
  (void) t;

  return MaxTimestep("ocean th");
}

const IceModelVec2S& Given::shelf_base_temperature_impl() const {
  return *m_shelf_base_temperature;
}

const IceModelVec2S& Given::shelf_base_mass_flux_impl() const {
  return *m_shelf_base_mass_flux;
}

} // end of namespace ocean
} // end of namespace pism
