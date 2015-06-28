if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("Dependency - SIP")
endif()

set(sip_root           ${GORGON_EXTERNAL_LIBRARIES_DIR}/sip  )
set(sip_librarydir     ${sip_root}/lib     )
set(sip_includedir     ${sip_root}/include )

set(sip_url            http://downloads.sourceforge.net/project/pyqt/sip/sip-4.16.8/sip-4.16.8.tar.gz )
#set(sip_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
set(sip_url_md5        d7547558fd673bd9d38e2108c6b42521                                                )
set(sip_install_prefix ${GORGON_EXTERNAL_LIBRARIES_DIR}/sip/                                                           )

# If not found so far, donwload, build and install sip
#if( NOT sip_FOUND)
  
    include(ExternalProject)
    
    ExternalProject_Add( SIP
    	PREFIX sip
     #--Download step--------------
        URL           ${sip_url}
#        URL_HASH SHA1=${sip_url_sha1}
        URL_MD5       ${sip_url_md5}
     #--Configure step-------------
#        CONFIGURE_COMMAND  ./configure --prefix=${sip_install_prefix} --enable-shared
        CONFIGURE_COMMAND  ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/sip --with-pydebug --enable-framework=${CMAKE_CURRENT_BINARY_DIR}/sip
     #--Build step-----------------
#        BUILD_COMMAND ${sip_b2_CMD}    ${sip_options}
        BUILD_IN_SOURCE 1
     #--Install step---------------
#         INSTALL_DIR sip
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${sip_install_prefix}
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

ExternalProject_Get_Property(SIP INSTALL_DIR)
#find_package(SIPLibs)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SIP DEFAULT_MSG SIP_LIBRARY SIP_INCLUDE_DIR SIP_EXECUTABLE)
MARK_AS_ADVANCED(SIP_LIBRARY SIP_INCLUDE_DIR SIP_EXECUTABLE)
