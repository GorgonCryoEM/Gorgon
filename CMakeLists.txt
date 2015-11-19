CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Gorgon
        VERSION 2.2.0
        LANGUAGES CXX
        )

INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)

SET(INCLUDE_DIRS 
	${GORGON_SOURCE_DIR}
	${GORGON_SOURCE_DIR}/ExternalLibraries
	$ENV{HOME}/include 
	/usr/local/include
	/usr/local/cuda/include
	/usr/include 
	/sw/include
	/usr/X11R6/include
	${GLUT_INCLUDE_PATH}
	${GLU_INCLUDE_PATH}
	${GL_INCLUDE_PATH}
	)
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


SET(LIB_DIRS 
	$ENV{GORGONDIR}/lib 
	$ENV{HOME}/lib 
	${PLATFORMLIB} 
	/usr/local/lib
	/usr/local/cuda/lib
	/usr/lib64 
	/usr/lib 
	/sw/lib
	)

MACRO(CHECK_REQUIRED_LIB2 upper lower header lower2 header2)
    FIND_PATH(${upper}_INCLUDE_PATH ${header} $ENV{${upper}DIR}/include ${INCLUDE_DIRS})
    IF(${upper}_INCLUDE_PATH)
    ELSE (${upper}_INCLUDE_PATH)
		FIND_PATH(${upper}_INCLUDE_PATH ${header2} $ENV{${upper}DIR}/include ${INCLUDE_DIRS})
    ENDIF(${upper}_INCLUDE_PATH)
    
    IF(${upper}_INCLUDE_PATH)
		FIND_LIBRARY(${upper}_LIBRARY NAMES ${lower} ${lower2} PATHS $ENV{${upper}DIR}/lib ${LIB_DIRS})		
		IF(${upper}_LIBRARY)
		ELSE(${upper}_LIBRARY)
			MESSAGE(SEND_ERROR "ERROR: ${upper} not found. please install ${upper} first!")
		ENDIF(${upper}_LIBRARY)
    ELSE(${upper}_INCLUDE_PATH)
		MESSAGE(SEND_ERROR "ERROR: ${upper} not found. please install ${upper} first!")
    ENDIF(${upper}_INCLUDE_PATH)
ENDMACRO(CHECK_REQUIRED_LIB2)

MACRO(CHECK_REQUIRED_LIB upper lower header)
    CHECK_REQUIRED_LIB2(${upper} ${lower} ${header} "" "")
ENDMACRO(CHECK_REQUIRED_LIB)

CHECK_REQUIRED_LIB(GL GL GL/gl.h)
CHECK_REQUIRED_LIB(GLU GLU GL/glu.h)
CHECK_REQUIRED_LIB(GLUT glut GL/glut.h)
CHECK_REQUIRED_LIB(GLUT glext GL/glext.h)

INCLUDE_DIRECTORIES(. ${GORGON_SOURCE_DIR}
					${GORGON_SOURCE_DIR}/ExternalLibraries
					${GSL_INCLUDE_PATH} 
					${GSL_INCLUDE_PATH}/gsl 
					${INCLUDE_DIRS}	   
			) 

MACRO(CHECK_OPTIONAL_LIB upper lower header)
    FIND_PATH(${upper}_INCLUDE_PATH ${header} $ENV{${upper}DIR}/include ${INCLUDE_DIRS}) 
    
    IF(${upper}_INCLUDE_PATH)
        FIND_LIBRARY(${upper}_LIBRARY NAMES ${lower} PATHS $ENV{${upper}DIR}/lib ${LIB_DIRS})
		
        IF(${upper}_LIBRARY)
			ADD_DEFINITIONS(-DEM_${upper})
			INCLUDE_DIRECTORIES(${${upper}_INCLUDE_PATH})
        ELSE(${upper}_LIBRARY)    
			#MESSAGE("WARNING: ${upper} not found, ENABLE_${upper} is set back to OFF")
			OPTION(ENABLE_${upper} "if enable ${upper} support" OFF)
        ENDIF(${upper}_LIBRARY)
		
    ELSE(${upper}_INCLUDE_PATH)
		#MESSAGE("WARNING: ${upper} not found, ENABLE_${upper} is set back to OFF")
        OPTION(ENABLE_${upper} "if enable ${upper} support" OFF)
    ENDIF(${upper}_INCLUDE_PATH)
ENDMACRO(CHECK_OPTIONAL_LIB)

