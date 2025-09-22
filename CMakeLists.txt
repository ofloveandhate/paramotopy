cmake_minimum_required(VERSION 3.31)
project(paramotopy VERSION 1.2.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type set to Release by default, since it was not specified.")
endif(NOT CMAKE_BUILD_TYPE)



configure_file(paramotopyconfig.h.in include/paramotopy/paramotopyconfig.h)



option(BERTINI_WAS_INSTALLED_BY_CMAKE "look for a version of Bertini 1 installed by cmake" ON)




# Append the cmake/ directory to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")



set(PARAMOTOPY_INC_DEST "include")
set(PARAMOTOPY_LIB_DEST "lib")
set(PARAMOTOPY_BIN_DEST "bin")
set(PARAMOTOPY_CMAKE_DEST "lib/cmake/paramotopy") # https://stackoverflow.com/questions/55845401/where-should-cmake-files-be-installed


# i want the debugger information, please
add_compile_options(-g)



if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-Wall -Wextra -Wno-unused-parameter)
  add_compile_options(-O0)
else()
  add_compile_options(-O2)
endif()


FIND_PACKAGE(FLEX REQUIRED)
FIND_PACKAGE(BISON REQUIRED)

# sadly, these don't have cmake-provided find calls.
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)


# bertini 1.7 or later should be findable from cmake with no extra work, 
# cuz I wrote the build system to also install cmake files to find it.
find_package(bertini1 1.7 REQUIRED CONFIG)

# Find required packages
find_package(MPI REQUIRED)
find_package(Boost 1.53 REQUIRED
    COMPONENTS
        filesystem
        timer
    )




set(CMAKE_CXX_FLAGS ${bertini1_PARALLEL_C_FLAGS})

include(CMakePrintHelpers)
cmake_print_variables(GMP_INCLUDES)
cmake_print_variables(MPFR_INCLUDES)
cmake_print_variables(bertini1_INCLUDE_DIR)
# cmake_print_variables(bertini1_LIBRARIES)
message("build include dir is " ${CMAKE_CURRENT_SOURCE_DIR}/include)



# Include directories



if (BERTINI_WAS_INSTALLED_BY_CMAKE)
    # nothing, the generated exports from the cmake-built bertini 1 will already bring the correct includes
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        "${PROJECT_BINARY_DIR}/include" 

        ${Boost_INCLUDE_DIRS}

        ${MPI_INCLUDE_PATH}

        ${GMP_INCLUDES}
        ${MPFR_INCLUDE_DIR}
    )
else()

endif(BERTINI_WAS_INSTALLED_BY_CMAKE)




include_directories(

    "${PROJECT_BINARY_DIR}/include" 

    ${Boost_INCLUDE_DIRS}

    ${MPI_INCLUDE_PATH}

    ${GMP_INCLUDES}
    ${MPFR_INCLUDE_DIR}

    ${bertini1_INCLUDE_DIR}

    # during build, look here
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>    # these are the human-written headers
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>    # this is for generated files, say from bison/flex
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/paramotopy>    # this is for generated files, say from bison/flex

    # post-install, this is where they can be found
    $<INSTALL_INTERFACE:${PARAMOTOPY_INC_DEST}/>         
)

# Source files

INCLUDE(files.cmake)






# Executable
add_executable(paramotopy 
                 ${common_headers} 
                 ${common_src}
                 ${para_headers}
                 ${para_src}
                 ${libtinyxml_src}
                 ${libtinyxml_headers}
                 )

add_executable(step2 
                 ${common_headers} 
                 ${common_src}
                 ${step2_headers}
                 ${step2_src}
                 ${libtinyxml_src}
                 ${libtinyxml_headers}
                 )

target_compile_definitions(paramotopy PUBLIC BOOST_TIMER_ENABLE_DEPRECATED=1)
target_compile_definitions(step2 PUBLIC BOOST_TIMER_ENABLE_DEPRECATED=1)




target_link_libraries(paramotopy
    ${MPI_CXX_LIBRARIES}
    ${Boost_LIBRARIES}
    ${MPFR_LIBRARIES}
    ${GMP_LIBRARIES}
)



target_link_libraries(step2
    ${MPI_CXX_LIBRARIES}
    ${Boost_LIBRARIES}
    ${MPFR_LIBRARIES}
    ${GMP_LIBRARIES}
    bertini-serial
)



# Installation
install(TARGETS paramotopy step2 EXPORT paramotopyTargets DESTINATION ${PARAMOTOPY_BIN_DEST})
install(FILES ${common_headers}  ${para_headers} ${step2_headers} DESTINATION ${PARAMOTOPY_INC_DEST}/paramotopy)






# from
# https://cmake.org/cmake/help/v3.9/module/CMakePackageConfigHelpers.html#module:CMakePackageConfigHelpers
include(CMakePackageConfigHelpers)



configure_package_config_file(
    cmake/paramotopyConfig.cmake.in  # the thing to read
    ${CMAKE_CURRENT_BINARY_DIR}/paramotopyConfig.cmake # the thing to create

    INSTALL_DESTINATION ${PARAMOTOPY_CMAKE_DEST}  #where to put it when installed using `make install`

    PATH_VARS PARAMOTOPY_INC_DEST # path variables to make visible
    )



write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/paramotopyConfigVersion.cmake
  VERSION ${paramotopy_VERSION}
  COMPATIBILITY SameMajorVersion)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/paramotopyConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/paramotopyConfigVersion.cmake
        DESTINATION ${PARAMOTOPY_CMAKE_DEST}
              )

# ----------
# stuff to generate compressed folders for source distribution
# -------------------------

# this uses `make package_source`

# first, we have to set up some variables.

# this is a ;list of regex's.  periods must be escaped.  so many //'s.
set(CPACK_SOURCE_IGNORE_FILES
  \\.git/
  \\.DS_Store
  documentation_cpp/
  build/
  manual/
  "bfiles*"
  ".*~$"
  \\.gitignore
  .\*.mat
  .\*.out
)


# CPACK_VERBATIM_VARIABLES
# see https://cmake.org/cmake/help/latest/module/CPack.html
# --------------------------
#
# Added in version 3.4.
#
# If set to TRUE, values of variables prefixed with CPACK_ will be escaped before being written to the configuration files, so that the cpack program receives them exactly as they were specified. If not, characters like quotes and backslashes can cause parsing errors or alter the value received by the cpack program. 
# Defaults to FALSE for backwards compatibility.
set(CPACK_VERBATIM_VARIABLES YES)


include(InstallRequiredSystemLibraries)

# pass in some variables for version requirement matching for the finder of this library
set(CPACK_PACKAGE_VERSION_MAJOR "${paramotopy_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${paramotopy_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${paramotopy_VERSION_PATCH}")

# ;-list of generators to use.  huzzah!!!
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")


# done setting up CPack variables.

# now, actually turn on CPack, AFTER the options variables are set.
# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Packaging%20With%20CPack.html
include(CPack)

