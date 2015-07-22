if( NOT ${QT_QMAKE_EXECUTABLE})
    set(qmake_arg "--qmake=${QT_QMAKE_EXECUTABLE}")
else()
    unset(qmake_arg)
endif()

external_project_vars(
        SIP #Qt4
#        http://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-4.11.4/PyQt-x11-gpl-4.11.4.tar.gz
#        http://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-4.11.4/PyQt-mac-gpl-4.11.4.tar.gz
""
        "${PYTHON_EXECUTABLE};configure-ng.py;--confirm-license;--sip;${sip_bindir}/${sip_sip-module};${qmake_arg};--static"
        "${CMAKE_MAKE_PROGRAM};-j${NUMBER_OF_PARALLEL_JOBS}"
        "${CMAKE_MAKE_PROGRAM};install"
    )

set(sip_url_md5_cmd        URL_MD5       2fe8265b2ae2fc593241c2c84d09d481  CACHE INTERNAL "")
