CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

include(cmake/MacOSX.cmake)
include(cmake/Windows.cmake)

PROJECT(Gorgon)
#include(cmake/Python.cmake)

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



#find_package(GLUT REQUIRED)


SET(GORGON_INCLUDE_DIRS 
	${GLUT_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibraries
	${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibraries/GL
	${GLU_INCLUDE_PATH}
	${GL_INCLUDE_PATH}
	${OPENGL_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
	${PYTHON_INCLUDE_DIR}
	${FFTW3F_INCLUDE_DIRS}
	)
INCLUDE_DIRECTORIES(${GORGON_INCLUDE_DIRS}) 

add_subdirectory(${GORGON_EXTERNAL_LIBRARIES_DIR}/python)
ADD_LIBRARY(pyGORGON SHARED Gorgon/src_cpp/LibPyGorgon.cpp)

#add_dependencies(pyGORGON boost)
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
install(CODE "execute_process(COMMAND python ../../setup.py py2app --bdist-base=${CMAKE_BINARY_DIR}/test/build  --dist-dir=${CMAKE_BINARY_DIR}/test/dist
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Gorgon/src_py/)"
    )
