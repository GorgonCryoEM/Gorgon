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
  
    external_project_build( 
        FFTW3F
        ""
    	";"
        ${fftw3_url}
#        URL_HASH SHA1=${fftw3_url_sha1}
#        URL_MD5       ${fftw3_url_md5}
#        CONFIGURE_COMMAND  ./configure --prefix=${fftw3_install_prefix} --enable-shared
        "./configure;--prefix=${CMAKE_CURRENT_BINARY_DIR}/fftw3f;--enable-shared;--enable-float"
#        BUILD_COMMAND ${fftw3_b2_CMD}    ${fftw3_options}
         "${CMAKE_MAKE_PROGRAM}"
#         INSTALL_DIR fftw3
#        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install --prefix=${fftw3_install_prefix}
         "${CMAKE_MAKE_PROGRAM};install"
     #--Custom targets-------------
     # [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
    )

#find_package(FFTW3F)

#include_directories(${FFTW3F_INCLUDE_DIRS})
ExternalProject_Get_Property(FFTW3F INSTALL_DIR)
set(FFTW3F_LIBRARIES     ${INSTALL_DIR}/lib/libfftw3f.dylib CACHE FILEPATH "")


set(FFTW3F_INCLUDE_DIRS ${INSTALL_DIR}/include/ CACHE PATH "")
#set(FFTW3F_EXECUTABLE ${INSTALL_DIR}/bin/python CACHE PATH "")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW3F DEFAULT_MSG FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIRS)
MARK_AS_ADVANCED(FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIRS)


OPTION(ENABLE_FFTW3 "enable fftw 3 support" ON)
OPTION(ENABLE_FFTW_PLAN_CACHING "enable fftw caching" ON)

IF(ENABLE_FFTW3)
	ADD_DEFINITIONS(-DFFTW3)
ENDIF()

IF(ENABLE_FFTW_PLAN_CACHING)
	ADD_DEFINITIONS(-DFFTW_PLAN_CACHING)
ENDIF(ENABLE_FFTW_PLAN_CACHING)

