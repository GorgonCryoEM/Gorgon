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

if( UNIX )
  set( Boost_Bootstrap_CMD ./bootstrap.sh )
  set( Boost_b2_CMD ./b2 )
else()
  if( WIN32 )
    set( Boost_Bootstrap_CMD bootstrap.bat )
    set( Boost_b2_CMD b2.exe )
  endif()
endif()

external_project_vars( Boost
        Python
        http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz
        "${Boost_Bootstrap_CMD};--with-python=${PYTHON_EXECUTABLE}"
        "${Boost_b2_CMD};${boost_options}"
        "${Boost_b2_CMD};install;--prefix=${boost_install_prefix};${boost_options}"
    )

set(Boost_USE_MULTITHREADED ON)

#set(CMAKE_PREFIX_PATH ${boost_install_prefix})
#find_package(Boost ${boost_version} COMPONENTS ${boost_components})

set( _pkg_arg 1.58 COMPONENTS python)
#set(Boost_find Boost)
external_project_find_paths(Boost)
unset( _pkg_arg )

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


set(boost_url_sha1       a27b010b9d5de0c07df9dddc9c336767725b1e6b                                        )
set(boost_url_md5        5a5d5614d9a07672e1ab2a250b5defc5                                                )
