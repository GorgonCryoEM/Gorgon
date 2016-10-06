add_custom_target_wrapper(TARGET Explorer
        DEPENDS Programs
        )
install_to_destinations(
        PROGRAMS ${CMAKE_SOURCE_DIR}/cli/explorer.py
        DIRECTORY ${CMAKE_SOURCE_DIR}/cli/Explorer ${CMAKE_SOURCE_DIR}/cli/ToolkitGui
        DESTINATIONS ${CMAKE_BINARY_DIR}/cli
        COMPONENT Explorer
        )

add_custom_target(Explorer-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Explorer -P cmake_install.cmake
        )
