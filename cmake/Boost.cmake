message("Dependency - Boost")

# Windows needs `libbost_serialization` for some reason
if(WIN32)
    set(boost_components python serialization)
else()
    set(boost_components python)
endif()

# Prepare options for all the components needed
set(boost_options)
foreach(comp ${boost_components})
    list(APPEND boost_options --with-${comp})
endforeach()

set(Boost_USE_MULTITHREADED ON)
  
find_package(Boost 1.41 COMPONENTS ${boost_components})

# Set variables
set(boost_root           ${CMAKE_SOURCE_DIR}/ExternalLibraries/boost )
set(boost_librarydir     ${boost_root}/lib                           )

set(boost_url            http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz )
set(boost_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
set(boost_url_md5        5a5d5614d9a07672e1ab2a250b5defc5                                                )
set(boost_install_prefix ${CMAKE_SOURCE_DIR}/ExternalLibraries/boost/                                    )

if( NOT Boost_FOUND)
    set(BOOST_ROOT       ${boost_root}       CACHE PATH "Boost root directory" )
    set(BOOST_LIBRARYDIR ${boost_librarydir} CACHE PATH "Boost library directory")
    find_package(Boost 1.41 COMPONENTS ${boost_components})
endif()

# If not found so far, donwload, build and install Boost
if( NOT Boost_FOUND)
    if( UNIX )
      set( Boost_Bootstrap_CMD ./bootstrap.sh )
      set( Boost_b2_CMD ./b2 )
    else()
      if( WIN32 )
        set( Boost_Bootstrap_CMD bootstrap.bat )
        set( Boost_b2_CMD b2.exe )
      endif()
    endif()
    
    include(ExternalProject)
    
    ExternalProject_Add( Boost
    	PREFIX boost
     #--Download step--------------
        URL           ${boost_url}
        URL_HASH SHA1=${boost_url_sha1}
        URL_MD5       ${boost_url_md5}
     #--Configure step-------------
        CONFIGURE_COMMAND  ${Boost_Bootstrap_CMD}
     #--Build step-----------------
        BUILD_COMMAND ${Boost_b2_CMD}    ${boost_options}
        BUILD_IN_SOURCE 1
     #--Install step---------------
        INSTALL_COMMAND ${Boost_b2_CMD} install --prefix=${boost_install_prefix} ${boost_options}
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
endif()

include_directories(${Boost_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Boost DEFAULT_MSG Boost_LIBRARIES Boost_LIBRARY_DIR Boost_INCLUDE_DIR)
MARK_AS_ADVANCED(Boost_LIBRARIES Boost_LIBRARY_DIR Boost_INCLUDE_DIR)
