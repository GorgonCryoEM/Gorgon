message("Dependency - GLUT")

# Set variables
set(glut_root           ${extlibs_dir}/GL  )

set(glut_win32  Win              )
set(glut_win64  ""               )
set(glut_linux  Linux_CentOS_6.6 )
set(glut_source freeglut-3.0.0   )

# Set top directory
if(WIN32)
    set(glut_root    ${glut_root}/${glut_win32}  )
elseif(UNIX AND NOT APPLE)
    set(glut_root    ${glut_root}/${glut_linux}  )
endif()

# Set include & library directories
set(glut_includedir   ${glut_root}/include/GL )

if(WIN32)
    if(target_arch eq 32)
        set(glut_librarydir   ${glut_root}/lib     )
    else()
        set(glut_librarydir   ${glut_root}/lib/x64 )
    endif()
elseif(APPLE)
    find_package(GLUT REQUIRED)
else()
    set(glut_librarydir   ${glut_root}/lib64   )
endif()

message("          GLUT: ${glut_root}")
message("          GLUT include: ${glut_includedir}")
message("          GLUT lib    : ${glut_librarydir}")


FIND_PATH(
    GLUT_INCLUDE_DIR
    NAMES glut.h
    PATHS ${glut_includedir}
    NO_DEFAULT_PATH
)

FIND_LIBRARY(
    GLUT_glut_LIBRARY
    NAMES freeglut
    PATHS ${glut_librarydir}
    NO_DEFAULT_PATH
)


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLUT DEFAULT_MSG GLUT_glut_LIBRARY GLUT_INCLUDE_DIR)
MARK_AS_ADVANCED(GLUT_glut_LIBRARY GLUT_INCLUDE_DIR)