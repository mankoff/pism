// Copyright (C) 2004-2017, 2019, 2021, 2023, 2024 Jed Brown, Ed Bueler, and Constantine Khroulev
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

#include <cmath>                // pow, sqrt

#include "pism/basalstrength/basal_resistance.hh"

#include "pism/util/ConfigInterface.hh"
#include "pism/util/Logger.hh"

namespace pism {

static double square(double x) {
  return x * x;
}

/* Purely plastic */

IceBasalResistancePlasticLaw::IceBasalResistancePlasticLaw(const Config &config) {
  m_plastic_regularize = config.get_number("basal_resistance.plastic.regularization", "m second-1");
}

void IceBasalResistancePlasticLaw::print_info(const Logger &log,
                                              int threshold,
                                              units::System::Ptr system) const {
  log.message(threshold, "Using purely plastic till with eps = %10.5e m year-1.\n",
              units::convert(system, m_plastic_regularize, "m second^-1", "m year^-1"));
}


//! Compute the drag coefficient for the basal shear stress.
double IceBasalResistancePlasticLaw::drag(double tauc, double vx, double vy) const {
  const double magreg2 = square(m_plastic_regularize) + square(vx) + square(vy);

  return tauc / sqrt(magreg2);
}

//! Compute the drag coefficient and its derivative with respect to \f$ \alpha = \frac 1 2 (u_x^2 + u_y^2) \f$
/**
 * @f{align*}{
 * \beta &= \frac{\tau_{c}}{|\mathbf{u}|} = \tau_{c}\cdot (|\mathbf{u}|^{2})^{-1/2}\\
 * \diff{\beta}{\frac12 |\mathbf{u}|^{2}} &= -\frac{1}{|\mathbf{u}|^{2}}\cdot \beta
 * @f}
 */
void IceBasalResistancePlasticLaw::drag_with_derivative(double tauc, double vx, double vy,
                                                        double *beta, double *dbeta) const {
  const double magreg2 = square(m_plastic_regularize) + square(vx) + square(vy);

  *beta = tauc / sqrt(magreg2);

  if (dbeta) {
    *dbeta = -1 * (*beta) / magreg2;
  }
}

/* Pseudo-plastic */

IceBasalResistancePseudoPlasticLaw::IceBasalResistancePseudoPlasticLaw(const Config &config)
  : IceBasalResistancePlasticLaw(config) {
  m_q = config.get_number("basal_resistance.pseudo_plastic.q");
  m_u_threshold = config.get_number("basal_resistance.pseudo_plastic.u_threshold", "m second-1");
  m_sliding_scale_factor_reduces_tauc = config.get_number("basal_resistance.pseudo_plastic.sliding_scale_factor");

  m_u_threshold_factor = pow(m_u_threshold, -m_q);
}

void IceBasalResistancePseudoPlasticLaw::print_info(const Logger &log,
                                                    int threshold,
                                                    units::System::Ptr system) const {

  if (m_q == 1.0) {
    log.message(threshold,
                 "Using linearly viscous till with u_threshold = %.2f m year-1.\n",
                 units::convert(system, m_u_threshold, "m second^-1", "m year^-1"));
  } else {
    log.message(threshold,
                 "Using pseudo-plastic till with eps = %10.5e m year-1, q = %.4f,"
                 " and u_threshold = %.2f m year-1.\n",
                 units::convert(system, m_plastic_regularize, "m second^-1", "m year^-1"),
                 m_q,
                 units::convert(system, m_u_threshold, "m second^-1", "m year^-1"));
  }
}

//! Compute the drag coefficient for the basal shear stress.
/*!

  The basal shear stress term  @f$ \tau_b @f$  in the SSA stress balance
  for ice is minus the return value here times (vx,vy). Thus this
  method computes the basal shear stress as

  @f[ \tau_b = - \frac{\tau_c}{|\mathbf{U}|^{1-q} U_{\mathtt{th}}^q} \mathbf{U} @f]

  where  @f$ \tau_b=(\tau_{(b)x},\tau_{(b)y}) @f$ ,  @f$ U=(u,v) @f$ ,
   @f$ q= @f$  `pseudo_q`, and  @f$ U_{\mathtt{th}}= @f$
  `pseudo_u_threshold`. Typical values for the constants are
   @f$ q=0.25 @f$  and  @f$ U_{\mathtt{th}} = 100 @f$  m year-1.

  The linearly-viscous till case pseudo_q = 1.0 is allowed, in which
  case  @f$ \beta = \tau_c/U_{\mathtt{th}} @f$ . The purely-plastic till
  case pseudo_q = 0.0 is also allowed; note that there is still a
  regularization with data member plastic_regularize.

  One can scale tauc if desired:

  A scale factor of  @f$ A @f$  is intended to increase basal sliding rate
  by  @f$ A @f$ . It would have exactly this effect \e if the driving
  stress were \e hypothetically completely held by the basal
  resistance. Thus this scale factor is used to reduce (if
  `-sliding_scale_factor_reduces_tauc`  @f$ A @f$  with  @f$ A > 1 @f$) or increase (if  @f$ A <
  1 @f$) the value of the (pseudo-) yield stress `tauc`. The concept
  behind this is described at
  [the SeaRISE wiki](https://web.archive.org/web/20120126164914/http://websrv.cs.umt.edu/isis/index.php/Category_1:_Whole_Ice_Sheet#Initial_Experiment_-_E1_-_Increased_Basal_Lubrication).

  Specifically, the concept behind this mechanism is to suppose
  equality of driving and basal shear stresses,

  @f[ \rho g H \nabla h = \frac{\tau_c}{|\mathbf{U}|^{1-q} U_{\mathtt{th}}^q} \mathbf{U}. @f]

  (*For emphasis:* The membrane stress held by the ice itself is
  missing from this incomplete stress balance.) Thus the pseudo yield
  stress  @f$ \tau_c @f$  would be related to the sliding speed
   @f$ |\mathbf{U}| @f$  by

  @f[ |\mathbf{U}| = \frac{C}{\tau_c^{1/q}} \f]

  for some (geometry-dependent) constant  @f$ C @f$ . Multiplying
   @f$ |\mathbf{U}| @f$  by  @f$ A @f$  in this equation corresponds to
  dividing  @f$ \tau_c @f$  by  @f$ A^q @f$ .

  Note that the mechanism has no effect whatsoever if  @f$ q=0 @f$ , which
  is the purely plastic case. In that case there is \e no direct
  relation between the yield stress and the sliding velocity, and the
  difference between the driving stress and the yield stress is
  entirely held by the membrane stresses. (There is also no singular
  mathematical operation as  @f$ A^q = A^0 = 1 @f$ .)
*/
double IceBasalResistancePseudoPlasticLaw::drag(double tauc, double vx, double vy) const {
  const double magreg2 = square(m_plastic_regularize) + square(vx) + square(vy);

  double Aq = 1.0;

  if (m_sliding_scale_factor_reduces_tauc > 0.0) {
    Aq = pow(m_sliding_scale_factor_reduces_tauc, m_q);
  }

  return (tauc / Aq) * pow(magreg2, 0.5*(m_q - 1)) * m_u_threshold_factor;
}


//! Compute the drag coefficient and its derivative with respect to @f$ \alpha = \frac 1 2 (u_x^2 + u_y^2) @f$
/**
 * @f{align*}{
 * \beta &= \frac{\tau_{c}}{u_{\text{threshold}}^q}\cdot (|u|^{2})^{\frac{q-1}{2}} \\
 * \diff{\beta}{\frac12 |\mathbf{u}|^{2}} &= \frac{\tau_{c}}{u_{\text{threshold}}^q}\cdot \frac{q-1}{2}\cdot (|\mathbf{u}|^{2})^{\frac{q-1}{2} - 1}\cdot 2 \\
 * &= \frac{q-1}{|\mathbf{u}|^{2}}\cdot \beta(\mathbf{u}) \\
 * @f}
 */
void IceBasalResistancePseudoPlasticLaw::drag_with_derivative(double tauc, double vx, double vy,
                                                              double *beta, double *dbeta) const
{
  const double magreg2 = square(m_plastic_regularize) + square(vx) + square(vy);

  if (m_sliding_scale_factor_reduces_tauc > 0.0) {
    double Aq = pow(m_sliding_scale_factor_reduces_tauc, m_q);
    *beta = (tauc / Aq) * pow(magreg2, 0.5*(m_q - 1)) * m_u_threshold_factor;
  } else {
    *beta =  tauc * pow(magreg2, 0.5*(m_q - 1)) * m_u_threshold_factor;
  }

  if (dbeta) {
    *dbeta = (m_q - 1) * (*beta) / magreg2;
  }

}

/* Regularized Coulomb */

IceBasalResistanceRegularizedLaw::IceBasalResistanceRegularizedLaw(const Config &config)
  : IceBasalResistancePlasticLaw(config) {
  m_q = config.get_number("basal_resistance.pseudo_plastic.q");
  m_u_threshold = config.get_number("basal_resistance.pseudo_plastic.u_threshold", "m second-1");
  m_sliding_scale_factor_reduces_tauc = config.get_number("basal_resistance.pseudo_plastic.sliding_scale_factor");
}

void IceBasalResistanceRegularizedLaw::print_info(const Logger &log,
                                                  int threshold,
                                                  units::System::Ptr system) const {
  log.message(threshold,
              "Using regularized Coulomb till with eps = %10.5e m year-1, q = %.4f,"
              " and u_threshold = %.2f m year-1.\n",
              units::convert(system, m_plastic_regularize, "m second^-1", "m year^-1"),
              m_q,
              units::convert(system, m_u_threshold, "m second^-1", "m year^-1"));
}

double IceBasalResistanceRegularizedLaw::drag(double tauc, double vx, double vy) const {
  const double magreg2sqr = sqrt(square(m_plastic_regularize) + square(vx) + square(vy));

  if (m_sliding_scale_factor_reduces_tauc > 0.0) {
    double Aq = pow(m_sliding_scale_factor_reduces_tauc, m_q);
    return (tauc / Aq) * pow(magreg2sqr, (m_q - 1)) * pow((magreg2sqr + m_u_threshold), -m_q);
  } else {
    return tauc * pow(magreg2sqr, (m_q - 1)) * pow((magreg2sqr + m_u_threshold), -m_q);
  }
}

//! Compute the drag coefficient and its derivative with respect to @f$ \alpha = \frac 1 2 (u_x^2 + u_y^2) @f$
/**
 * @f{align*}{
 * \beta &= \frac{\tau_{c}}{ \left( (|u|^{2})^{\frac12} + u_{\text{threshold}} \right)^{q} }\cdot (|\mathbf{u}|^{2})^{\frac{q-1}{2}} \\
 * \diff{\beta}{\frac12 |\mathbf{u}|^{2}} &= \frac{\tau_{c}}{ \left( (|\mathbf{u}|^{2})^{\frac12} + u_{\text{threshold}}\right)^{q} }\cdot \frac{q-1}{2}\cdot (|\mathbf{u}|^{2})^{\frac{q-1}{2} - 1}\cdot 2  -  \frac{\tau_{c}}{ \left( (|\mathbf{u}|^{2})^{\frac12} + u_{\text{threshold}}\right)^{q+1} }\cdot \frac{2 \cdot q }{(|\mathbf{u}|^{2})^{\frac12}} \cdot (|\mathbf{u}|^{2})^{\frac{q-1}{2} }    \\
 * &= \left( \frac{q-1}{|\mathbf{u}|^{2}} - \frac{q}{|\mathbf{u}| \cdot ( |\mathbf{u}| + u_{\text{threshold}}) }    \right) \cdot \beta(\mathbf{u})\\
 * @f}
 * Same parameters are used as in the pseudo-plastic case, namely @f$ q, u_{\text{threshold}}, A_q @f$.
 * It should be noted, that in the original equation (3) in Zoet et al, 2020 the exponent @f$ q=1/p @f$ is used.
 */
void IceBasalResistanceRegularizedLaw::drag_with_derivative(double tauc, double vx, double vy,
                                                            double *beta, double *dbeta) const {
  const double magreg2    = square(m_plastic_regularize) + square(vx) + square(vy),
               magreg2sqr = sqrt(magreg2);

  if (m_sliding_scale_factor_reduces_tauc > 0.0) {
    double Aq = pow(m_sliding_scale_factor_reduces_tauc, m_q);
    *beta = (tauc / Aq) * pow(magreg2sqr, (m_q - 1)) * pow((magreg2sqr + m_u_threshold), -m_q);
  } else {
    *beta =  tauc * pow(magreg2sqr, (m_q - 1)) * pow((magreg2sqr + m_u_threshold), -m_q);
  }

  if (dbeta) {
    *dbeta = (((m_q - 1) / magreg2) - (m_q / (magreg2sqr * (magreg2sqr + m_u_threshold)))) * (*beta);
  }
}


} // end of namespace pism
