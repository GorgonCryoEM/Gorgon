add_custom_target_wrapper(TARGET Gorgon
        DEPENDS Toolkit
        )
install_to_destinations(
        DIRECTORY ${CMAKE_SOURCE_DIR}/gorgon/
        DESTINATIONS ${CMAKE_BINARY_DIR}/bin
        COMPONENT Gorgon
        )
        
add_custom_target(Gorgon-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Gorgon -P cmake_install.cmake
        )
