message("Dependency - FFTW3")

# Set variables
set(fftw_root           ${extlibs_dir}/FFTW  )

set(fftw_win32  fftw-3.3.4-dll32            )
set(fftw_win64  fftw-3.3.4-dll64            )
set(fftw_linux  fftw-3.3.4-Linux_CentOS_6.6 )
set(fftw_mac    fftw-3.3.4-MacOSX_10.10     )
set(fftw_source fftw-3.3.4-source           )

# Set top directory
if(WIN32)
    set(fftw_root    ${fftw_root}/${fftw_win32}  )
elseif(APPLE)
    set(fftw_root    ${fftw_root}/${fftw_mac}  )
else()
    set(fftw_root    ${fftw_root}/${fftw_linux}  )
endif()

if(WIN32)
    set(fftw_includedir   ${fftw_root}         )
    set(fftw_librarydir   ${fftw_root}         )
else()
    set(fftw_includedir   ${fftw_root}/include )
    set(fftw_librarydir   ${fftw_root}/lib     )
endif()

message("          FFTW: ${fftw_root}")
message("          FFTW include: ${fftw_includedir}")
message("          FFTW lib    : ${fftw_librarydir}")

FIND_PATH(
    FFTW3F_INCLUDE_DIRS
    NAMES fftw3.h
    PATHS ${fftw_includedir}
    NO_DEFAULT_PATH
)

FIND_LIBRARY(
    FFTW3F_LIBRARIES
    NAMES fftw3f fftw3f-3 libfftw3f-3
    PATHS ${fftw_librarydir}
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