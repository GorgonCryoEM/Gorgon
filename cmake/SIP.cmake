external_project_vars( SIP
    http://downloads.sourceforge.net/project/pyqt/sip/sip-4.16.8/sip-4.16.8.tar.gz
    )

# If not found so far, donwload, build and install sip
#if( NOT SIP_FOUND)
  
    external_project_build( 
     SIP
     Python
    ";"
           ${sip_url}
#        URL_HASH SHA1=${sip_url_sha1}
#        URL_MD5       ${sip_url_md5}
#        CONFIGURE_COMMAND  ./configure --prefix=${sip_install_prefix} --enable-shared
        "${PYTHON_EXECUTABLE};configure.py"
#        BUILD_COMMAND ${sip_b2_CMD}    ${sip_options}
         "${CMAKE_MAKE_PROGRAM}"
         "${CMAKE_MAKE_PROGRAM};install"
     #--Install step---------------
#         INSTALL_DIR sip
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${sip_install_prefix}
     #--Custom targets-------------
     # [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
    )

ExternalProject_Get_Property(SIP INSTALL_DIR)
#find_package(SIPLibs)
