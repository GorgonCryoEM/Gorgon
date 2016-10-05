file(GLOB progs "${CMAKE_SOURCE_DIR}/bin/*.py")

add_custom_target_wrapper(TARGET Programs
        DEPENDS Toolkit
        )
install_to_destinations(
        PROGRAMS ${progs}
        DESTINATIONS ${CMAKE_BINARY_DIR}/${build_install_dir}
        COMPONENT Programs
        )
        
add_custom_target(Programs-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Programs -P cmake_install.cmake
        )
