external_project_vars( PyQt4
    http://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-4.11.4/PyQt-x11-gpl-4.11.4.tar.gz
    )

# If not found so far, donwload, build and install pyqt4
#if( NOT PyQt4_FOUND)
  
    external_project_build( 
         PyQt4
         SIP #Qt4
    	";"
           ${pyqt4_url}
#        URL_HASH SHA1=${pyqt4_url_sha1}
#        URL_MD5       ${pyqt4_url_md5}
#        CONFIGURE_COMMAND  ./configure --prefix=${pyqt4_install_prefix} --enable-shared
        "${PYTHON_EXECUTABLE};configure-ng.py;--confirm-license;--sip;${CMAKE_BINARY_DIR}/python/Python.framework/Versions/2.7/bin/sip"
         "${CMAKE_MAKE_PROGRAM};-j${NUMBER_OF_PARALLEL_JOBS}"
         "${CMAKE_MAKE_PROGRAM};install"
#         INSTALL_DIR pyqt4
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${pyqt4_install_prefix}
     #--Custom targets-------------
     # [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
    )

ExternalProject_Get_Property(PyQt4 INSTALL_DIR)
#find_package(PyQt4Libs)
