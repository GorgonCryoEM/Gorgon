add_custom_target_wrapper(TARGET Demo)
install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/demo
        DESTINATIONS ${CMAKE_BINARY_DIR}
        COMPONENT Demo
        )
