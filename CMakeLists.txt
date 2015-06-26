CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

include(cmake/MacOSX.cmake)
include(cmake/Windows.cmake)

PROJECT(Gorgon)
ADD_LIBRARY(pyGORGON SHARED Gorgon/src_cpp/LibPyGorgon.cpp)

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



include(cmake/Python.cmake)
include(cmake/Boost.cmake)

include(cmake/FFTW3f.cmake)
include(cmake/GLUT.cmake)

find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)

OPTION(ENABLE_FFTW3 "enable fftw 3 support" ON)
OPTION(ENABLE_FFTW_PLAN_CACHING "enable fftw caching" ON)
IF(ENABLE_FFTW3)
	ADD_DEFINITIONS(-DFFTW3)
ENDIF()

IF(ENABLE_FFTW_PLAN_CACHING)
	ADD_DEFINITIONS(-DFFTW_PLAN_CACHING)
ENDIF(ENABLE_FFTW_PLAN_CACHING)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib${GORGON_OS_VERSION}/x${GORGON_TARGET_ARCH})
if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("LIBRARY_OUTPUT_PATH= ${LIBRARY_OUTPUT_PATH}")
endif()

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

SET(GORGON_LIBS
	${OPENGL_gl_LIBRARY}
	${OPENGL_glu_LIBRARY}
	${GLUT_glut_LIBRARY}
	${OPENGL_LIBRARIES} 
	${Boost_LIBRARIES} 
	${PYTHON_LIBRARY}
	${FFTW3F_LIBRARIES}
	)

INCLUDE_DIRECTORIES(${GORGON_INCLUDE_DIRS}) 

TARGET_LINK_LIBRARIES(pyGORGON Python)

#add_dependencies(pyGORGON boost)
#add_dependencies(pyGORGON python)
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
