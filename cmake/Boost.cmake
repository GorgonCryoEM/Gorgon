set(boost_version 1.44)

set(boost_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
set(boost_url_md5        5a5d5614d9a07672e1ab2a250b5defc5                                                )

external_project_vars( Boost
    http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz
    )

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
    
  external_project_build( 
              Boost
        Python
        ";"
        ${boost_url}
#        URL_HASH SHA1=${boost_url_sha1}
#        URL_MD5       ${boost_url_md5}
        "${Boost_Bootstrap_CMD};--with-python=${PYTHON_EXECUTABLE}"
        "${Boost_b2_CMD};${boost_options}"
        "${Boost_b2_CMD};install;--prefix=${boost_install_prefix};${boost_options}"
#     STEP_TARGETS install_name  # Generate custom targets for these steps
    )
#endif()
#else()
#    find_package(Boost ${boost_version} COMPONENTS ${boost_components})

##TODO: Modify to work with multiple libraries
#ExternalProject_Add_Step(Boost install_name
#  COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id ${Boost_LIBRARIES} ${Boost_LIBRARIES}
#  COMMENT "Updating install_name"     # Text printed when step executes
#  DEPENDEES install    # Steps on which this step depends
##  DEPENDERS install     # Steps that depend on this step
#  DEPENDS ${Boost_LIBRARIES} ${CMAKE_CURRENT_LIST_FILE}     # Files on which this step depends
##  [ALWAYS 1]              # No stamp file, step always runs
##  [WORKING_DIRECTORY dir] # Working directory for command
#  LOG 1                 # Wrap step in script to log output
#  )

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
