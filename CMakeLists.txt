PROJECT(Gorgon)

MARK_AS_ADVANCED(FORCE CMAKE_BACKWARDS_COMPATIBILITY)
MARK_AS_ADVANCED(CLEAR CMAKE_VERBOSE_MAKEFILE)
OPTION(CMAKE_VERBOSE_MAKEFILE "if all commands will be echoed to the console during the make" ON)
OPTION(ENABLE_PYTHON "enable boost python support" ON)

SET(GORGON_INSTALL_PREFIX /data2/gtang/Gorgon CACHE PATH "installation prefix")
SET(CMAKE_INSTALL_PREFIX ${GORGON_INSTALL_PREFIX} CACHE INTERNAL "installation prefix")
MARK_AS_ADVANCED(CLEAR GORGON_INSTALL_PREFIX)

SET(LIBRARY_OUTPUT_PATH ${EMAN_INSTALL_PREFIX}/lib CACHE PATH "output directory for building all libraries.")

IF(CMAKE_COMPILER_IS_GNUCXX)
    #SET(EMAN_CXX_FLAGS "-Wall -Werror" CACHE INTERNAL "EMAN CXX FLAGS")
    IF (ENABLE_DEBUG)
    	SET(EMAN_CXX_FLAGS "-g -Wall -W -Wconversion -Wcast-qual -Wwrite-strings" CACHE INTERNAL "EMAN CXX FLAGS")
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g")
    ELSE (ENABLE_DEBUG)
    	SET(EMAN_CXX_FLAGS "-Wall" CACHE INTERNAL "EMAN CXX FLAGS")
    ENDIF (ENABLE_DEBUG)
    SET(PLATFORMLIB "/usr/lib64" CACHE INTERNAL "lib64")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(CMAKE_C_COMPILER ${CMAKE_CXX_COMPILER})
SET(CMAKE_ORIG_FLAGS ${CMAKE_CXX_FLAGS})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMAN_CXX_FLAGS} ${OPT_FLAGS}")

MARK_AS_ADVANCED(CMAKE_C_FLAGS)
MARK_AS_ADVANCED(CMAKE_CXX_FLAGS)

INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)

SET(INCLUDE_DIRS 
	${GORGON_SOURCE_DIR}/source
	${GORGON_SOURCE_DIR}/source/ExternalLibraries
	$ENV{HOME}/include 
	/usr/local/include 
	/usr/include 
	/sw/include
	/usr/X11R6/include
	${GLUT_INCLUDE_PATH}
	${GLU_INCLUDE_PATH}
	${GL_INCLUDE_PATH}
	)

SET(LIB_DIRS 
	$ENV{GORGONDIR}/lib 
	$ENV{HOME}/lib 
	${PLATFORMLIB} 
	/usr/local/lib
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

INCLUDE_DIRECTORIES(. ${GORGON_INSTALL_PREFIX}/source 
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
	
   	CHECK_PYTHON($ENV{PYTHON_VERSION} 2.5 2.4 2.3 2.2)
    
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

SET(LIB_GORGON_SOURCE
	Foundation/Foundation.cpp
)

ADD_LIBRARY(GORGON SHARED ${LIB_GORGON_SOURCE})
TARGET_LINK_LIBRARIES(GORGON ${GL_LIBRARY} ${GLU_LIBRARY} ${GLUT_LIBRARY} m)

INSTALL_TARGETS(/lib GORGON)
#INSTALL_FILES(/include "^.+\\.h$")
