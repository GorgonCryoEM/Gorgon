add_custom_target_wrapper(TARGET Explorer
        DEPENDS Hello Toolkit Programs
        )
install_to_destinations(
        PROGRAMS ${CMAKE_SOURCE_DIR}/bin/explorer.py
        DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Explorer ${CMAKE_SOURCE_DIR}/bin/ToolkitGui
        DESTINATIONS ${CMAKE_BINARY_DIR}/bin
        COMPONENT Explorer
        )

add_custom_target(Explorer-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Explorer -P cmake_install.cmake
        )
