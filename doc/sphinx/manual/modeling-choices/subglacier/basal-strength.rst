.. include:: ../../../global.txt

.. _sec-basestrength:

Controlling basal strength
--------------------------

.. contents::

When using option :opt:`-stress_balance ssa+sia`, the SIA+SSA hybrid stress balance, a
model for basal resistance is required. This model for basal resistance is based, at least
conceptually, on the hypothesis that the ice sheet is underlain by a layer of till
:cite:`Clarke05`. The user can control the parts of this model:

- the so-called sliding law, typically a power law, which relates the ice base (sliding)
  velocity to the basal shear stress, and which has a coefficient which is or has the
  units of a yield stress,
- the model relating the effective pressure on the till layer to the yield stress of that
  layer, and
- the model for relating the amount of water stored in the till to the effective pressure
  on the till.

This subsection explains the relevant options.

The primary example of ``-stress_balance ssa+sia`` usage is in section :ref:`sec-start` of
this Manual, but the option is also used in sections :ref:`sec-MISMIP`,
:ref:`sec-MISMIP3d`, and :ref:`sec-jako`.

In PISM the key coefficient in the sliding is always denoted as yield stress `\tau_c`,
which is :var:`tauc` in PISM output files. This parameter represents the strength of the
aggregate material at the base of an ice sheet, a poorly-observed mixture of pressurized
liquid water, ice, granular till, and bedrock bumps. The yield stress concept also extends
to the power law form, and thus most standard sliding laws can be chosen by user options
(below). One reason that the yield stress is a useful parameter is that it can be
compared, when looking at PISM output files, to the driving stress (:var:`taud_mag` in
PISM output files). Specifically, where :var:`tauc` `<` :var:`taud_mag` you are likely to
see sliding if option ``-stress_balance ssa+sia`` is used.

A historical note on modeling basal sliding is in order. Sliding can be added directly to
a SIA stress balance model by making the sliding velocity a local function of the basal
value of the driving stress. Such an SIA sliding mechanism appears in ISMIP-HEINO
:cite:`Calovetal2009HEINOfinal` and in EISMINT II experiment H :cite:`EISMINT00`, among
other places. This kind of sliding is *not* recommended, as it does not make sense to
regard the driving stress as the local generator of flow if the bed is not holding all of
that stress :cite:`BBssasliding`, :cite:`Fowler01`. Within PISM, for historical reasons,
there is an implementation of SIA-based sliding only for verification test E; see section
:ref:`sec-verif`. PISM does *not* support this SIA-based sliding mode in other contexts.

Choosing the sliding law
^^^^^^^^^^^^^^^^^^^^^^^^

In PISM the sliding law can be chosen to be a purely-plastic (Coulomb) model, namely,

.. math::
   :label: eq-plastic

   |\boldsymbol{\tau}_b| \le \tau_c \quad \text{and} \quad \boldsymbol{\tau}_b =
   - \tau_c \frac{\mathbf{u}}{|\mathbf{u}|} \quad\text{if and only if}\quad |\mathbf{u}| > 0.

Equation :eq:`eq-plastic` says that the (vector) basal shear stress `\boldsymbol{\tau}_b`
is at most the yield stress `\tau_c`, and that only when the shear stress reaches the
yield value can there be sliding. The sliding law can, however, also be chosen to be the
power law

.. math::
   :label: eq-pseudoplastic

   \boldsymbol{\tau}_b =  - \tau_c \frac{\mathbf{u}}{u_{\text{threshold}}^q |\mathbf{u}|^{1-q}},

where `u_{\text{threshold}}` is a parameter with units of velocity (see below). Condition
:eq:`eq-plastic` is studied in :cite:`SchoofStream` and :cite:`SchoofTill` in particular,
while power laws for sliding are common across the glaciological literature (e.g. see
:cite:`CuffeyPaterson`, :cite:`GreveBlatter2009`). Notice that the coefficient `\tau_c` in
:eq:`eq-pseudoplastic` has units of stress, regardless of the power `q`.

