external_project_vars(
        Python
        http://downloads.sourceforge.net/project/pyqt/sip/sip-4.16.8/sip-4.16.8.tar.gz
        "${PYTHON_EXECUTABLE};configure.py;${sip_extra_options}"
        "${CMAKE_MAKE_PROGRAM};-j${NUMBER_OF_PARALLEL_JOBS}"
        "${CMAKE_MAKE_PROGRAM};install"
        "bindir;${python_install_prefix}/bin"
        "sip-module;sip"
    )

set(sip_url_md5_cmd        URL_MD5       8e48cd78bcd31f9310a701a4111c2739  CACHE INTERNAL "")
