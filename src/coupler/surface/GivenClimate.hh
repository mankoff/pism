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

#ifndef _PSGIVEN_H_
#define _PSGIVEN_H_

#include "pism/coupler/SurfaceModel.hh"
#include "pism/util/iceModelVec2T.hh"

namespace pism {
namespace surface {

class Given : public SurfaceModel {
public:
  Given(IceGrid::ConstPtr g, std::shared_ptr<atmosphere::AtmosphereModel> input);
  virtual ~Given() = default;
protected:
  void init_impl(const Geometry &geometry);
  void update_impl(const Geometry &geometry, double t, double dt);

  const IceModelVec2S &temperature_impl() const;
  const IceModelVec2S &mass_flux_impl() const;

  const IceModelVec2S& accumulation_impl() const;
  const IceModelVec2S& melt_impl() const;
  const IceModelVec2S& runoff_impl() const;

  void define_model_state_impl(const File &output) const;
  void write_model_state_impl(const File &output) const;

  std::shared_ptr<IceModelVec2T> m_mass_flux;
  std::shared_ptr<IceModelVec2T> m_temperature;
};

} // end of namespace surface
} // end of namespace pism

#endif /* _PSGIVEN_H_ */