In both of the above equations :eq:`eq-plastic` and :eq:`eq-pseudoplastic` we call
`\tau_c` the *yield stress*. It corresponds to the variable :var:`tauc` in PISM output files.
We call the power law :eq:`eq-pseudoplastic` a "pseudo-plastic" law with power `q` and
threshold velocity `u_{\text{threshold}}`. At the threshold velocity the basal shear
stress `\boldsymbol{\tau}_b` has exact magnitude `\tau_c`. In equation
:eq:`eq-pseudoplastic`, `q` is the power controlled by ``-pseudo_plastic_q``, and the
threshold velocity `u_{\text{threshold}}` is controlled by ``-pseudo_plastic_uthreshold``.
The plastic model :eq:`eq-plastic` is the `q=0` case of :eq:`eq-pseudoplastic`.

See :numref:`tab-sliding-power-law` for options controlling the choice of sliding law. The
purely plastic case is the default; just use ``-stress_balance ssa+sia`` to turn it on.
(Or use ``-stress_balance ssa`` if a model with no vertical shear is desired.)

.. warning::

   Options ``-pseudo_plastic_q`` and ``-pseudo_plastic_uthreshold`` have no effect if
   ``-pseudo_plastic`` is not set.

.. list-table:: Sliding law command-line options
   :name: tab-sliding-power-law
   :header-rows: 1
   :widths: 1,2

   * - Option
     - Description
   * - :opt:`-pseudo_plastic`
     - Enables the pseudo-plastic power law model. If this is not set the sliding law is
       purely-plastic, so ``pseudo_plastic_q`` and ``pseudo_plastic_uthreshold`` are
       inactive.
   * - :opt:`-plastic_reg` (m/a)
     - Set the value of `\epsilon` regularization of the plastic law, in the formula
       `\boldsymbol{\tau}_b = - \tau_c \mathbf{u}/\sqrt{|\mathbf{u}|^2 + \epsilon^2}`. The
       default is `0.01` m/a. This parameter is inactive if ``-pseudo_plastic`` is set.
   * - :opt:`-pseudo_plastic_q`
     - Set the exponent `q` in :eq:`eq-pseudoplastic`.  The default is `0.25`.
   * - :opt:`-pseudo_plastic_uthreshold` (m/a)
     - Set `u_{\text{threshold}}` in :eq:`eq-pseudoplastic`.  The default is `100` m/a.

Equation :eq:`eq-pseudoplastic` is a very flexible power law form. For example, the linear
case is `q=1`, in which case if `\beta=\tau_c/u_{\text{threshold}}` then the law is of the
form

.. math::
   :label: eq-sliding-linear

   \boldsymbol{\tau}_b = - \beta \mathbf{u}

(The "`\beta`" coefficient is also called `\beta^2` in some sources (see :cite:`MacAyeal`,
for example).) If you want such a linear sliding law, and you have a value
`\beta=` ``beta`` in `\text{Pa}\,\text{s}\,\text{m}^{-1}`, then use this option
combination:

.. code-block:: none

   -pseudo_plastic \
   -pseudo_plastic_q 1.0 \
   -pseudo_plastic_uthreshold 1m/s \
   -yield_stress constant -tauc beta

More generally, it is common in the literature to see power-law sliding relations in the
form

.. math::

   \boldsymbol{\tau}_b = - C |\mathbf{u}|^{m-1} \mathbf{u},

where `C` is a constant, as for example in sections :ref:`sec-MISMIP` and
:ref:`sec-MISMIP3d`. In that case, use this option combination:

.. code-block:: none

   -pseudo_plastic \
   -pseudo_plastic_q m \
   -pseudo_plastic_uthreshold 1m/s \
   -yield_stress constant \
   -tauc C

Another alternative is the slip law by :cite:`ZoetIverson20` that combines processes of
hard-bedded sliding (Coulomb behavior) and viscous bed deformation without required
knowledge of the bed type. It is defined by

.. math::
   :label: eq-regularizedcoulomb

   \boldsymbol{\tau}_b = - \tau_c \frac{\uu}{\left(|\uu| + u_{\text{threshold}} \right)^{q} |\uu|^{1-q}},

