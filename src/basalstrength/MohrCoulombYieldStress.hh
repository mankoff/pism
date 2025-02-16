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

#ifndef _PISMMOHRCOULOMBYIELDSTRESS_H_
#define _PISMMOHRCOULOMBYIELDSTRESS_H_

#include "YieldStress.hh"

#include "pism/util/iceModelVec.hh"

namespace pism {

class IceModelVec2T;

class IceModelVec2CellType;

//! @brief PISM's default basal yield stress model which applies the
//! Mohr-Coulomb model of deformable, pressurized till.
class MohrCoulombYieldStress : public YieldStress {
public:
  MohrCoulombYieldStress(IceGrid::ConstPtr g);
  virtual ~MohrCoulombYieldStress() = default;

  void set_till_friction_angle(const IceModelVec2S &input);
protected:
  void restart_impl(const File &input_file, int record);
  void bootstrap_impl(const File &input_file, const YieldStressInputs &inputs);
  void init_impl(const YieldStressInputs &inputs);

  void define_model_state_impl(const File &output) const;
  void write_model_state_impl(const File &output) const;

  DiagnosticList diagnostics_impl() const;

  MaxTimestep max_timestep_impl(double t) const;
  void update_impl(const YieldStressInputs &inputs, double t, double dt);

  void finish_initialization(const YieldStressInputs &inputs);

  IceModelVec2S m_till_phi;

  std::shared_ptr<IceModelVec2T> m_delta;
private:
  void till_friction_angle(const IceModelVec2S &bed_topography,
                           IceModelVec2S &result);

  void till_friction_angle(const IceModelVec2S &basal_yield_stress,
                           const IceModelVec2S &till_water_thickness,
                           const IceModelVec2S &ice_thickness,
                           const IceModelVec2CellType &cell_type,
                           IceModelVec2S &result);
};

} // end of namespace pism

#endif /* _PISMMOHRCOULOMBYIELDSTRESS_H_ */
