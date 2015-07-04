external_project_vars( SIP
        Python
        http://downloads.sourceforge.net/project/pyqt/sip/sip-4.16.8/sip-4.16.8.tar.gz
        "${PYTHON_EXECUTABLE};configure.py;${sip_extra_options}"
        "${CMAKE_MAKE_PROGRAM}"
        "${CMAKE_MAKE_PROGRAM};install"
        "bindir;${python_install_prefix}/bin"
        "sip-module;sip"
    )
    