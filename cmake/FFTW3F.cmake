external_project_vars(
        ""
        ${GORGON_EXTERNAL_LIBRARIES_DIR}/FFTW/fftw-3.3.4-source
        "./configure;--prefix=${fftw3f_install_prefix};--enable-shared;--enable-float"
        "${CMAKE_MAKE_PROGRAM};-j${NUMBER_OF_PARALLEL_JOBS}"
        "${CMAKE_MAKE_PROGRAM};install"
    )

OPTION(ENABLE_FFTW3 "enable fftw 3 support" ON)
OPTION(ENABLE_FFTW_PLAN_CACHING "enable fftw caching" ON)

IF(ENABLE_FFTW3)
	ADD_DEFINITIONS(-DFFTW3)
ENDIF()

IF(ENABLE_FFTW_PLAN_CACHING)
	ADD_DEFINITIONS(-DFFTW_PLAN_CACHING)
ENDIF(ENABLE_FFTW_PLAN_CACHING)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${fftw3f_install_prefix})
find_package(FFTW3F)

#list(APPEND GORGON_LIBRARIES    ${FFTW3F_LIBRARIES}   )
#list(APPEND GORGON_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIRS})
update_libs_includes(FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIRS)
