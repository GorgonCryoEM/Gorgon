#TODO: remove if not needed on Linux anymore
if(UNIX)
	set(Boost_NO_BOOST_CMAKE ON)
endif()

set(Boost_USE_MULTITHREADED ON)

if(NOT WIN32)
	find_package(Boost COMPONENTS python REQUIRED)
else()
	set(BOOST_LIBRARYDIR $ENV{LIBRARY_LIB})
	find_package(Boost COMPONENTS python serialization REQUIRED)
endif()

update_libs_includes(Boost_LIBRARIES Boost_INCLUDE_DIR)
if(Boost_FOUND)
	message(STATUS "  ${Boost_LIBRARIES}")
endif()
