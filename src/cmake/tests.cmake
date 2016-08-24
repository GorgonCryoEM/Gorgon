add_custom_target_and_install_to_destinations(TARGET Tests
                DIRECTORY ${CMAKE_SOURCE_DIR}/tests
                DESTINATIONS ${CMAKE_BINARY_DIR}
                DEPENDS Programs
                COMPONENT Tests
        )

install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/demo/groel
        DESTINATIONS ${CMAKE_BINARY_DIR}/tests
        #DEPENDS ${old_gorgon_lib_target_name}
        COMPONENT Tests
)