MACRO(CHECK_PYTHON)
	SET(PYTHON_FOUND FALSE)
	
	FOREACH(PYVER ${ARGV})
		IF(NOT PYTHON_FOUND)			
			FIND_PATH(PYTHON_INCLUDE_PATH Python.h
				$ENV{PYTHON_ROOT}/include/python${PYVER}
				/usr/local/include/python${PYVER}
				/usr/include/python${PYVER})
			
			FIND_LIBRARY(PYTHON_LIBRARY NAMES python${PYVER} PATHS
				$ENV{PYTHON_ROOT}/lib/python${PYVER}/config
				/usr/local/lib/python${PYVER}/config
				/usr/lib/python${PYVER}/config
				/usr/lib64/python${PYVER}/config
				$ENV{PYTHON_ROOT}/lib /usr/local/lib /usr/lib)
			
			IF(PYTHON_INCLUDE_PATH)
				IF(PYTHON_LIBRARY)
					SET(PYTHON_FOUND TRUE)
				ENDIF(PYTHON_LIBRARY)
			ENDIF(PYTHON_INCLUDE_PATH)
			
		ENDIF(NOT PYTHON_FOUND)
    ENDFOREACH(PYVER)
ENDMACRO(CHECK_PYTHON)
				   
IF(ENABLE_PYTHON)
	
   	CHECK_PYTHON($ENV{PYTHON_VERSION} 2.7 2.5 2.4 2.3 2.2)
    
    IF(PYTHON_INCLUDE_PATH)
		CHECK_OPTIONAL_LIB(BOOST boost_python boost/python.hpp)
		IF(BOOST_LIBRARY)
		ELSE(BOOST_LIBRARY)
	    	CHECK_OPTIONAL_LIB(BOOST boost_python-gcc boost/python.hpp)
		ENDIF(BOOST_LIBRARY)
		IF(BOOST_INCLUDE_PATH AND BOOST_LIBRARY)
		    SUBDIRS(Gorgon/src_cpp)
		ENDIF(BOOST_INCLUDE_PATH AND BOOST_LIBRARY)
	    ELSE(PYTHON_INCLUDE_PATH)
		MESSAGE("WARNING: Python.h not found, ENABLE_PYTHON is set back to OFF")
		OPTION(ENABLE_PYTHON "if enable boost python support" OFF)
    ENDIF(PYTHON_INCLUDE_PATH)
    
ENDIF(ENABLE_PYTHON)



IF(ENABLE_FFTW3)
    CHECK_REQUIRED_LIB(FFTW3 fftw3f fftw3.h)
    ADD_DEFINITIONS(-DFFTW3)
    SET(FFTW_LIBRARIES ${FFTW3_LIBRARY})
    SET(FFTW_INCLUDE_PATH ${FFTW3_INCLUDE_PATH})
    SET(FFT_LIBRARIES ${FFTW_LIBRARIES})
ENDIF(ENABLE_FFTW3)

IF(ENABLE_FFTW_PLAN_CACHING)
	ADD_DEFINITIONS(-DFFTW_PLAN_CACHING)
ENDIF(ENABLE_FFTW_PLAN_CACHING)


#ADD_LIBRARY(GORGON SHARED ${LIB_GORGON_SOURCE})
#TARGET_LINK_LIBRARIES(GORGON ${GL_LIBRARY} ${GLU_LIBRARY} ${GLUT_LIBRARY} m)

#INSTALL_TARGETS(/lib GORGON)


INCLUDE_DIRECTORIES( ${GORGON_SOURCE_DIR} 
		./ExternalLibraries
		${BOOST_INCLUDE_PATH}
		${PYTHON_INCLUDE_PATH} 
		${INCLUDE_DIRS}
		${FFTW_INCLUDE_PATH}
) 




ADD_LIBRARY(pyGORGON SHARED Gorgon/src_cpp/LibPyGorgon.cpp)
TARGET_LINK_LIBRARIES(pyGORGON 
						${GL_LIBRARY} 
						${GLU_LIBRARY} 
						${GLUT_LIBRARY} 
						${BOOST_LIBRARY} 
						${PYTHON_LIBRARY}
						${FFT_LIBRARIES}
						m)

#ADD_EXECUTABLE(test test.cpp)
#TARGET_LINK_LIBRARIES(test GORGON)
#INSTALL_TARGETS($/ test)


INSTALL_TARGETS(/lib pyGORGON)
#INSTALL_FILES(/include "^.+\\.h$")
