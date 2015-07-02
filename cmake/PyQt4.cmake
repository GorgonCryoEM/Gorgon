if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("Dependency - PyQt4")
endif()

set(pyqt4_root           ${GORGON_EXTERNAL_LIBRARIES_DIR}/pyqt4  )
set(pyqt4_librarydir     ${pyqt4_root}/lib     )
set(pyqt4_includedir     ${pyqt4_root}/include )

set(pyqt4_url            http://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-4.11.4/PyQt-x11-gpl-4.11.4.tar.gz )
#set(pyqt4_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
#set(pyqt4_url_md5        d7547558fd673bd9d38e2108c6b42521                                                )
set(pyqt4_install_prefix ${GORGON_EXTERNAL_LIBRARIES_DIR}/pyqt4/                                                           )

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

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PyQt4 DEFAULT_MSG PyQt4_LIBRARY PyQt4_INCLUDE_DIR PyQt4_EXECUTABLE)
MARK_AS_ADVANCED(PyQt4_LIBRARY PyQt4_INCLUDE_DIR PyQt4_EXECUTABLE)
