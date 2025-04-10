.. include:: ../../global.txt

Initialization from a saved model state
---------------------------------------

"Initialization" has the specific, simple meaning in PISM that option ":opt:`-i`" was
used. If a previous PISM run has saved a NetCDF file using ":opt:`-o`" then that file will
contain complete initial conditions for continuing the run. The output file from the last
run can be loaded with ":opt:`-i`":

.. code-block:: none

   pism -eisII A -Mx 61 -My 61 -y 100 -o foo.nc
   pism -eisII A -Mx 61 -My 61 -i foo.nc -y 100 -o bar.nc

As noted, verification tests (section :ref:`sec-verif`) and simplified-geometry experiments
(section :ref:`sec-simp`) do not need input files at all because they initialize from
formulas in the source code. They can, however, be continued from saved model states using
:opt:`-i`. Specifying the simplified geometry experiment or verification test *is*,
however, necessary if the run is to continue with the climate inputs for that experiment
or test. For example, based on the above ``pism -eisII A`` runs, it is valid to do

.. code-block:: none

   pism -i foo.nc -y 100 -o bar.nc

but the climate and other parameters use PISM default values, and thus are not
(necessarily) the values specified in EISMINT II.

.. _sec-i-format:

``-i`` file format
^^^^^^^^^^^^^^^^^^

PISM produces CF-1.6 compliant NetCDF files. The easiest way to learn the output format
*and* the :opt:`-i` format is to do a simple run and then look at the metadata in the
resulting file, like this:

.. code-block:: none

   pism -eisII A -Mx 61 -My 61 -y 10 -o foo.nc
   ncdump -h foo.nc | less
