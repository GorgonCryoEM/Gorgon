add_custom_target_wrapper(TARGET Demo
                DIRECTORY ${CMAKE_SOURCE_DIR}/demo
                DESTINATIONS ${CMAKE_BINARY_DIR}
                #DEPENDS ${old_gorgon_lib_target_name}
                COMPONENT "Demo"
        )
