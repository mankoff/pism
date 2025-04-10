// Copyright (C) 2010--2018, 2021, 2022, 2024 Ed Bueler, Constantine Khroulev, and David Maxwell
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

static char help[] =
  "\nSSA_TESTJ\n"
  "  Testing program for the finite element implementation of the SSA.\n"
  "  Does a time-independent calculation.  Does not run IceModel or a derived\n"
  "  class thereof. Uses verification test J. Also may be used in a PISM\n"
  "  software (regression) test.\n\n";

#include <petscsys.h>

#include "pism/stressbalance/ssa/SSATestCase.hh"
#include "pism/util/Mask.hh"
#include "pism/util/Context.hh"
#include "pism/util/error_handling.hh"
#include "pism/util/petscwrappers/PetscInitializer.hh"
#include "pism/util/pism_options.hh"
#include "pism/verification/tests/exactTestsIJ.h"

namespace pism {
namespace stressbalance {

std::shared_ptr<Grid> ssa_test_j_grid(std::shared_ptr<Context> ctx, int Mx, int My) {
  return SSATestCase::grid(ctx, Mx, My, 300e3, 300e3, grid::CELL_CENTER, grid::XY_PERIODIC);
}

class SSATestCaseJ: public SSATestCase
{
public:
  SSATestCaseJ(std::shared_ptr<SSA> ssa) : SSATestCase(ssa) {

    EnthalpyConverter EC(*m_config);
    // 0.01 water fraction
    m_ice_enthalpy.set(EC.enthalpy(273.15, 0.01, 0.0));
  }

protected:
  virtual void initializeSSACoefficients();

  virtual void exactSolution(int i, int j,
                             double x, double y, double *u, double *v);
};

void SSATestCaseJ::initializeSSACoefficients() {
  m_tauc.set(0.0);    // irrelevant for test J
  m_geometry.bed_elevation.set(-1000.0); // assures shelf is floating (maximum ice thickness is 770 m)
  m_geometry.cell_type.set(MASK_FLOATING);

  /* use Ritz et al (2001) value of 30 MPa year for typical vertically-averaged viscosity */
  double ocean_rho = m_config->get_number("constants.sea_water.density"),
    ice_rho = m_config->get_number("constants.ice.density");
  const double nu0 = units::convert(m_sys, 30.0, "MPa year", "Pa s"); /* = 9.45e14 Pa s */
  const double H0 = 500.;       /* 500 m typical thickness */

  // Test J has a viscosity that is independent of velocity.  So we force a
  // constant viscosity by settting the strength_extension
  // thickness larger than the given ice thickness. (max = 770m).
  m_ssa->strength_extension->set_notional_strength(nu0 * H0);
  m_ssa->strength_extension->set_min_thickness(800);

  array::AccessScope list{&m_geometry.ice_thickness, &m_geometry.ice_surface_elevation, &m_bc_mask, &m_bc_values};

  for (auto p = m_grid->points(); p; p.next()) {
    const int i = p.i(), j = p.j();

    const double myx = m_grid->x(i), myy = m_grid->y(j);

    // set H,h on regular grid
    struct TestJParameters J_parameters = exactJ(myx, myy);

    m_geometry.ice_thickness(i,j) = J_parameters.H;
    m_geometry.ice_surface_elevation(i,j) = (1.0 - ice_rho / ocean_rho) * J_parameters.H; // FIXME issue #15

    // special case at center point: here we set bc_values at (i,j) by
    // setting bc_mask and bc_values appropriately
    if ((i == ((int)m_grid->Mx()) / 2) and
        (j == ((int)m_grid->My()) / 2)) {
      m_bc_mask(i,j) = 1;
      m_bc_values(i,j).u = J_parameters.u;
      m_bc_values(i,j).v = J_parameters.v;
    }
  }

  // communicate what we have set
  m_geometry.ice_surface_elevation.update_ghosts();
  m_geometry.ice_thickness.update_ghosts();
  m_bc_mask.update_ghosts();
  m_bc_values.update_ghosts();
}

void SSATestCaseJ::exactSolution(int /*i*/, int /*j*/,
                                 double x, double y,
                                 double *u, double *v) {
  struct TestJParameters J_parameters = exactJ(x, y);
  *u = J_parameters.u;
  *v = J_parameters.v;
}

} // end of namespace stressbalance
} // end of namespace pism

int main(int argc, char *argv[]) {

  using namespace pism;
  using namespace pism::stressbalance;

  MPI_Comm com = MPI_COMM_WORLD;
  petsc::Initializer petsc(argc, argv, help);

  com = PETSC_COMM_WORLD;

  /* This explicit scoping forces destructors to be called before PetscFinalize() */
  try {
    std::shared_ptr<Context> ctx = context_from_options(com, "ssa_testj");
    Config::Ptr config = ctx->config();

    std::string usage = "\n"
      "usage of SSA_TESTJ:\n"
      "  run ssafe_test -Mx <number> -My <number> -ssa_method <fd|fem>\n"
      "\n";

    bool stop = show_usage_check_req_opts(*ctx->log(), "ssa_testj", {}, usage);

    if (stop) {
      return 0;
    }

    // Parameters that can be overridden by command line options

    unsigned int Mx = config->get_number("grid.Mx");
    unsigned int My = config->get_number("grid.My");

    auto method = config->get_string("stress_balance.ssa.method");
    auto output_file = config->get_string("output.file");

    bool write_output = config->get_string("output.size") != "none";

    config->set_flag("basal_resistance.pseudo_plastic.enabled", false);
    config->set_string("stress_balance.ssa.flow_law", "isothermal_glen");

    auto grid = ssa_test_j_grid(ctx, Mx, My);
    SSATestCaseJ testcase(SSATestCase::solver(grid, method));
    testcase.init();
    testcase.run();
    testcase.report("J");
    if (write_output) {
      testcase.write(output_file);
    }
  }
  catch (...) {
    handle_fatal_errors(com);
    return 1;
  }

  return 0;
}
