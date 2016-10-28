CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
PROJECT(Gorgon
        VERSION 3.0.4
        LANGUAGES CXX
        )

include(cmake/functions.cmake)
include(cmake/wrappers.cmake)
include(cmake/Debug.cmake)

include(cmake/MacOSX/init.cmake)
include(cmake/Windows/init.cmake)

# Number of processors/cores
include(cmake/ParallelJobs.cmake)

# --------------------------------------------------------------------
# Set some variables and options
# --------------------------------------------------------------------
option(ENABLE_CMAKE_DEBUG_OUTPUT "Print output of some variables for debugging purposes" OFF)

set(GORGON_EXTERNAL_LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/ExternalLibraries CACHE INTERNAL "External Libraries directory")

set( GORGON_TARGET_ARCH $ENV{ARCH} CACHE INTERNAL "Target architecture")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DGORGON_DEBUG")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib${GORGON_OS_VERSION}/x${GORGON_TARGET_ARCH})
SET(CMAKE_INSTALL_PREFIX ${LIBRARY_OUTPUT_PATH} CACHE PATH "" FORCE)
if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("LIBRARY_OUTPUT_PATH= ${LIBRARY_OUTPUT_PATH}")
endif()

#Conda prefix
if(NOT WIN32)
    set(CMAKE_PREFIX_PATH $ENV{PREFIX})
else()
    set(CMAKE_PREFIX_PATH $ENV{LIBRARY_PREFIX})
endif()

# --------------------------------------------------------------------
# Find dependencies
# --------------------------------------------------------------------
set( EXTERNAL_PROJECTS
        OpenGL
        GLUT
        Python
        Boost
        FFTW3F
        )

foreach(proj ${EXTERNAL_PROJECTS})
    include(cmake/${proj}.cmake)
endforeach()

# --------------------------------------------------------------------
# Include directories
# --------------------------------------------------------------------
list(APPEND GORGON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src
	${GORGON_EXTERNAL_LIBRARIES_DIR}
	)

include_directories(${GORGON_INCLUDE_DIRS})
# --------------------------------------------------------------------
# Set directory vars
# --------------------------------------------------------------------
set(lib_install_dir gorgon)
set(cli_install_dir cli)
# --------------------------------------------------------------------
# Add subdirectory src/ which handles its subdirectories
# --------------------------------------------------------------------
add_subdirectory(src)
# --------------------------------------------------------------------
# Some debug output
# --------------------------------------------------------------------
if(ENABLE_CMAKE_DEBUG_OUTPUT)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

    foreach(dir ${dirs})
      message(STATUS "DIRS='${dir}'")
    endforeach()

    foreach(dir ${GORGON_LIBRARIES})
      message("LIBS='${dir}'")
    endforeach()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
        ${CMAKE_BINARY_DIR}/setup.py
        )
