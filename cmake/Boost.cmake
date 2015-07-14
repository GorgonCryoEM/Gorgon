if(ENABLE_CMAKE_DEBUG_OUTPUT)
    message("Dependency - Boost")
endif()

set(BOOST_LOCATION_OPTIONS Gorgon CACHE STRING "Boost location option")
set_property(CACHE BOOST_LOCATION_OPTIONS PROPERTY STRINGS Gorgon System)
# Set variables
set(boost_version 1.44)

set(boost_root           ${CMAKE_CURRENT_BINARY_DIR}/boost  )
set(boost_librarydir     ${boost_root}/lib     )
set(boost_includedir     ${boost_root}/include )

set(boost_url            http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz )
set(boost_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
set(boost_url_md5        5a5d5614d9a07672e1ab2a250b5defc5                                                )
#set(boost_install_prefix ${GORGON_EXTERNAL_LIBRARIES_DIR}/boost/                                         )
set(boost_install_prefix ${boost_root}                                           )

# Windows needs `libbost_serialization` for some reason
if(WIN32)
    set(boost_components python serialization)
    set(boost_options  --build-type=complete)
else()
    set(boost_options)
    set(boost_components python)
endif()

# Prepare options for all the components needed
foreach(comp ${boost_components})
    list(APPEND boost_options --with-${comp})
endforeach()

set(Boost_USE_MULTITHREADED ON)

if(BOOST_LOCATION_OPTIONS STREQUAL Gorgon)
    set(BOOST_ROOT       ${boost_root}       CACHE PATH "Boost root directory"   )
    set(BOOST_INCLUDEDIR ${boost_includedir} CACHE PATH "Boost include directory")
    set(BOOST_LIBRARYDIR ${boost_librarydir} CACHE PATH "Boost library directory")
 endif()

find_package(Boost ${boost_version} COMPONENTS ${boost_components})

# If not found so far, donwload, build and install Boost
#if( NOT Boost_FOUND)
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

#    --with-python=PYTHON      specify the Python executable [python]
#  --with-python-root=DIR
  
    ExternalProject_Add( Boost
    DEPENDS Python
    	PREFIX boost
     #--Download step--------------
        URL           ${boost_url}
        URL_HASH SHA1=${boost_url_sha1}
        URL_MD5       ${boost_url_md5}
     #--Configure step-------------
        CONFIGURE_COMMAND  ${Boost_Bootstrap_CMD} --with-python=${PYTHON_EXECUTABLE}
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
#endif()
#else()
#    find_package(Boost ${boost_version} COMPONENTS ${boost_components})

#if( NOT Boost_FOUND)
#        add_custom_target(Rescan ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} DEPENDS Boost)
#else()
#        add_custom_target(Rescan)
#endif()

#add_dependencies(pyGORGON Rescan)
#if(  Boost_FOUND)
#        TARGET_LINK_LIBRARIES(pyGORGON Boost)
#endif()
ExternalProject_Get_Property(Boost INSTALL_DIR)

set(BOOST_ROOT       ${boost_root}       CACHE PATH "Boost root directory"   )
find_package(Boost ${boost_version} COMPONENTS ${boost_components})

#include_directories(${Boost_INCLUDE_DIR})
#
#INCLUDE(FindPackageHandleStandardArgs)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(Boost DEFAULT_MSG Boost_LIBRARIES Boost_LIBRARY_DIR Boost_INCLUDE_DIR)
#MARK_AS_ADVANCED(Boost_LIBRARIES Boost_LIBRARY_DIR Boost_INCLUDE_DIR)
