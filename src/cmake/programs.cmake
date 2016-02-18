
file(GLOB progs "${CMAKE_SOURCE_DIR}/bin/*.py")

install(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Toolkit
        DESTINATION ${CMAKE_BINARY_DIR}/bin
        COMPONENT "Programs"
#        DEPENDS Hello Toolkit
        )

add_custom_target_wrapper(TARGET Programs
        PROGRAMS ${progs}
        DESTINATIONS ${CMAKE_BINARY_DIR}/bin
        COMPONENT "Programs"
        DEPENDS Hello Toolkit
        )
        
add_custom_target(Programs-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Programs -P cmake_install.cmake
        )
