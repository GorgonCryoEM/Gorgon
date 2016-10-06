add_custom_target_wrapper(TARGET Gorgon
        DEPENDS Toolkit
        )
install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/gorgon/
        DESTINATIONS ${CMAKE_BINARY_DIR}/${lib_install_dir}
        COMPONENT Gorgon
        )
        
add_custom_target(Gorgon-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Gorgon -P cmake_install.cmake
        )
