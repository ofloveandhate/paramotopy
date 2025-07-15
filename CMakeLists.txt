cmake_minimum_required(VERSION 3.31)
project(paramotopy VERSION 1.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


configure_file(paramotopyconfig.h.in include/paramotopy/paramotopyconfig.h)



option(BERTINI_WAS_INSTALLED_BY_CMAKE "look for a version of Bertini 1 installed by cmake" ON)




# Append the cmake/ directory to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")




FIND_PACKAGE(FLEX REQUIRED)
FIND_PACKAGE(BISON REQUIRED)

# sadly, these don't have cmake-provided find calls.
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)



# i wrote this one, and my code for finding it probably sucks

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    "${PROJECT_BINARY_DIR}/include" 

    ${Boost_INCLUDE_DIRS}

    ${MPI_INCLUDE_PATH}

    ${GMP_INCLUDES}
    ${MPFR_INCLUDE_DIR}

    ${bertini1_INCLUDE_DIR}
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
install(TARGETS paramotopy step2 DESTINATION bin)
install(FILES ${common_headers}  ${para_headers} ${step2_headers} DESTINATION include/paramotopy)
