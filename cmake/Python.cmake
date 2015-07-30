find_package(PythonInterp)
find_package(PythonLibs)

#list(APPEND GORGON_LIBRARIES    ${PYTHON_LIBRARY}    )
#list(APPEND GORGON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIR})
update_libs_includes(PYTHON_LIBRARY PYTHON_INCLUDE_DIR)

set(Python_lib-dynload    "FIND-ME"  CACHE PATH "Python path for lib-dynload"  )
set(Python_site-packages  "FIND-ME"  CACHE PATH "Python path for site-packages, needed for pip")
