enable_testing()

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
add_custom_target_wrapper(TARGET Tests
        DEPENDS Hello Cli
        )
install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/tests
        DESTINATIONS ${CMAKE_BINARY_DIR}
        COMPONENT Tests
        )

install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/demo/groel
        DESTINATIONS ${CMAKE_BINARY_DIR}/tests
        COMPONENT Tests
)
