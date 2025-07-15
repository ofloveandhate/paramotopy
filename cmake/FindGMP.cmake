# Try to find the GNU Multiple Precision Arithmetic Library (GMP)
# See http://gmplib.org/
# borrow from bertini2

# In order to find gmp installed with conda, run 'conda activate' and use 'cmake .. -DCMAKE_PREFIX_PATH=$CONDA_PREFIX'

if (GMP_INCLUDES AND GMP_LIBRARIES)
  set(GMP_FIND_QUIETLY TRUE)
endif (GMP_INCLUDES AND GMP_LIBRARIES)

find_path(GMP_INCLUDES
  NAMES
  gmp.h
  PATHS
  $ENV{GMP_INC}
  ${INCLUDE_INSTALL_DIR}
)

find_library(GMP_LIBRARIES gmp PATHS $ENV{GMP_LIB} ${LIB_INSTALL_DIR})

include(FindPackageHandleStandardArgs)

# Makes sure that gmp_include and gmp_libraries are valid
# https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.html
find_package_handle_standard_args(GMP DEFAULT_MSG
                                  GMP_INCLUDES GMP_LIBRARIES)
mark_as_advanced(GMP_INCLUDES GMP_LIBRARIES)