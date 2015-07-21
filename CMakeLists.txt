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

set( EXTERNAL_PROJECTS_DOWNLOAD
        Python
        Boost
        FFTW3F
        )

set( EXTERNAL_PROJECTS_BUILD
        OpenGL
        GLUT
        ${EXTERNAL_PROJECTS_DOWNLOAD}
        )

set( EXTERNAL_PROJECTS_INSTALL
        SIP
        PyQt4
        )

foreach(proj ${EXTERNAL_PROJECTS_BUILD})
    include(cmake/${proj}.cmake)
endforeach()
# --------------------------------------------------------------------
# Download-All Step
# --------------------------------------------------------------------
list(APPEND EXTERNAL_PROJECTS_DOWNLOAD ${EXTERNAL_PROJECTS_INSTALL})
foreach(proj ${EXTERNAL_PROJECTS_DOWNLOAD})
    list(APPEND downs ${proj}-download) 
endforeach()

add_custom_target(Download-All
                 DEPENDS ${downs}
#                COMMAND
                COMMENT "Download-All Step"
#                VERBATIM
                )
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
# --------------------------------------------------------------------
#Install Step
# --------------------------------------------------------------------
# --------------------------------------------------------------------
# This is, actually, an install step, but runs at build time.
# Adding the loop into install(CODE ...) seems to run only those lines, not the whole CMakeLists.txt
# Thus, the functions called in *.cmake are not defined at that stage
# It is not possible to add dependencies to install target as it is not a real target
# --------------------------------------------------------------------

install(TARGETS pyGORGON
         LIBRARY
         DESTINATION ${python_lib-dynload}
         )

foreach(proj ${EXTERNAL_PROJECTS_INSTALL})
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${proj}.cmake)
endforeach()


install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${python_site-packages}/pip install --upgrade pip py2app PyOpenGL PyOpenGL_accelerate
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
    )
# --------------------------------------------------------------------
#Package Step
# --------------------------------------------------------------------
install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/setup.py py2app
                                                    --bdist-base=${CMAKE_BINARY_DIR}/package/build
                                                    --dist-dir=${CMAKE_BINARY_DIR}/package/dist
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Gorgon/src_py/)"
    )
