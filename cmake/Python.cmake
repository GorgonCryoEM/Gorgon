if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("Dependency - Python")
endif()
#find_package(PythonInterp)

set(python_root           ${GORGON_EXTERNAL_LIBRARIES_DIR}/python  )
set(python_librarydir     ${python_root}/lib     )
set(python_includedir     ${python_root}/include )

set(python_url            https://www.python.org/ftp/python/2.7.10/Python-2.7.10.tgz )
#set(python_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
set(python_url_md5        d7547558fd673bd9d38e2108c6b42521                                                )
set(python_install_prefix ${GORGON_EXTERNAL_LIBRARIES_DIR}/python/                                                           )

# If not found so far, donwload, build and install python
#if( NOT python_FOUND)
  
    include(ExternalProject)
    
    ExternalProject_Add( Python
    	PREFIX python
     #--Download step--------------
        URL           ${python_url}
#        URL_HASH SHA1=${python_url_sha1}
        URL_MD5       ${python_url_md5}
     #--Configure step-------------
#        CONFIGURE_COMMAND  ./configure --prefix=${python_install_prefix} --enable-shared
        CONFIGURE_COMMAND  ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/python --enable-shared
     #--Build step-----------------
#        BUILD_COMMAND ${python_b2_CMD}    ${python_options}
        BUILD_IN_SOURCE 1
     #--Install step---------------
#         INSTALL_DIR python
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${python_install_prefix}
     #--Output logging-------------
      LOG_DOWNLOAD 1            # Wrap download in script to log output
      LOG_UPDATE 1              # Wrap update in script to log output
      LOG_CONFIGURE 1           # Wrap configure in script to log output
      LOG_BUILD 1               # Wrap build in script to log output
      LOG_TEST 1                # Wrap test in script to log output
      LOG_INSTALL 1             # Wrap install in script to log output
     #--Custom targets-------------
     # [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
    )

#find_package(PythonInterp)
#find_package(PythonLibs REQUIRED)

#include_directories(${PYTHON_INCLUDE_DIR})
ExternalProject_Get_Property(Python INSTALL_DIR)
#set(Python_LIBRARY     ${INSTALL_DIR}/lib/libpython2.7.dylib CACHE FILEPATH "")

#find_package(PkgConfig)
#pkg_search_module(PYTHON REQUIRED python)

#include(cmake/LibFindMacros.cmake)
# Dependencies
#libfind_package(PYTHON python)
# Use pkg-config to get hints about paths
#libfind_pkg_check_modules(PYTHON_PKGCONF python)
#libfind_pkg_detect(Python python FIND_PATH Python.h FIND_LIBRARY python)

find_library(PYTHON_LIBRARY
                name python2.7
                PATHS ${INSTALL_DIR}/lib
                NO_DEFAULT_PATH
                )
#set(Python_LIBRARY_DIR ${python_install_dir})
set(PYTHON_INCLUDE_DIR ${INSTALL_DIR}/include/python2.7 CACHE PATH "")
set(PYTHON_EXECUTABLE ${INSTALL_DIR}/bin/python CACHE PATH "")
#set(PYTHON_LIBRARY_DIR ${INSTALL_DIR}/lib CACHE PATH "")

#set(CMAKE_FIND_FRAMEWORK NEVER)
#find_path(Python_INCLUDE_DIR
#                Python.h
#                PATHS ${INSTALL_DIR}/include/
#                NO_DEFAULT_PATH
#                )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python DEFAULT_MSG PYTHON_LIBRARY PYTHON_INCLUDE_DIR PYTHON_EXECUTABLE)
MARK_AS_ADVANCED(PYTHON_LIBRARY PYTHON_INCLUDE_DIR PYTHON_EXECUTABLE)
