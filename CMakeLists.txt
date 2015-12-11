CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Gorgon
        VERSION 2.2.0
        LANGUAGES CXX
        )

include(cmake/Configure.cmake)

include(cmake/Debug.cmake)
include(cmake/MacOSX.cmake)
include(cmake/Windows.cmake)


# Number of processors/cores
include(cmake/ParallelJobs.cmake)

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

set(CMAKE_CXX_FLAGS_DEBUG "-DGORGON_DEBUG")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib${GORGON_OS_VERSION}/x${GORGON_TARGET_ARCH})
SET(CMAKE_INSTALL_PREFIX ${LIBRARY_OUTPUT_PATH} CACHE PATH "" FORCE)
if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("LIBRARY_OUTPUT_PATH= ${LIBRARY_OUTPUT_PATH}")
endif()

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
# Include and link directories, targets, target link libraries
# --------------------------------------------------------------------
list(APPEND GORGON_INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}
	${GORGON_EXTERNAL_LIBRARIES_DIR}
	)

link_directories(${Boost_LIBRARY_DIR})
include_directories(${GORGON_INCLUDE_DIRS})
ADD_LIBRARY(pyGORGON MODULE Gorgon/src_cpp/LibPyGorgon.cpp)
target_link_libraries(pyGORGON ${GORGON_LIBRARIES})

# --------------------------------------------------------------------
# C++11 flags
# --------------------------------------------------------------------
#set(cxx11_flags "-std=c++0x --stdlib=libc++")
#list( APPEND CMAKE_CXX_FLAGS           ${cxx11_flags})
#list( APPEND CMAKE_SHARED_LINKER_FLAGS ${cxx11_flags})

# --------------------------------------------------------------------
# Some debug output
# --------------------------------------------------------------------
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
install(TARGETS pyGORGON
        LIBRARY
        DESTINATION ${run_dir}
        COMPONENT "Gorgon"
        )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Gorgon/src_py/
        DESTINATION ${run_dir}
        COMPONENT "Gorgon"
        FILES_MATCHING
        PATTERN "*.py"
        )

add_custom_target(Gorgon
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Gorgon -P cmake_install.cmake
        )
        
include(cmake/Win_install.cmake)
# --------------------------------------------------------------------
#Package Step
# --------------------------------------------------------------------
if(APPLE)
    include(cmake/Mac_package.cmake)
elseif(WIN32)
    include(cmake/Win_package.cmake)
else()
    install(CODE "execute_process(COMMAND pyinstaller ${run_dir}/gorgon.pyw
                                                        --distpath=${package_dir}
                                                        --clean -y
        WORKING_DIRECTORY ${run_dir})"
        COMPONENT "Package"
        )
endif()

add_custom_target(Package
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Package -P cmake_install.cmake
        )
