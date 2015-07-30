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

set(python_libs_dir       ${python_install_prefix}/${python_framework_extra_path}/lib/python2.7 CACHE INTERNAL "Python directory for lib-dynload & site-settings")
set(python_lib-dynload    ${python_libs_dir}/lib-dynload                                        CACHE INTERNAL "Python path for lib-dynload"                     )
set(python_site-packages  ${python_libs_dir}/site-packages                                      CACHE INTERNAL "Python path for site-packages"                   )
