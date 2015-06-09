
set(ext_dir ${CMAKE_SOURCE_DIR}/ExternalLibraries/GL/${folder_name})

message("          GLUT: ${ext_dir}")

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