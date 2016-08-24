file(GLOB progs "${CMAKE_SOURCE_DIR}/bin/*.py")

add_custom_target_and_install_to_destinations(TARGET Programs
        PROGRAMS ${progs}
        DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Toolkit ${CMAKE_SOURCE_DIR}/bin/Core
        DESTINATIONS ${CMAKE_BINARY_DIR}/bin
        COMPONENT Programs
        DEPENDS Hello Toolkit #Core
        )
        
add_custom_target(Programs-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Programs -P cmake_install.cmake
        )