Set :opt:`-regularized_coulomb` to select this sliding law.

The original equation (3) in :cite:`ZoetIverson20` uses the exponent `q=1/p`. Otherwise,
same configuration parameters can be used as in the pseudo-plastic case, but they should
have different values, namely `q`\=0.2, `u_{\text{threshold}}`\=40-80 m/year`:

.. code-block:: none

   -regularized_coulomb \
   -pseudo_plastic_q 0.2 \
   -pseudo_plastic_uthreshold 50.0 

The model's performance should be close to the pseudo-plastic implementation (Eq.
:eq:`eq-pseudoplastic`), although there ought to be slightly more fast sliding and a less
diffuse onset of sliding.




.. _sec-lateral-drag:

Lateral drag
~~~~~~~~~~~~

PISM prescribes lateral drag at ice margins next to ground with elevation above the ice.
(This is relevant in outlet glaciers flowing through fjords, valley glaciers, and next to
nunataks.) Set :config:`basal_resistance.beta_lateral_margin` to control the amount of
additional drag at these margins.

Determining the yield stress
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Other than setting it to a constant, which only applies in some special cases, the above
discussion does not determine the yield stress `\tau_c`. As shown in
:numref:`tab-yieldstress`, there are two schemes for determining `\tau_c` in a
spatially-variable manner:

- ``-yield_stress mohr_coulomb`` (the default) determines the yields stress by models of
  till material property (the till friction angle) and of the effective pressure on the
  saturated till, or
- ``-yield_stress constant`` allows the yield stress to be supplied as time-independent
  data, read from the input file.

In normal modelling cases, variations in yield stress are part of the explanation of the
locations of ice streams :cite:`SchoofStream`. The default model ``-yield_stress
mohr_coulomb`` determines these variations in time and space. The value of `\tau_c` is
determined in part by a subglacial hydrology model, including the modeled till-pore water
amount (`W_{till}`, NetCDF variable :var:`tillwat`; see :ref:`sec-subhydro`), which then
determines the effective pressure `N_{till}` (see below). The value of `\tau_c` is also
determined in part by a material property field `\phi`, the "till friction angle" (NetCDF
variable :var:`tillphi`). These quantities are related by the Mohr-Coulomb criterion
:cite:`CuffeyPaterson`:

.. math::
   :label: eq-mohrcoulomb

   \tau_c = c_{0} + (\tan\phi)\,N_{till}.

Here `c_0` is called the "till cohesion", whose default value in PISM is zero (see
:cite:`SchoofStream`, formula (2.4)) but which can be set by option :opt:`-till_cohesion`.

Option combination ``-yield_stress constant -tauc X`` can be used to fix the yield stress
to have value `\tau_c = X` at all grounded locations and all times if desired. This is
unlikely to be a good modelling choice for real ice sheets.

.. list-table:: Command-line options controlling how yield stress is determined
   :name: tab-yieldstress
   :header-rows: 1
   :widths: 3,5

   * - Option
     - Description
   * - :opt:`-yield_stress mohr_coulomb`
     - The default. Use equation :eq:`eq-mohrcoulomb` to determine `\tau_c`. Only
       effective if ``-stress_balance ssa`` or ``-stress_balance ssa+sia`` is also set.
   * - :opt:`-till_cohesion`
     - Set the value of the till cohesion (`c_{0}`) in the plastic till model. The value
       is a pressure, given in Pa.
   * - :opt:`-tauc_slippery_grounding_lines`
     - If set, reduces the basal yield stress at grounded-below-sea-level grid points one
       cell away from floating ice or ocean. Specifically, it replaces the
       normally-computed `\tau_c` from the Mohr-Coulomb relation, which uses the effective
       pressure from the modeled amount of water in the till, by the minimum value of
       `\tau_c` from Mohr-Coulomb, i.e. using the effective pressure corresponding to the
       maximum amount of till-stored water. Does not alter the reported amount of till
       water, nor does this mechanism affect water conservation.
   * - :opt:`-plastic_phi` (degrees)
     - Use a constant till friction angle. The default is `30^{\circ}`.
   * - :opt:`-topg_to_phi` and :opt:`-phi_min`, :opt:`-phi_max`, :opt:`-topg_min`,
       :opt:`-topg_max`
     - Compute `\phi` using equation :eq:`eq-phipiecewise`.
   * - :opt:`-yield_stress tillphi_opt`
     - Compute the till friction angle `\phi` in :eq:`eq-mohrcoulomb` iteratively in an
       equilibrium simulation using equation :eq:`eq-phi-iterative`.
   * - :opt:`-yield_stress constant`
     - Keep the current values of the till yield stress `\tau_c`. That is, do not update
       them by the default model using the stored basal melt water. Only effective if
       ``-stress_balance ssa`` or ``-stress_balance ssa+sia`` is also set.
   * - :opt:`-tauc`
     - Directly set the till yield stress `\tau_c`, in units Pa, at all grounded locations
       and all times. Only effective if used with ``-yield_stress constant``, because
       otherwise `\tau_c` is updated dynamically.

.. _sec-tillphi-heuristic:

Till friction angle heuristic
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We find that an effective, though heuristic, way to determine `\phi` in
:eq:`eq-mohrcoulomb` is to make it a function of bed elevation
:cite:`AschwandenAdalgeirsdottirKhroulev`, :cite:`Martinetal2011`,
:cite:`Winkelmannetal2011`. This heuristic is motivated by hypothesis that basal material
with a marine history should be weak :cite:`HuybrechtsdeWolde`. PISM has a mechanism
setting `\phi` to be a *piece-wise linear* function of bed elevation. The corresponding
command line options are

.. code-block:: none

   -topg_to_phi -phi_min phimin -phi_max phimax -topg_min bmin -topg_max bmax

Thus the user supplies 4 parameters: `\phimin`, `\phimax`, `\bmin`, `\bmax`, where `b`
stands for the bed elevation. To explain these, we define `M = (\phimax - \phimin) /
(\bmax - \bmin)`. Then

.. math::
   :label: eq-phipiecewise

   \phi(x,y) =
   \begin{cases}
     \phimin, & b(x,y) \le \bmin, \\
     \phimin + (b(x,y) - \bmin) \,M, & \bmin < b(x,y) < \bmax, \\
     \phimax, & \bmax \le b(x,y).
   \end{cases}

It is worth noting that an earth deformation model (see section :ref:`sec-bed-def`) changes
`b(x,y)` (NetCDF variable :var:`topg`) used in :eq:`eq-phipiecewise`, so that a sequence
of runs such as

.. code-block:: bash

   pism -i foo.nc -bed_def lc -stress_balance ssa+sia \
         -topg_to_phi -phi_min 10 -phi_max 30 -topg_min -50 -topg_max 0 ... -o bar.nc

   pism -i bar.nc -bed_def lc -stress_balance ssa+sia \
         -topg_to_phi -phi_min 10 -phi_max 30 -topg_min -50 -topg_max 0 ... -o baz.nc

will use *different* :var:`tillphi` fields in the first and second runs. PISM will print a
warning during initialization of the second run:

.. code-block:: none

   * Initializing the default basal yield stress model...
     option -topg_to_phi seen; creating tillphi map from bed elev ...
   PISM WARNING: -topg_to_phi computation will override the 'tillphi' field
                 present in the input file 'bar.nc'!

Omitting :opt:`-topg_to_phi` in the second run would make PISM continue with the
same :var:`tillphi` field which was set in the first run.

.. rubric:: Parameters

Prefix: ``basal_yield_stress.mohr_coulomb.topg_to_phi.``

.. pism-parameters::
   :prefix: basal_yield_stress.mohr_coulomb.topg_to_phi.

.. _sec-tillphi-optimization:

Till friction angle optimization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::

   This is a work in progress. Use at your own risk.

In *grounded* areas the distribution of till friction angle `\phi` (see
:eq:`eq-mohrcoulomb`) can be iteratively optimized in a forward equilibrium simulation.
[#f1]_

The iteration starts from a `\phi` distribution set using :opt:`-plastic_phi`,
:opt:`-topg_to_phi`, or read from an input file (variable :var:`tillphi`).

During each step, an adjustment `\Delta \phi` is added to the previous value of
`\phi` and the result is clipped to ensure `\phi \in [\phimin, \phimax]`:

.. math::
   :label: eq-phi-iterative

   \phi_{n+1} = \min(\max(\phimin, \phi_n + \Delta \phi), \phimax),

The value of `\Delta \phi` is proportional to the difference between the target (usually
observed present day, e.g. :cite:`BEDMAP02`) and modeled surface elevations. It is
similarly clipped to ensure `\Delta \phi \in [\dphimin, \dphimax]`:

.. math::
   :label: eq-phi-adjustment

   \Delta \phi &= \min (\max(\dphimin, \Delta\tilde\phi), \dphimax),

   \Delta \tilde \phi &= C \left( h_{\mathrm{observed}} - h_{\mathrm{modeled}} \right),

   \dphimin &= -2 \dphimax.

Here `C` is the (positive) scaling factor (units: `{}^\circ / \mathrm{m}`) set using
:config:`basal_yield_stress.mohr_coulomb.tillphi_opt.dphi_scale`.

.. note::

   The adjustment `\Delta \phi` is *positive* if the modeled surface elevation is below
   the reference value, and *negative* otherwise.

   In other words, the basal resistance is *increased* if the ice thickness is too low and
   *decreased* otherwise.

The lower bound `\phimin = \phi_0` is a piece-wise linear function of the bed topography
`b`:

.. math::
   :label: eq-phi-lower-bound

   \phi_0(b) =
   \begin{cases}
   \phi_{0,\mathrm{min}}, &b \le \bmin,\\
   \phi_{0,\mathrm{min}} + (\phi_{0,\mathrm{max}} - \phi_{0,\mathrm{min}})
   \frac{b - \bmin}{\bmax - \bmin}, & \bmin < b \le \bmax, \\
   \phi_{0,\mathrm{max}} & \bmax < b.
   \end{cases}

Similarly to the till friction angle heuristic :eq:`eq-phipiecewise`, we assume that
"marine" sediments (below `\bmin`) can be much weaker than rather "continental" bedrock
material (above `\bmax`). In sensitivity experiments we found a strong sensitivity of the
Antarctic Ice Sheet's ice volume in particular to the choice of `\phimin` (see
:cite:`Albrecht2020PaleoSensitivity`).

To allow ice geometry to respond to changes in the till friction angle the simulation goes
on for `\dt_{\phi}` years between iterations.

Iterations at a particular location are considered "done" when the rate of change of the
surface elevation mismatch `\Delta h = h_{\mathrm{observed}} - h_{\mathrm{modeled}}`
approximated using subsequent steps falls below a threshold `D` set using
:config:`basal_yield_stress.mohr_coulomb.tillphi_opt.dhdt_min`:

.. math::
   :label: eq-phi-iterative-convergence

   \frac{\Delta h(x,y,T+\dt_{\phi}) - \Delta h(x,y,T) }{\dt_{\phi}} \leq D

The :var:`diff_mask` diagnostic variable is set to 0 to indicate that `\phi` "converged"
at this location.

.. rubric:: Parameters

Prefix: ``basal_yield_stress.mohr_coulomb.tillphi_opt.``

.. pism-parameters::
   :prefix: basal_yield_stress.mohr_coulomb.tillphi_opt.

When the domain contains a grounding line the mismatch between modeled and observed
surface elevation is meaningful only if the ice is grounded in *both* data sets. A retreat
of the grounding line would make it impossible to optimize the till friction angle in
areas that are observed to contain grounded ice but are covered by water in a simulation.

To avoid this issue, we

- disable the influence of the basal melt rate on geometry evolution by setting
  :config:`geometry.update.use_basal_melt_rate` to "false",
- modify the surface mass balance in grounded areas to disallow grounding line retreat by
  adding ``no_gl_retreat`` to the command-line option selecting a surface model (see
  :ref:`sec-surface-no-gl-retreat`), and
- fix ice thickness where the bed elevation is below sea level and the ice (if present) is
  floating.

To fix ice thickness, we create a mask with ones where ice is floating or there is no ice
and the bed is below sea level:

.. code-block:: bash

   ncap2 -O \
     -s "where(topg < 0 && thk*(910.0/1028.0) < 0 - topg) thk_bc_mask=1;" \
     input.nc input-with-mask.nc

Here `910` is the ice density (see :config:`constants.ice.density`), `1028` is the sea
water density (see :config:`constants.sea_water.density`), and `0` is the sea level
elevation.

.. rubric:: Reported diagnostic quantities

#. :var:`usurf_difference` reports the mismatch between modeled and reference surface
   elevation fields
#. :var:`diff_mask` reports the area where the till friction angle is iteratively adjusted
   or where the convergence criterion is met.
#. :var:`usurf_target` reports the reference (target) ice surface elevation in use.


.. _sec-effective-pressure:

Determining the effective pressure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When using the default option ``-yield_stress mohr_coulomb``, the effective pressure on
the till `N_{till}` is determined by the modeled amount of water in the till. Lower
effective pressure means that more of the weight of the ice is carried by the pressurized
water in the till and thus the ice can slide more easily. That is, equation
:eq:`eq-mohrcoulomb` sets the value of `\tau_c` proportionately to `N_{till}`. The amount
of water in the till is, however, a nontrivial output of the hydrology (see
:ref:`sec-subhydro`) and conservation-of-energy (see :ref:`sec-energy`) submodels in PISM.

Following :cite:`Tulaczyketal2000`, based on laboratory experiments with till extracted
from an ice stream in Antarctica, :cite:`BuelervanPelt2015` propose the following
parameterization which is used in PISM. It is based on the ratio
`s=W_{till}/W_{till}^{max}` where `W_{till}` is the effective thickness of water in the
till and `W_{till}^{max}` (:config:`hydrology.tillwat_max`) is the maximum amount of water
in the till (see section :ref:`sec-subhydro`):

.. math::
   :label: eq-computeNtill

   N_{till} = \min\left\{P_o, N_0 \left(\frac{\delta P_o}{N_0}\right)^s \, 10^{(e_0/C_c) \left(1 - s\right).}\right\}

Here `P_o` is the ice overburden pressure, which is determined entirely by the ice
thickness and density, and the remaining parameters are listed below

.. rubric:: Parameters

Prefix: ``basal_yield_stress.mohr_coulomb.``

.. pism-parameters::
   :prefix: basal_yield_stress.mohr_coulomb.
   :exclude: basal_yield_stress.mohr_coulomb.(tillphi_opt|topg_to_phi)

.. note::

   While there is experimental support for the default values of `C_c`, `e_0`, and `N_0`,
   the value of `\delta` should be regarded as uncertain, important, and subject to
   parameter studies to assess its effect.

.. _sec-min-effective-pressure:

Controlling minimum effective pressure
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The effective pressure `N_{till}` above satisfies (see equation 20 in
:cite:`BuelervanPelt2015`)

.. math::
   :label: eq-mohr-coulomb-Ntill-bounds

   \delta P_o \le N_{till} \le P_o.

In other words, `\delta` controls the lower bound of the effective pressure. In addition
to setting it using a configuration parameter one can use a space- and time-dependent
field. Set :config:`basal_yield_stress.mohr_coulomb.delta.file` to the name of the file
containing the variable :var:`mohr_coulomb_delta` (dimensionless, i.e. units of "1").

.. note::

   PISM restricts the time step to capture changes in `\delta`. For example, if you
   provide monthly records of `\delta` PISM will make sure no time step spans more than
   one month.

   PISM uses piece-wise linear interpolation in time for model times between records of
   `\delta`.

..
   FIXME: EVOLVING CODE: If the :config:`basal_yield_stress.add_transportable_water`
   configuration flag is set then the above formula becomes...

.. rubric:: Footnotes

.. [#f1] This is similar to the simple inversion method described in
   :cite:`PollardDeConto2012SLIDE` which optimizes the basal sliding coefficient `\tau_c`
   instead.
