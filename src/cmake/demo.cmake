add_custom_target_and_install_to_destinations(TARGET Demo
                DIRECTORY ${CMAKE_SOURCE_DIR}/demo
                DESTINATIONS ${CMAKE_BINARY_DIR}
                #DEPENDS ${old_gorgon_lib_target_name}
                COMPONENT "Demo"
        )
