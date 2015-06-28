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
  
    include(ExternalProject)
    
    ExternalProject_Add( PyQt4
    DEPENDS SIP #Qt4
    	PREFIX pyqt4
     #--Download step--------------
        URL           ${pyqt4_url}
#        URL_HASH SHA1=${pyqt4_url_sha1}
#        URL_MD5       ${pyqt4_url_md5}
     #--Configure step-------------
#        CONFIGURE_COMMAND  ./configure --prefix=${pyqt4_install_prefix} --enable-shared
        CONFIGURE_COMMAND  ${PYTHON_EXECUTABLE} configure-ng.py --confirm-license
     #--Build step-----------------
#        BUILD_COMMAND ${pyqt4_b2_CMD}    ${pyqt4_options}
        BUILD_IN_SOURCE 1
     #--Install step---------------
#         INSTALL_DIR pyqt4
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${pyqt4_install_prefix}
     #--Output logging-------------
      LOG_DOWNLOAD 1            # Wrap download in script to log output
      LOG_UPDATE 1              # Wrap update in script to log output
      LOG_CONFIGURE 1           # Wrap configure in script to log output
      LOG_BUILD 1               # Wrap build in script to log output
      LOG_TEST 1                # Wrap test in script to log output
      LOG_INSTALL 1             # Wrap install in script to log output
     #--Custom targets-------------
     # [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
    )

ExternalProject_Get_Property(PyQt4 INSTALL_DIR)
#find_package(PyQt4Libs)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PyQt4 DEFAULT_MSG PyQt4_LIBRARY PyQt4_INCLUDE_DIR PyQt4_EXECUTABLE)
MARK_AS_ADVANCED(PyQt4_LIBRARY PyQt4_INCLUDE_DIR PyQt4_EXECUTABLE)
