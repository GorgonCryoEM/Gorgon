
file(GLOB progs "${CMAKE_SOURCE_DIR}/bin/*.py")

add_custom_target_wrapper(TARGET Toolkit
        PROGRAMS ${progs}
        DESTINATIONS ${CMAKE_BINARY_DIR}/bin
        COMPONENT "Toolkit"
        DEPENDS Hello Core
        )
        
add_custom_target(Toolkit-only
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Toolkit -P cmake_install.cmake
        )
