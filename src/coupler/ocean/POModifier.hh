// Copyright (C) 2011, 2013, 2014 PISM Authors
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

#ifndef _POMODIFIER_H_
#define _POMODIFIER_H_

#include "PISMOcean.hh"

namespace pism {

class POModifier : public Modifier<OceanModel>
{
public:
  POModifier(const IceGrid &g, OceanModel* in)
    : Modifier<OceanModel>(g, in) {}
  virtual ~POModifier() {}

  virtual void sea_level_elevation(double &result)
  {
    input_model->sea_level_elevation(result);
  }

  virtual void shelf_base_temperature(IceModelVec2S &result)
  {
    input_model->shelf_base_temperature(result);
  }

  virtual void shelf_base_mass_flux(IceModelVec2S &result)
  {
    input_model->shelf_base_mass_flux(result);
  }

  virtual void melange_back_pressure_fraction(IceModelVec2S &result)
  {
    input_model->melange_back_pressure_fraction(result);
  }
};

} // end of namespace pism

#endif /* _POMODIFIER_H_ */
