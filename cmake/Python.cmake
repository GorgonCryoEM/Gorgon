external_project_vars( Python
        ""
        https://www.python.org/ftp/python/2.7.10/Python-2.7.10.tgz
        "./configure;--prefix=${CMAKE_CURRENT_BINARY_DIR}/python;--with-pydebug;--enable-framework=${CMAKE_CURRENT_BINARY_DIR}/python"
        "${CMAKE_MAKE_PROGRAM}"
        "${CMAKE_MAKE_PROGRAM};install"
    )

set(CMAKE_PREFIX_PATH ${python_install_prefix})
find_package(PythonInterp)

find_library(PYTHON_LIBRARY
                name python
                PATHS ${python_install_prefix}
                NO_DEFAULT_PATH
                )

set(CMAKE_PREFIX_PATH ${python_install_prefix})
find_package(PythonLibs)


list(APPEND GORGON_LIBRARIES ${PYTHON_LIBRARY})

list(APPEND GORGON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIR})

#set(PYTHON_find PythonLibs)
#external_project_find_paths(PYTHON)

set(python_url_md5        d7547558fd673bd9d38e2108c6b42521                                                )
