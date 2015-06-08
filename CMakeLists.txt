CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Gorgon)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/share/cmake-3.2/Modules/)
#message(FATAL_ERROR "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
find_package(PythonLibs REQUIRED)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
#find_package(FFTW3F REQUIRED)

#set(ext_dir ${CMAKE_SOURCE_DIR}/ExternalLibraries/FFTW)
set(ext_dir ${CMAKE_SOURCE_DIR}/ExternalLibraries)
set(ext_dirs FFTW GL)

if(APPLE)
	set(folder_name "Mac")
else()
	if(UNIX)
		set(folder_name "Linux")
	else()
		set(folder_name "Win")
	endif()
endif()

include(${CMAKE_SOURCE_DIR}/ExternalLibraries/FFTW3f.cmake)
include(${CMAKE_SOURCE_DIR}/ExternalLibraries/GLUT.cmake)
#message(FATAL_ERROR "After GLUT: ${GLUT_INCLUDE_DIR}")

message("Before: ${Boost_INCLUDE_DIR}")
include(${CMAKE_SOURCE_DIR}/ExternalLibraries/Boost.cmake)

if(WIN32)
    set(boost_components "python serialization")
else()
    set(boost_components "python")
endif()

set(Boost_USE_MULTITHREADED ON)
  
#find_package(Boost 1.57 REQUIRED COMPONENTS ${boost_components})

message("After 2: ${Boost_INCLUDE_DIR}")

function (getListOfVarsStartingWith _prefix _varResult)
    get_cmake_property(_vars VARIABLES)
    string (REGEX MATCHALL "[A-Za-z0-9_]*${_prefix}[A-Za-z0-9_]*" _matchedVars "${_vars}")
    set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()

#
#if(NOT Boost_FOUND)
#	set(BOOST_ROOT $ENV{HOME} CACHE PATH "The root directory containing Boost")
#	message(SEND_ERROR ${Boost_ERROR_REASON})
#endif()
#else()
	getListOfVarsStartingWith("GL" matchedVars)
	getListOfVarsStartingWith("gl" matchedVars1)
	foreach (_var IN LISTS matchedVars matchedVars1)
		message("${_var}=${${_var}}")
	endforeach()
#endif()

## Linux
#set(PYTHON_INCLUDE_DIR /usr/include/python2.6 CACHE PATH "blah (??) blah" FORCE)
#set(PYTHON_LIBRARY /usr/lib64/libpython2.6.so CACHE PATH "blah (??) blah" FORCE)


OPTION(CMAKE_VERBOSE_MAKEFILE "if all commands will be echoed to the console during the make" ON)
OPTION(ENABLE_FFTW3 "enable fftw 3 support" ON)
OPTION(ENABLE_FFTW_PLAN_CACHING "enable fftw caching" ON)
IF(ENABLE_FFTW3)
	ADD_DEFINITIONS(-DFFTW3)
ENDIF()

IF(ENABLE_FFTW_PLAN_CACHING)
	ADD_DEFINITIONS(-DFFTW_PLAN_CACHING)
ENDIF(ENABLE_FFTW_PLAN_CACHING)

SET(GORGON_INSTALL_PREFIX $ENV{HOME}/Gorgon CACHE PATH "installation prefix")
SET(CMAKE_INSTALL_PREFIX ${GORGON_INSTALL_PREFIX} CACHE INTERNAL "installation prefix")

SET(LIBRARY_OUTPUT_PATH ${GORGON_INSTALL_PREFIX}/lib CACHE PATH "output directory for building all libraries.")

MARK_AS_ADVANCED(CLEAR CMAKE_VERBOSE_MAKEFILE)
MARK_AS_ADVANCED(CLEAR GORGON_INSTALL_PREFIX)

SET(INCLUDE_DIRS 
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

SET(LIB_DIRS
	${OPENGL_gl_LIBRARY}
	${OPENGL_glu_LIBRARY}
	${GLUT_glut_LIBRARY}
	${OPENGL_LIBRARIES} 
	${Boost_LIBRARIES} 
	${PYTHON_LIBRARY}
	${FFTW3F_LIBRARIES}
	)

#INCLUDE_DIRECTORIES(${INCLUDE_DIRS}) 
include_directories(${INCLUDE_DIRS})
message("include dirs: ${INCLUDE_DIRS}")
get_directory_property(_vv INCLUDE_DIRECTORIES)
message("vvvvv: ${_vv}")
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ADD_LIBRARY(pyGORGON SHARED Gorgon/src_cpp/LibPyGorgon.cpp)
TARGET_LINK_LIBRARIES(pyGORGON ${LIB_DIRS})

include(CPack)

#install(TARGETS pyGORGON
#         LIBRARY DESTINATION dist)
#INSTALL_FILES(/include "^.+\\.h$")
