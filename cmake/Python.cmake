external_project_vars(
        ""
        https://www.python.org/ftp/python/2.7.10/Python-2.7.10.tgz
        "./configure;--prefix=${python_install_prefix};--with-pydebug;--enable-framework=${python_install_prefix};--with-ensurepip=install"
        "${CMAKE_MAKE_PROGRAM};-j${NUMBER_OF_PARALLEL_JOBS}"
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

#list(APPEND GORGON_LIBRARIES    ${PYTHON_LIBRARY}    )
#list(APPEND GORGON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIR})
update_libs_includes(PYTHON_LIBRARY PYTHON_INCLUDE_DIR)

set(python_url_md5_cmd        URL_MD5       d7547558fd673bd9d38e2108c6b42521  CACHE INTERNAL "")
