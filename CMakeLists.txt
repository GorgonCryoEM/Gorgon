CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Gorgon
        VERSION 2.2.0
        LANGUAGES CXX
        )

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

if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("LIBRARY_OUTPUT_PATH= ${LIBRARY_OUTPUT_PATH}")
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
