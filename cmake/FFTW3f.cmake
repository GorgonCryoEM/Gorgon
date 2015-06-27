if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("Dependency - FFTW3F")
endif()

set(fftw3_root           ${CMAKE_CURRENT_BINARY_DIR}/fftw  )
set(fftw3_librarydir     ${fftw3_root}/lib     )
set(fftw3_includedir     ${fftw3_root}/include )

set(fftw3_url            ${GORGON_EXTERNAL_LIBRARIES_DIR}/FFTW/fftw-3.3.4-source )
#set(fftw3_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
#set(fftw3_url_md5        d7547558fd673bd9d38e2108c6b42521                                                )
set(fftw3_install_prefix ${fftw3_root}                                                           )
  
    include(ExternalProject)
    
    ExternalProject_Add( FFTW3F
    	PREFIX fftw3
     #--Download step--------------
        URL           ${fftw3_url}
#        URL_HASH SHA1=${fftw3_url_sha1}
#        URL_MD5       ${fftw3_url_md5}
     #--Configure step-------------
#        CONFIGURE_COMMAND  ./configure --prefix=${fftw3_install_prefix} --enable-shared
        CONFIGURE_COMMAND  ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/fftw3 --enable-shared
     #--Build step-----------------
#        BUILD_COMMAND ${fftw3_b2_CMD}    ${fftw3_options}
        BUILD_IN_SOURCE 1
     #--Install step---------------
#         INSTALL_DIR fftw3
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${fftw3_install_prefix}
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

#find_package(FFTW3F)

#include_directories(${FFTW3F_INCLUDE_DIR})
ExternalProject_Get_Property(FFTW3F INSTALL_DIR)
set(FFTW3F_LIBRARY     ${INSTALL_DIR}/lib/libfftw3f.dylib CACHE FILEPATH "")


set(FFTW3F_INCLUDE_DIR ${INSTALL_DIR}/include/python2.7 CACHE PATH "")
#set(FFTW3F_EXECUTABLE ${INSTALL_DIR}/bin/python CACHE PATH "")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW3F DEFAULT_MSG FFTW3F_LIBRARY FFTW3F_INCLUDE_DIR)
MARK_AS_ADVANCED(FFTW3F_LIBRARY FFTW3F_INCLUDE_DIR)
