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

#TODO: remove if not needed on Linux anymore
#set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS ${boost_components})

update_libs_includes(Boost_LIBRARIES Boost_INCLUDE_DIR)
