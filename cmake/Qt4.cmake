if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("Dependency - Qt4")
endif()

set(qt4_root           ${GORGON_EXTERNAL_LIBRARIES_DIR}/qt4  )
set(qt4_librarydir     ${qt4_root}/lib     )
set(qt4_includedir     ${qt4_root}/include )

set(qt4_url            http://download.qt.io/archive/qt/4.8/4.8.6/qt-everywhere-opensource-src-4.8.6.tar.gz )
set(qt4_url_sha1       ddf9c20ca8309a116e0466c42984238009525da6                                        )
set(qt4_url_md5        2edbe4d6c2eff33ef91732602f3518eb                                                )
set(qt4_install_prefix ${GORGON_EXTERNAL_LIBRARIES_DIR}/qt4/                                                           )

# If not found so far, donwload, build and install qt4
#if( NOT Qt4_FOUND)
  
    include(ExternalProject)
    
    ExternalProject_Add( Qt4
    DEPENDS SIP Qt4
    	PREFIX qt4
     #--Download step--------------
        URL           ${qt4_url}
        URL_HASH SHA1=${qt4_url_sha1}
        URL_MD5       ${qt4_url_md5}
     #--Configure step-------------
#        CONFIGURE_COMMAND  ./configure --prefix=${qt4_install_prefix} --enable-shared
        CONFIGURE_COMMAND   ./configure -debug-and-release -developer-build
     #--Build step-----------------
#        BUILD_COMMAND ${qt4_b2_CMD}    ${qt4_options}
        BUILD_IN_SOURCE 1
     #--Install step---------------
#         INSTALL_DIR qt4
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${qt4_install_prefix}
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

ExternalProject_Get_Property(Qt4 INSTALL_DIR)
#find_package(Qt4Libs)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Qt4 DEFAULT_MSG Qt4_LIBRARY Qt4_INCLUDE_DIR Qt4_EXECUTABLE)
MARK_AS_ADVANCED(Qt4_LIBRARY Qt4_INCLUDE_DIR Qt4_EXECUTABLE)
