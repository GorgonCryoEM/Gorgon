# --------------------------------------------------------------------
#http://www.cmake.org/Wiki/CMake_RPATH_handling
# use, i.e. don't skip the full RPATH for the build tree
#SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
#
# when building, don't use the install RPATH already
# (but later on when installing)
#SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#
# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH ${CMAKE_BINARY_DIR}/bin/)
#
# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# --------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/cmake/wrappers.cmake)

add_subdirectory(hello)
# --------------------------------------------------------------------
# Install Step
# --------------------------------------------------------------------
include(cmake/programs.cmake)
include(cmake/demo.cmake)
include(cmake/tests.cmake)
include(cmake/explorer.cmake)
# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
add_custom_target(test-verbose
        COMMAND ${CMAKE_CTEST_COMMAND} -V -C Release
        DEPENDS Tests
        )
        
add_test(NAME test-all
        COMMAND nosetests --with-doctest --doctest-tests -vv
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
# --------------------------------------------------------------------
add_custom_target(test-verbose-quick
        COMMAND nosetests --with-doctest --doctest-tests -s -vv -a \!long
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS Tests
        )
# --------------------------------------------------------------------
