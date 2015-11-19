CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Gorgon
        VERSION 2.2.0
        LANGUAGES CXX
        )

include(cmake/Debug.cmake)
include(cmake/MacOSX.cmake)
include(cmake/Windows.cmake)

# --------------------------------------------------------------------
# Number of preocessors/cores
# --------------------------------------------------------------------
include(ProcessorCount)
ProcessorCount(NUMBER_OF_PARALLEL_JOBS)
math(EXPR NUMBER_OF_PARALLEL_JOBS ${NUMBER_OF_PARALLEL_JOBS}-1)
# --------------------------------------------------------------------
# Set some variables and options
# --------------------------------------------------------------------
option(ENABLE_CMAKE_DEBUG_OUTPUT "Print output of some variables for debugging purposes" OFF)

set(GORGON_EXTERNAL_LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/ExternalLibraries CACHE INTERNAL "External Libraries directory")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( GORGON_TARGET_ARCH 64 CACHE INTERNAL "Target architecture")
else()
    set( GORGON_TARGET_ARCH 32 CACHE INTERNAL "Target architecture")
endif()

ADD_LIBRARY(pyGORGON MODULE Gorgon/src_cpp/LibPyGorgon.cpp)
# --------------------------------------------------------------------
# Find dependencies
# --------------------------------------------------------------------
include(cmake/functions.cmake)

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
if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("LIBRARY_OUTPUT_PATH= ${LIBRARY_OUTPUT_PATH}")
endif()

list(APPEND GORGON_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}
	${GORGON_EXTERNAL_LIBRARIES_DIR}
#	TODO: Is this line necessary? Removing works on Mac 
	${GORGON_EXTERNAL_LIBRARIES_DIR}/GL
	)

include_directories(${GORGON_INCLUDE_DIRS})
target_link_libraries(pyGORGON ${GORGON_LIBRARIES})


if(ENABLE_CMAKE_DEBUG_OUTPUT)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

    foreach(dir ${dirs})
      message(STATUS "DIRS='${dir}'")
    endforeach()

    get_property(dirs TARGET pyGORGON PROPERTY LINK_LIBRARIES)    

    foreach(dir ${GORGON_LIBRARIES})
      message("LIBS='${dir}'")
    endforeach()
endif()
#INSTALL_TARGETS(/lib GORGON)







INSTALL_TARGETS(/lib pyGORGON)
#INSTALL_FILES(/include "^.+\\.h$")
