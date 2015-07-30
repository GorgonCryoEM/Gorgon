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

# Fails miserably on Linux without this
set(Boost_NO_BOOST_CMAKE ON)

set(Boost_USE_MULTITHREADED ON)
set(BOOST_ROOT ${boost_root})

#set(CMAKE_PREFIX_PATH ${boost_install_prefix})
find_package(Boost ${boost_version} COMPONENTS ${boost_components})

#list(APPEND GORGON_LIBRARIES    ${Boost_LIBRARIES}  )
#list(APPEND GORGON_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
update_libs_includes(Boost_LIBRARIES Boost_INCLUDE_DIR)
