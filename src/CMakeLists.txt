CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Gorgon
        VERSION 2.3.0
        LANGUAGES CXX
        )
include(cmake/Debug.cmake)
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
#set(GORGON_EXTERNAL_LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/../ExternalLibraries CACHE INTERNAL "External Libraries directory")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( GORGON_TARGET_ARCH 64 CACHE INTERNAL "Target architecture")
else()
    set( GORGON_TARGET_ARCH 32 CACHE INTERNAL "Target architecture")
endif()
# --------------------------------------------------------------------
# Find dependencies
# --------------------------------------------------------------------
include(cmake/functions.cmake)

set( EXTERNAL_PROJECTS
#        OpenGL
#        GLUT
        Python
        Boost
#        FFTW3F
        )

foreach(proj ${EXTERNAL_PROJECTS})
    include(cmake/${proj}.cmake)
endforeach()
# --------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib${GORGON_OS_VERSION}/x${GORGON_TARGET_ARCH})

#list(APPEND GORGON_INCLUDE_DIRS 
#	${CMAKE_CURRENT_SOURCE_DIR}
#	${GORGON_EXTERNAL_LIBRARIES_DIR}
#	TODO: Is this line necessary? Removing works on Mac 
#	${GORGON_EXTERNAL_LIBRARIES_DIR}/GL
#	)
# --------------------------------------------------------------------
# Set target name vars
# --------------------------------------------------------------------
set(gorgon_lib_target_name pygorgon)
set(core_lib_target_name core)
# --------------------------------------------------------------------
configure_file(${CMAKE_SOURCE_DIR}/gorgon.cpp.in
                ${CMAKE_SOURCE_DIR}/gorgon.cpp
               )
# --------------------------------------------------------------------
add_subdirectory(${core_lib_target_name})
add_library(${gorgon_lib_target_name} MODULE gorgon.cpp $<TARGET_OBJECTS:${core_lib_target_name}>)

include_directories(${GORGON_INCLUDE_DIRS} ${core_lib_target_name})
target_link_libraries(${gorgon_lib_target_name} ${GORGON_LIBRARIES})

#set(cxx11_flags "-std=c++0x --stdlib=libc++")
#list( APPEND CMAKE_CXX_FLAGS           ${cxx11_flags})
#list( APPEND CMAKE_SHARED_LINKER_FLAGS ${cxx11_flags})

if(ENABLE_CMAKE_DEBUG_OUTPUT)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

    foreach(dir ${dirs})
      message(STATUS "DIRS='${dir}'")
    endforeach()

    get_property(dirs TARGET gorgon PROPERTY LINK_LIBRARIES)    

    foreach(dir ${GORGON_LIBRARIES})
      message("LIBS='${dir}'")
    endforeach()
endif()
# --------------------------------------------------------------------
#Install Step
# --------------------------------------------------------------------
#install(TARGETS ${gorgon_lib_target_name}
#        LIBRARY
#        DESTINATION ${CMAKE_BINARY_DIR}/bin/
#        COMPONENT "Gorgon"
#        )

install(TARGETS ${gorgon_lib_target_name}
        LIBRARY
        DESTINATION ${CMAKE_BINARY_DIR}/tests/
        COMPONENT "Gorgon"
        )

#install(TARGETS pyGORGON
#        LIBRARY
#        DESTINATION ${CMAKE_SOURCE_DIR}/TestSuite
#        COMPONENT "Gorgon"
#        )
#

file(GLOB py_srcs ${CMAKE_SOURCE_DIR}/../bin/*.py)

install(FILES ${py_srcs}
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ WORLD_READ
        DESTINATION ${CMAKE_BINARY_DIR}/bin/
        COMPONENT "Gorgon"
        )

file(GLOB py_tests ${CMAKE_SOURCE_DIR}/../tests/*.py)

install(FILES ${py_tests}
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ WORLD_READ
        DESTINATION ${CMAKE_BINARY_DIR}/tests/
        COMPONENT "Gorgon"
        )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/../gorg
        DESTINATION ${CMAKE_BINARY_DIR}/bin
        COMPONENT "Gorgon"
        FILES_MATCHING PATTERN "*.py"         
        )
        
install(DIRECTORY ${CMAKE_SOURCE_DIR}/../demo
        DESTINATION ${CMAKE_BINARY_DIR}
        COMPONENT "Gorgon"
        )
        
add_custom_target(Gorgon
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Gorgon -P cmake_install.cmake
        )
# --------------------------------------------------------------------
#Package Step
# --------------------------------------------------------------------
#install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/setup.py py2app
#                                                    --bdist-base=${CMAKE_BINARY_DIR}/package/build
#                                                    --dist-dir=${CMAKE_BINARY_DIR}/package/dist
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Gorgon/src_py/)"
#    COMPONENT "Package-Dist"
#    )
#
#add_custom_target(Package-Dist
#        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Package-Dist -P cmake_install.cmake
#        )
#
#install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E rename
#                                        package/dist/Gorgon.app/Contents/Frameworks/libpyGORGON.so
#                                        package/dist/Gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/libpyGORGON.so
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
#    COMPONENT "Package-Dist-Post"
#    )

#add_custom_target(Package-Dist-Post
#        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Package-Dist-Post -P cmake_install.cmake
#        )
