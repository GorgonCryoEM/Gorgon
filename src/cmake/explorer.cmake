add_custom_target_wrapper(TARGET Explorer
        PROGRAMS ${CMAKE_SOURCE_DIR}/bin/explorer.py
        DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Explorer
        DESTINATIONS ${CMAKE_BINARY_DIR}/bin
        COMPONENT "Explorer"
        DEPENDS Hello Toolkit
        )

add_custom_target(Explorer-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Explorer -P cmake_install.cmake
        )
