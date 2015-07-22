if(APPLE)
    set(enable_shared --enable-framework=${python_install_prefix})
    set(config_cmd "./configure;--prefix=${python_install_prefix};--with-pydebug;${enable_shared};--with-ensurepip=install;--enable-unicode=ucs4")
    set(python_libs_dir       ${python_install_prefix}/Python.framework/Versions/2.7/lib/python2.7  CACHE INTERNAL "Python directory for lib-dynload & site-settings")
else()
    set(enable_shared --enable-shared)
    set(config_cmd "./configure;--prefix=${python_install_prefix};--with-pydebug;${enable_shared};--with-ensurepip=install;--enable-unicode=ucs4;LDFLAGS=-Wl,-rpath,${python_install_prefix}/lib")
    set(python_libs_dir       ${python_install_prefix}/lib/python2.7  CACHE INTERNAL "Python directory for lib-dynload & site-settings")
endif()

external_project_vars(
        ""
        https://www.python.org/ftp/python/2.7.10/Python-2.7.10.tgz
        "${config_cmd}"
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

set(python_lib-dynload    ${python_libs_dir}/lib-dynload                                        CACHE INTERNAL "Python path for lib-dynload"                     )
set(python_site-packages  ${python_libs_dir}/site-packages                                      CACHE INTERNAL "Python path for site-packages"                   )
