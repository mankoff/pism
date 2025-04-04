if (Pism_BUILD_PYTHON_BINDINGS)

  # ``UseSWIG`` generates standard target names.
  cmake_policy(SET CMP0078 NEW)

  # ``UseSWIG`` honors ``SWIG_MODULE_NAME`` via ``-module`` flag.
  cmake_policy(SET CMP0086 NEW)

  include(${SWIG_USE_FILE})

  include_directories (BEFORE SYSTEM ${PETSC4PY_INCLUDES} ${Python3_INCLUDE_DIRS})
  include_directories (. ${Pism_SOURCE_DIR}/src)

  set(PISM_PYTHON_BINDING_LIBRARIES "libpism")

  list(APPEND PISM_Python_deps
    AgeModel.i
    geometry.i
    pism_BedDef.i
    pism_ColumnSystem.i
    pism_DM.i
    pism_FlowLaw.i
    pism_Hydrology.i
    pism_Grid.i
    pism_Array.i
    pism_File.i
    pism_SIA.i
    pism_SSA.i
    pism_blatter.i
    pism_VariableMetadata.i
    pism_Vars.i
    pism_Vec.i
    pism_atmosphere.i
    pism_energy.i
    pism_exception.i
    pism_inverse.i
    pism_ocean.i
    pism_options.i
    pism_surface.i
    pism_verification.i
    pism_frontalmelt.i
    pism_calving.i
    )

  set_property(SOURCE PISM.i PROPERTY SWIG_FLAGS "-I${CMAKE_BINARY_DIR}" APPEND)

  if(Pism_USE_JANSSON)
    set_property(SOURCE PISM.i PROPERTY SWIG_FLAGS "-DPism_USE_JANSSON=1" APPEND)
  else()
    set_property(SOURCE PISM.i PROPERTY SWIG_FLAGS "-DPism_USE_JANSSON=0" APPEND)
  endif()

  if(Pism_DEBUG)
    set_property(SOURCE PISM.i PROPERTY SWIG_FLAGS "-DPism_DEBUG=1" APPEND)
  else()
    set_property(SOURCE PISM.i PROPERTY SWIG_FLAGS "-DPism_DEBUG=0" APPEND)
  endif()

  set_source_files_properties(PISM.i PROPERTIES
    CPLUSPLUS ON
    SWIG_MODULE_NAME cpp)

  # has to be set *before* calling SWIG_ADD_MODULE
  set(SWIG_MODULE_cpp_EXTRA_DEPS ${PISM_Python_deps})

  SWIG_ADD_LIBRARY(cpp
    LANGUAGE python
    SOURCES PISM.i pism_python.cc)

  SWIG_LINK_LIBRARIES(cpp ${Python3_LIBRARIES} ${PISM_PYTHON_BINDING_LIBRARIES})

  # Disable compiler warnings:
  SET_SOURCE_FILES_PROPERTIES(${swig_generated_file_fullname}
    PROPERTIES COMPILE_FLAGS "-w")

  add_custom_target (clean_swig
    COMMAND ${CMAKE_COMMAND} -E touch PISM.i
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM)

  # Disable compiler warnings:
  SET_SOURCE_FILES_PROPERTIES(${swig_generated_file_fullname}
    PROPERTIES COMPILE_FLAGS "-w")

  # Generate version_info.py to make PETSc's version information available to PISM's
  # Python bindings. This is necessary to initialize petsc4py using the correct PETSC_ARCH
  # value *before* importing PISM.cpp.
  configure_file(version_info.py.in version_info.py ESCAPE_QUOTES)

  set(TARGET_FILE $<TARGET_FILE:cpp>)

  install(TARGETS cpp
    DESTINATION ${PISM_PYTHON_MODULE_DIR}/PISM
    COMPONENT RuntimeLibraries
    )

  # Install Python .py files
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp.py
    ${CMAKE_CURRENT_BINARY_DIR}/version_info.py
    DESTINATION ${PISM_PYTHON_MODULE_DIR}/PISM
    COMPONENT RuntimeLibraries
    )

  add_custom_target (install_python_bindings_in_the_build_directory ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_FILE} ${CMAKE_BINARY_DIR}/site-packages/PISM
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cpp.py ${CMAKE_BINARY_DIR}/site-packages/PISM/
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/version_info.py ${CMAKE_BINARY_DIR}/site-packages/PISM/
    VERBATIM
    DEPENDS cpp install_site_packages_in_the_build_directory )

  # The Python code snippet below returns 1 if Python can import a PISM
  # and if the directory of said PISM matches the place we are about to
  # install PISM's Python wrappers. If the return value is not 1, we
  # print a message stating how to set your PYTHONPATH.
  set (pism_test_py "${PROJECT_BINARY_DIR}/pism_test.py")
  file (WRITE "${pism_test_py}" "
from os.path import realpath, join
try:
    import PISM
except:
    exit(0)

if realpath(PISM.__file__) == realpath(join('${PISM_PYTHON_MODULE_DIR}', 'PISM', '__init__.pyc')):
    exit(1)
")

  install(CODE "EXEC_PROGRAM( ${Python3_EXECUTABLE} ARGS ${pism_test_py} RETURN_VALUE PISM_PYTHON_MATCHES OUTPUT_VARIABLE PYTHON_OUT )
if(NOT PISM_PYTHON_MATCHES)
    MESSAGE(\"\n   ATTENTION! To complete installation of Python bindings you need to add\n\n   export PYTHONPATH=${PISM_FULL_PYTHON_MODULE_DIR}:\$PYTHONPATH\n\n   to your .bashrc or equivalent. \")
endif()
" )

endif(Pism_BUILD_PYTHON_BINDINGS)
