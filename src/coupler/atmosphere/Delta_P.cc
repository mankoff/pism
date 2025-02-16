// Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2021 PISM Authors
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

#include "Delta_P.hh"

#include "pism/util/ConfigInterface.hh"
#include "pism/util/ScalarForcing.hh"

namespace pism {
namespace atmosphere {

Delta_P::Delta_P(IceGrid::ConstPtr grid, std::shared_ptr<AtmosphereModel> in)
  : AtmosphereModel(grid, in) {

  m_forcing.reset(new ScalarForcing(*grid->ctx(),
                                    "atmosphere.delta_P",
                                    "delta_P",
                                    "kg m-2 second-1",
                                    "kg m-2 year-1",
                                    "precipitation offsets"));

  m_precipitation = allocate_precipitation(grid);
}

void Delta_P::init_impl(const Geometry &geometry) {
  m_input_model->init(geometry);

  m_log->message(2,
                 "* Initializing precipitation forcing using scalar offsets...\n");
}

void Delta_P::init_timeseries_impl(const std::vector<double> &ts) const {
  AtmosphereModel::init_timeseries_impl(ts);

  m_offset_values.resize(ts.size());
  for (unsigned int k = 0; k < ts.size(); ++k) {
    m_offset_values[k] = m_forcing->value(ts[k]);
  }
}

void Delta_P::update_impl(const Geometry &geometry, double t, double dt) {
  m_input_model->update(geometry, t, dt);

  m_precipitation->copy_from(m_input_model->mean_precipitation());
  m_precipitation->shift(m_forcing->value(t + 0.5 * dt));
}

const IceModelVec2S& Delta_P::mean_precipitation_impl() const {
  return *m_precipitation;
}

void Delta_P::precip_time_series_impl(int i, int j, std::vector<double> &result) const {
  m_input_model->precip_time_series(i, j, result);
  
  for (unsigned int k = 0; k < m_offset_values.size(); ++k) {
    result[k] += m_offset_values[k];
  }
}

} // end of namespace atmosphere
} // end of namespace pism
