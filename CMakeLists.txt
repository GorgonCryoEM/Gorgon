CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

include(cmake/Debug.cmake)
include(cmake/MacOSX.cmake)
include(cmake/Windows.cmake)

PROJECT(Gorgon)
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

set(GORGON_EXTERNAL_LIBRARIES_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/../Gorgon-Downloads CACHE PATHNAME "Download directory")
option(EXTERNAL_LIBS_BUILD         "Build external libraries" OFF)
option(LOG_EXTERNAL_LIBRARY_BUILDS "Log output of external library builds" ON )

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
        SIP
        PyQt4
        )

foreach(proj ${EXTERNAL_PROJECTS})
    include(cmake/${proj}.cmake)
endforeach()
# --------------------------------------------------------------------

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib${GORGON_OS_VERSION}/x${GORGON_TARGET_ARCH})
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

#add_dependencies(pyGORGON Rescan)

set(CMAKE_INSTALL_PREFIX $ENV{HOME}/Gorgon/)

if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("CMAKE_INSTALL_PREFIX= ${CMAKE_INSTALL_PREFIX}")
endif()

#install(TARGETS pyGORGON
#         BUNDLE DESTINATION dist)

set( EXTERNAL_PROJECTS_INSTALL
        SIP
        PyQt4
        )

install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/get-pip.py
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
    )

install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/python/Python.framework/Versions/2.7/lib/python2.7/site-packages/pip install py2app PyOpenGL PyOpenGL_accelerate
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
    )
install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/setup.py py2app --bdist-base=${CMAKE_BINARY_DIR}/test/build  --dist-dir=${CMAKE_BINARY_DIR}/test/dist
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Gorgon/src_py/)"
    )
