set(python_root           ${GORGON_EXTERNAL_LIBRARIES_DIR}/python  )
set(python_librarydir     ${python_root}/lib     )
set(python_includedir     ${python_root}/include )

set(python_url            https://www.python.org/ftp/python/2.7.10/Python-2.7.10.tgz )
#set(python_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
set(python_url_md5        d7547558fd673bd9d38e2108c6b42521                                                )
set(python_install_prefix ${GORGON_EXTERNAL_LIBRARIES_DIR}/python/                                                           )

# If not found so far, donwload, build and install python
#if( NOT python_FOUND)
  
    external_project_build(
            Python
            ""
            ";"
               ${python_url}
##        URL_HASH SHA1=${python_url_sha1}
#        URL_MD5       ${python_url_md5}
#        CONFIGURE_COMMAND  ./configure --prefix=${python_install_prefix} --enable-shared
         "./configure;--prefix=${CMAKE_CURRENT_BINARY_DIR}/python;--with-pydebug;--enable-framework=${CMAKE_CURRENT_BINARY_DIR}/python"
         "${CMAKE_MAKE_PROGRAM}"
         "${CMAKE_MAKE_PROGRAM};install"
#        BUILD_COMMAND ${python_b2_CMD}    ${python_options}
#         INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${python_install_prefix}
     # [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
    )

ExternalProject_Get_Property(Python INSTALL_DIR)
set(CMAKE_PREFIX_PATH ${INSTALL_DIR})
find_package(PythonInterp)
#set(CMAKE_PREFIX_PATH ${INSTALL_DIR})
#set(CMAKE_LIBRARY_PATH ${INSTALL_DIR})
find_library(PYTHON_LIBRARY
                name python
                PATHS ${INSTALL_DIR}
#                NO_DEFAULT_PATH
                )
find_package(PythonLibs)
#message("PYTHON_LIBRARIES= ${PYTHON_LIBRARIES}")
#include_directories(${PYTHON_INCLUDE_DIR})
#set(Python_LIBRARY     ${INSTALL_DIR}/lib/libpython2.7.dylib CACHE FILEPATH "")

#include(cmake/LibFindMacros.cmake)
# Dependencies

#find_library(PYTHON_LIBRARY
#                name python
#                PATHS ${INSTALL_DIR}
#                NO_DEFAULT_PATH
#                )
#set(Python_LIBRARY_DIR ${python_install_dir})
#set(PYTHON_INCLUDE_DIR ${INSTALL_DIR}/include/python2.7 CACHE PATH "")
#set(PYTHON_EXECUTABLE ${INSTALL_DIR}/bin/python CACHE PATH "")
#set(PYTHON_LIBRARY_DIR ${INSTALL_DIR}/lib CACHE PATH "")

#set(CMAKE_FIND_FRAMEWORK NEVER)
#find_path(Python_INCLUDE_DIR
#                Python.h
#                PATHS ${INSTALL_DIR}/include/
#                NO_DEFAULT_PATH
#                )
