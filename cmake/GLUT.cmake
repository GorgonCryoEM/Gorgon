message("Dependency - FFTW3")

# Set variables
set(glut_root           ${extlibs_dir}/GL  )

set(glut_win32  Win              )
set(glut_win64  ""               )
set(glut_linux  Linux_CentOS_6.6 )
set(glut_mac    MacOSX           )
set(glut_source freeglut-3.0.0   )

# Set top directory
if(WIN32)
    set(glut_root    ${glut_root}/${fftw_win32}  )
elseif(APPLE)
    set(glut_root    ${glut_root}/${fftw_mac}  )
else()
    set(glut_root    ${glut_root}/${fftw_linux}  )
endif()

# Set include & library directories
set(glut_includedir   ${glut_root}/include )
if(WIN32)
    set(glut_librarydir   ${glut_root}/lib     )
elseif(UNIX AND NOT APPLE)
    set(glut_librarydir   ${glut_root}/lib64         )
endif()

set( include_folder "${ext_dir}/include/GL" )
set( lib_folder "${ext_dir}/lib" )

message("          GLUT: ${glut_root}")
message("          GLUT include: ${include_folder}")
message("          GLUT lib    : ${lib_folder}")


FIND_PATH(
    GLUT_INCLUDE_DIR
    NAMES glut.h
    PATHS ${include_folder}
    #NO_DEFAULT_PATH
)

FIND_LIBRARY(
    GLUT_glut_LIBRARY
    NAMES freeglut
    PATHS ${lib_folder}
    #NO_DEFAULT_PATH
)


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLUT DEFAULT_MSG GLUT_glut_LIBRARY GLUT_INCLUDE_DIR)
#MARK_AS_ADVANCED(FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIRS FFTW3F_THREADS_LIBRARIES)