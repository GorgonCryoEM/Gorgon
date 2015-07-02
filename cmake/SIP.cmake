set(sip_root           ${GORGON_EXTERNAL_LIBRARIES_DIR}/sip  )
set(sip_librarydir     ${sip_root}/lib     )
set(sip_includedir     ${sip_root}/include )

set(sip_url            http://downloads.sourceforge.net/project/pyqt/sip/sip-4.16.8/sip-4.16.8.tar.gz )
#set(sip_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
#set(sip_url_md5        d7547558fd673bd9d38e2108c6b42521                                                )
set(sip_install_prefix ${GORGON_EXTERNAL_LIBRARIES_DIR}/sip/                                                           )

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
