// Copyright (C) 2018, 2021 PISM Authors
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

#ifndef _SEA_LEVEL_FACTORY_
#define _SEA_LEVEL_FACTORY_

#include "pism/coupler/util/PCFactory.hh"
#include "pism/coupler/SeaLevel.hh"

namespace pism {
namespace ocean {
namespace sea_level {

class Factory : public PCFactory<ocean::sea_level::SeaLevel> {
public:
  Factory(IceGrid::ConstPtr grid);
  ~Factory() = default;
};

} // end of namespace sea_level
} // end of namespace ocean
} // end of namespace pism

#endif /* _SEA_LEVEL_FACTORY_ */
