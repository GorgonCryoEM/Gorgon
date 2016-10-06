file(GLOB progs "${CMAKE_SOURCE_DIR}/cli/*.py")
#set(prog_build_dir ${build_install_dir})
set(prog_build_dir cli)

add_custom_target_wrapper(TARGET Programs
        DEPENDS Toolkit
        )
install_to_destinations(
        PROGRAMS ${progs}
        DESTINATIONS ${CMAKE_BINARY_DIR}/${prog_build_dir}
        COMPONENT Programs
        )
        
add_custom_target(Programs-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Programs -P cmake_install.cmake
        )
