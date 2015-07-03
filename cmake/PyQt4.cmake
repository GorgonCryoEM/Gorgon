external_project_vars( PyQt4
        SIP #Qt4
        http://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-4.11.4/PyQt-x11-gpl-4.11.4.tar.gz
        "${PYTHON_EXECUTABLE};configure-ng.py;--confirm-license;--sip;${CMAKE_BINARY_DIR}/python/Python.framework/Versions/2.7/bin/sip"
        "${CMAKE_MAKE_PROGRAM};-j${NUMBER_OF_PARALLEL_JOBS}"
        "${CMAKE_MAKE_PROGRAM};install"
    )

external_project_find_paths(PyQt4)
