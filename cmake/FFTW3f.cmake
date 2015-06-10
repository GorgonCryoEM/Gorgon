message("Dependency - Boost")

# Set variables
set(boost_version 1.41)

set(boost_root           ${extlibs_dir}/boost  )
set(boost_librarydir     ${boost_root}/lib     )
set(boost_includedir     ${boost_root}/include )


set(ext_dir ${CMAKE_SOURCE_DIR}/ExternalLibraries/FFTW/${folder_name})

message("          FFTW: ${ext_dir}")

if(NOT WIN32)
    set( include_folder "${ext_dir}/include" )
    set( lib_folder "${ext_dir}/lib" )
else()
    set( include_folder ${ext_dir} )
    set( lib_folder     ${ext_dir} )
endif()

message("          FFTW include: ${include_folder}")
message("          FFTW lib    : ${lib_folder}")

FIND_PATH(
    FFTW3F_INCLUDE_DIRS
    NAMES fftw3.h
    PATHS ${include_folder}
    NO_DEFAULT_PATH
)

FIND_LIBRARY(
    FFTW3F_LIBRARIES
    NAMES fftw3f fftw3f-3 libfftw3f-3
    PATHS ${lib_folder}
    NO_DEFAULT_PATH
)

#FIND_LIBRARY(
#    FFTW3F_THREADS_LIBRARIES
#    NAMES fftw3f_threads libfftw3f_threads
#    HINTS $ENV{FFTW3_DIR}/lib
#        ${PC_FFTW3F_LIBDIR}
#    PATHS /usr/local/lib
#          /usr/lib
#          /usr/lib64
#)


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW3F DEFAULT_MSG FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIRS)
MARK_AS_ADVANCED(FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIRS FFTW3F_THREADS_LIBRARIES)