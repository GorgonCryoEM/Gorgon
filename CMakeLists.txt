CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

include(cmake/Debug.cmake)
include(cmake/MacOSX.cmake)
include(cmake/Windows.cmake)

PROJECT(Gorgon)

include(ProcessorCount)
ProcessorCount(NUMBER_OF_PARALLEL_JOBS)
math(EXPR NUMBER_OF_PARALLEL_JOBS ${NUMBER_OF_PARALLEL_JOBS}-1)

option(ENABLE_CMAKE_DEBUG_OUTPUT "print output of some variables for debugging purposes" OFF)

set(GORGON_EXTERNAL_LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/ExternalLibraries CACHE INTERNAL "External Libraries directory")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( GORGON_TARGET_ARCH 64 CACHE INTERNAL "Target architecture")
  else()
    set( GORGON_TARGET_ARCH 32 CACHE INTERNAL "Target architecture")
endif()

#find_package(PythonInterp)
#find_package(PythonLibs REQUIRED)
#INCLUDE(FindPackageHandleStandardArgs)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(python DEFAULT_MSG python_LIBRARIES python_LIBRARY_DIR python_INCLUDE_DIR)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

include(cmake/Python.cmake)
include(cmake/Boost.cmake)
include(cmake/FFTW3f.cmake)

#include(cmake/Qt4.cmake)
include(cmake/SIP.cmake)
include(cmake/PyQt4.cmake)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib${GORGON_OS_VERSION}/x${GORGON_TARGET_ARCH})
if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("LIBRARY_OUTPUT_PATH= ${LIBRARY_OUTPUT_PATH}")
endif()

SET(GORGON_INCLUDE_DIRS 
	${GLUT_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${GORGON_EXTERNAL_LIBRARIES_DIR}
	${GORGON_EXTERNAL_LIBRARIES_DIR}/GL
	${GLU_INCLUDE_PATH}
	${GL_INCLUDE_PATH}
	${OPENGL_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${PYTHON_INCLUDE_DIR}
	${FFTW3F_INCLUDE_DIR}
	)
INCLUDE_DIRECTORIES(${GORGON_INCLUDE_DIRS}) 

ADD_LIBRARY(pyGORGON MODULE Gorgon/src_cpp/LibPyGorgon.cpp)
TARGET_LINK_LIBRARIES(pyGORGON
            ${PYTHON_LIBRARY}
            ${Boost_LIBRARIES}
            ${FFTW3F_LIBRARY}
            
            ${OPENGL_gl_LIBRARY}
        	${OPENGL_glu_LIBRARY}
        	${GLUT_glut_LIBRARY}
            )

#add_dependencies(pyGORGON Rescan)

set(CMAKE_INSTALL_PREFIX $ENV{HOME}/Gorgon/)

if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("CMAKE_INSTALL_PREFIX= ${CMAKE_INSTALL_PREFIX}")
endif()

#execute_process(COMMAND ls)
#add_custom_target(install 
#    COMMENT "Installing target"
#    )
#    
#add_custom_command(TARGET pyGORGON
#    POST_BUILD
#    COMMAND 
#    WORKING_DIRECTORY $CMAKE_INSTALL_PREFIX}/test
#    COMMENT "WORKING on my new command"
#)
#install(TARGETS pyGORGON
#         BUNDLE DESTINATION dist)
install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/get-pip.py
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
    )
#install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/python/lib/python2.7/site-packages/pip install py2app
install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/python/Python.framework/Versions/2.7/lib/python2.7/site-packages/pip install py2app PyOpenGL PyOpenGL_accelerate
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
    )
install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/setup.py py2app --bdist-base=${CMAKE_BINARY_DIR}/test/build  --dist-dir=${CMAKE_BINARY_DIR}/test/dist
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Gorgon/src_py/)"
    )
