// Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2022, 2023, 2024 Andy Aschwanden and Constantine Khroulev
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

#include "pism/coupler/surface/Elevation.hh"

#include "pism/util/Grid.hh"
#include "pism/util/ConfigInterface.hh"
#include "pism/util/error_handling.hh"
#include "pism/util/pism_options.hh"
#include "pism/util/MaxTimestep.hh"
#include "pism/geometry/Geometry.hh"

namespace pism {
namespace surface {

///// Elevation-dependent temperature and surface mass balance.
Elevation::Elevation(std::shared_ptr<const Grid> grid, std::shared_ptr<atmosphere::AtmosphereModel> input)
  : SurfaceModel(grid) {
  (void) input;

  m_temperature = allocate_temperature(grid);
  m_mass_flux   = allocate_mass_flux(grid);
}

void Elevation::init_impl(const Geometry &geometry) {
  (void) geometry;

  bool limits_set = false;

  m_log->message(2,
                 "* Initializing the constant-in-time surface processes model Elevation. Setting...\n");

  {
    // ice surface temperature
    {
      m_T_min   = m_config->get_number("surface.elevation_dependent.T_min", "kelvin");
      m_T_max   = m_config->get_number("surface.elevation_dependent.T_max", "kelvin");
      m_z_T_min = m_config->get_number("surface.elevation_dependent.z_T_min");
      m_z_T_max = m_config->get_number("surface.elevation_dependent.z_T_max");
    }

    // climatic mass balance
    {
      m_M_min   = m_config->get_number("surface.elevation_dependent.M_min", "m s^-1");
      m_M_max   = m_config->get_number("surface.elevation_dependent.M_max", "m s^-1");
      m_z_M_min = m_config->get_number("surface.elevation_dependent.z_M_min");
      m_z_ELA   = m_config->get_number("surface.elevation_dependent.z_ELA");
      m_z_M_max = m_config->get_number("surface.elevation_dependent.z_M_max");
    }

    // limits of the climatic mass balance
    {
      m_M_limit_min = m_config->get_number("surface.elevation_dependent.M_limit_min", "m s^-1");
      m_M_limit_max = m_config->get_number("surface.elevation_dependent.M_limit_max", "m s^-1");

      double eps = 1e-16;
      if (std::fabs(m_M_limit_min - m_M_limit_max) < eps) {
        m_M_limit_min = m_M_min;
        m_M_limit_max = m_M_max;
      }
    }
  }

  units::Converter meter_per_year(m_sys, "m second^-1", "m year^-1");
  m_log->message(3,
                 "     temperature at %.0f m a.s.l. = %.2f deg C\n"
                 "     temperature at %.0f m a.s.l. = %.2f deg C\n"
                 "     mass balance below %.0f m a.s.l. = %.2f m year-1\n"
                 "     mass balance at  %.0f m a.s.l. = %.2f m year-1\n"
                 "     mass balance at  %.0f m a.s.l. = %.2f m year-1\n"
                 "     mass balance above %.0f m a.s.l. = %.2f m year-1\n"
                 "     equilibrium line altitude z_ELA = %.2f m a.s.l.\n",
                 m_z_T_min, m_T_min,
                 m_z_T_max, m_T_max,
                 m_z_M_min, meter_per_year(m_M_limit_min),
                 m_z_M_min, m_M_min,
                 m_z_M_max, meter_per_year(m_M_max),
                 m_z_M_max, meter_per_year(m_M_limit_max),
                 m_z_ELA);

  // parameterizing the ice surface temperature 'ice_surface_temp'
  m_log->message(2, "    - parameterizing the ice surface temperature 'ice_surface_temp' ... \n");
  m_log->message(2,
                 "      ice temperature at the ice surface (T = ice_surface_temp) is piecewise-linear function\n"
                 "        of surface altitude (usurf):\n"
                 "                 /  %2.2f K                            for            usurf < %.f m\n"
                 "            T = |   %5.2f K + %5.3f * (usurf - %.f m) for   %.f m < usurf < %.f m\n"
                 "                 \\  %5.2f K                            for   %.f m < usurf\n",
                 m_T_min, m_z_T_min,
                 m_T_min, (m_T_max-m_T_min)/(m_z_T_max-m_z_T_min), m_z_T_min, m_z_T_min, m_z_T_max,
                 m_T_max, m_z_T_max);

  // parameterizing the ice surface mass balance 'climatic_mass_balance'
  m_log->message(2,

                 "    - parameterizing the ice surface mass balance 'climatic_mass_balance' ... \n");

  if (limits_set) {
    m_log->message(2,
                   "    - option '-climatic_mass_balance_limits' seen, limiting upper and lower bounds ... \n");
  }

  m_log->message(2,
                 "      surface mass balance (M = climatic_mass_balance) is piecewise-linear function\n"
                 "        of surface altitue (usurf):\n"
                 "                  /  %5.2f m year-1                       for          usurf < %3.f m\n"
                 "             M = |    %5.3f 1/a * (usurf-%.0f m)     for %3.f m < usurf < %3.f m\n"
                 "                  \\   %5.3f 1/a * (usurf-%.0f m)     for %3.f m < usurf < %3.f m\n"
                 "                   \\ %5.2f m year-1                       for %3.f m < usurf\n",
                 meter_per_year(m_M_limit_min), m_z_M_min,
                 meter_per_year(-m_M_min)/(m_z_ELA - m_z_M_min), m_z_ELA, m_z_M_min, m_z_ELA,
                 meter_per_year(m_M_max)/(m_z_M_max - m_z_ELA), m_z_ELA, m_z_ELA, m_z_M_max,
                 meter_per_year(m_M_limit_max), m_z_M_max);
}

MaxTimestep Elevation::max_timestep_impl(double t) const {
  (void) t;
  return MaxTimestep("surface 'elevation'");
}

void Elevation::update_impl(const Geometry &geometry, double t, double dt) {
  (void) t;
  (void) dt;

  compute_mass_flux(geometry.ice_surface_elevation, *m_mass_flux);
  compute_temperature(geometry.ice_surface_elevation, *m_temperature);

  dummy_accumulation(*m_mass_flux, *m_accumulation);
  dummy_melt(*m_mass_flux, *m_melt);
  dummy_runoff(*m_mass_flux, *m_runoff);
  
}

const array::Scalar &Elevation::mass_flux_impl() const {
  return *m_mass_flux;
}

const array::Scalar &Elevation::temperature_impl() const {
  return *m_temperature;
}

const array::Scalar &Elevation::accumulation_impl() const {
  return *m_accumulation;
}

const array::Scalar &Elevation::melt_impl() const {
  return *m_melt;
}

const array::Scalar &Elevation::runoff_impl() const {
  return *m_runoff;
}
  
void Elevation::compute_mass_flux(const array::Scalar &surface, array::Scalar &result) const {
  double dabdz = -m_M_min/(m_z_ELA - m_z_M_min);
  double dacdz = m_M_max/(m_z_M_max - m_z_ELA);

  array::AccessScope list{&result, &surface};

  ParallelSection loop(m_grid->com);
  try {
    for (auto p = m_grid->points(); p; p.next()) {
      const int i = p.i(), j = p.j();

      double z = surface(i, j);

      if (z < m_z_M_min) {
        result(i, j) = m_M_limit_min;
      }
      else if ((z >= m_z_M_min) and (z < m_z_ELA)) {
        result(i, j) = dabdz * (z - m_z_ELA);
      }
      else if ((z >= m_z_ELA) and (z <= m_z_M_max)) {
        result(i, j) = dacdz * (z - m_z_ELA);
      }
      else if (z > m_z_M_max) {
        result(i, j) = m_M_limit_max;
      }
      else {
        throw RuntimeError(PISM_ERROR_LOCATION,
                           "Elevation::compute_mass_flux: HOW DID I GET HERE?");
      }
    }
  } catch (...) {
    loop.failed();
  }
  loop.check();

  // convert from m second-1 ice equivalent to kg m-2 s-1:
  result.scale(m_config->get_number("constants.ice.density"));
}

void Elevation::compute_temperature(const array::Scalar &surface, array::Scalar &result) const {

  array::AccessScope list{&result, &surface};

  double dTdz = (m_T_max - m_T_min)/(m_z_T_max - m_z_T_min);
  ParallelSection loop(m_grid->com);
  try {
    for (auto p = m_grid->points(); p; p.next()) {
      const int i = p.i(), j = p.j();

      double z = surface(i, j);

      if (z <= m_z_T_min) {
        result(i, j) = m_T_min;
      }
      else if ((z > m_z_T_min) and (z < m_z_T_max)) {
        result(i, j) = m_T_min + dTdz * (z - m_z_T_min);
      }
      else if (z >= m_z_T_max) {
        result(i, j) = m_T_max;
      }
      else {
        throw RuntimeError(PISM_ERROR_LOCATION,
                           "Elevation::compute_temperature: HOW DID I GET HERE?");
      }
    }
  } catch (...) {
    loop.failed();
  }
  loop.check();
}

} // end of namespace surface
} // end of namespace pism
