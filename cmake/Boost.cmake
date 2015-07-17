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
elseif(WIN32 )
    set( Boost_Bootstrap_CMD bootstrap.bat )
    set( Boost_b2_CMD b2.exe )
endif()

external_project_vars(
        Python
        http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz
        "${Boost_Bootstrap_CMD};--with-python=${PYTHON_EXECUTABLE}"
        "${Boost_b2_CMD};-j${NUMBER_OF_PARALLEL_JOBS};${boost_options}"
        "${Boost_b2_CMD};install;-j${NUMBER_OF_PARALLEL_JOBS};--prefix=${boost_install_prefix};${boost_options}"
    )

set(Boost_USE_MULTITHREADED ON)

set(BOOST_ROOT ${boost_root})
find_package(Boost ${boost_version} COMPONENTS ${boost_components})

#list(APPEND GORGON_LIBRARIES    ${Boost_LIBRARIES}  )
#list(APPEND GORGON_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
update_libs_includes(Boost_LIBRARIES Boost_INCLUDE_DIR)

#TODO: Modify to work with multiple libraries
set(boost_install_cmd_extra 
    COMMAND    ${CMAKE_INSTALL_NAME_TOOL} -id ${Boost_PYTHON_LIBRARY_RELEASE} ${Boost_PYTHON_LIBRARY_RELEASE}
    CACHE INTERNAL ""
    )

set(boost_url_sha1_cmd       URL_HASH SHA1=a27b010b9d5de0c07df9dddc9c336767725b1e6b  CACHE INTERNAL "")
set(boost_url_md5_cmd        URL_MD5       5a5d5614d9a07672e1ab2a250b5defc5          CACHE INTERNAL "")
