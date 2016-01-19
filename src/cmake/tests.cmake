add_custom_target_wrapper(TARGET Tests
                DIRECTORY ${CMAKE_SOURCE_DIR}/tests
                DESTINATIONS ${CMAKE_BINARY_DIR}
                DEPENDS Hello Core
                COMPONENT "Tests"
        )
