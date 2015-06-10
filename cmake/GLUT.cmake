message("Dependency - FFTW3")

# Set variables
set(glut_root           ${extlibs_dir}/GL  )

set(glut_win32  Win              )
set(glut_win64  ""               )
set(glut_linux  Linux_CentOS_6.6 )
set(glut_mac    MacOSX           )
set(glut_source freeglut-3.0.0   )

message("          GLUT: ${glut_root}")

#if(NOT WIN32)
#    set( include_folder "${ext_dir}/include" )
#    set( lib_folder "${ext_dir}/lib" )
#else()
#    set( include_folder ${ext_dir} )
#    set( lib_folder     ${ext_dir} )
#endif()

set( include_folder "${ext_dir}/include/GL" )
set( lib_folder "${ext_dir}/lib" )

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