add_custom_target_wrapper(TARGET Tests
        DEPENDS Programs
        )
install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/tests
        DESTINATIONS ${CMAKE_BINARY_DIR}
        COMPONENT Tests
        )

install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/demo/groel
        DESTINATIONS ${CMAKE_BINARY_DIR}/tests
        #DEPENDS ${old_gorgon_lib_target_name}
        COMPONENT Tests
)
