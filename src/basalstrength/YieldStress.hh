// Copyright (C) 2004--2012, 2014, 2015, 2016, 2017, 2018, 2019, 2021 Jed Brown, Ed Bueler and Constantine Khroulev
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

#ifndef _PISMYIELDSTRESS_H_
#define _PISMYIELDSTRESS_H_

#include "pism/util/Component.hh"
#include "pism/util/iceModelVec.hh"

namespace pism {

class Geometry;

class YieldStressInputs {
public:
  YieldStressInputs();

  const Geometry *geometry;

  const IceModelVec2S *till_water_thickness;

  const IceModelVec2S *subglacial_water_thickness;

  // inputs used by regional models
  const IceModelVec2Int *no_model_mask;
};

//! \brief The PISM basal yield stress model interface (virtual base class)
class YieldStress : public Component {
public:
  YieldStress(IceGrid::ConstPtr g);
  virtual ~YieldStress() = default;

  void restart(const File &input_file, int record);

  void bootstrap(const File &input_file, const YieldStressInputs &inputs);

  void init(const YieldStressInputs &inputs);

  void update(const YieldStressInputs &inputs, double t, double dt);

  const IceModelVec2S& basal_material_yield_stress();

  std::string name() const;
protected:
  virtual void restart_impl(const File &input_file, int record) = 0;

  virtual void bootstrap_impl(const File &input_file, const YieldStressInputs &inputs) = 0;

  virtual void init_impl(const YieldStressInputs &inputs) = 0;

  virtual void update_impl(const YieldStressInputs &inputs, double t, double dt) = 0;

  virtual void define_model_state_impl(const File &output) const;

  virtual void write_model_state_impl(const File &output) const;

  DiagnosticList diagnostics_impl() const;

  IceModelVec2S m_basal_yield_stress;

  std::string m_name;
};

} // end of namespace pism

#endif /* _PISMYIELDSTRESS_H_ */
