enable_testing()

add_custom_target(test-verbose
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/gorgon ${CMAKE_BINARY_DIR}/cli/gorgon
        COMMAND ${CMAKE_CTEST_COMMAND} -V -C Release
        DEPENDS Tests
        )

add_test(NAME test-all
        COMMAND nosetests --with-doctest --doctest-tests -vv
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
# --------------------------------------------------------------------
add_custom_target(test-verbose-quick
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/gorgon ${CMAKE_BINARY_DIR}/cli/gorgon
        COMMAND nosetests --with-doctest --doctest-tests -s -vv -a \!long
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS Tests
        )
# --------------------------------------------------------------------
add_custom_target_wrapper(TARGET Tests
        DEPENDS Hello Cli Demo
        )
install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/tests
        DESTINATIONS ${CMAKE_BINARY_DIR}
        COMPONENT Tests
        )
