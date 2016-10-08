file(GLOB progs "${CMAKE_SOURCE_DIR}/cli/*.py")

add_custom_target_wrapper(TARGET Cli
        DEPENDS Toolkit
        )
install_to_destinations(
        PROGRAMS ${progs}
        DESTINATIONS ${CMAKE_BINARY_DIR}/${prog_build_dir}
        COMPONENT Cli
        )
        
add_custom_target(Cli-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Cli -P cmake_install.cmake
        )
