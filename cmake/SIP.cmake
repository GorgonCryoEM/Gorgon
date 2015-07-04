external_project_vars( SIP
        Python
        http://downloads.sourceforge.net/project/pyqt/sip/sip-4.16.8/sip-4.16.8.tar.gz
        "${PYTHON_EXECUTABLE};configure.py"
        "${CMAKE_MAKE_PROGRAM}"
        "${CMAKE_MAKE_PROGRAM};install"
    )
