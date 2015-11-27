#TODO: remove if not needed on Linux anymore
if(UNIX)
	set(Boost_NO_BOOST_CMAKE ON)
endif()

set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS python)
update_libs_includes(Boost_LIBRARIES Boost_INCLUDE_DIR)
