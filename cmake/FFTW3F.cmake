external_project_vars( FFTW3F
        ""
        ${GORGON_EXTERNAL_LIBRARIES_DIR}/FFTW/fftw-3.3.4-source
        "./configure;--prefix=${CMAKE_CURRENT_BINARY_DIR}/fftw3f;--enable-shared;--enable-float"
        "${CMAKE_MAKE_PROGRAM}"
        "${CMAKE_MAKE_PROGRAM};install"
    )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(FFTW3F_find FFTW3F)
external_project_find_paths(FFTW3F)

OPTION(ENABLE_FFTW3 "enable fftw 3 support" ON)
OPTION(ENABLE_FFTW_PLAN_CACHING "enable fftw caching" ON)

IF(ENABLE_FFTW3)
	ADD_DEFINITIONS(-DFFTW3)
ENDIF()

IF(ENABLE_FFTW_PLAN_CACHING)
	ADD_DEFINITIONS(-DFFTW_PLAN_CACHING)
ENDIF(ENABLE_FFTW_PLAN_CACHING)