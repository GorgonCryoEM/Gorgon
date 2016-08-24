file(GLOB progs "${CMAKE_SOURCE_DIR}/bin/*.py")

add_custom_target_wrapper(TARGET Programs
        DEPENDS Hello Toolkit #Core
        )
install_to_destinations(
        PROGRAMS ${progs}
        DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Toolkit ${CMAKE_SOURCE_DIR}/bin/Core
        DESTINATIONS ${CMAKE_BINARY_DIR}/bin
        COMPONENT Programs
        )
        
add_custom_target(Programs-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Programs -P cmake_install.cmake
        )
