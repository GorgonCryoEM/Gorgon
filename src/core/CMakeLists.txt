file(GLOB_RECURSE srcs "*.cpp")

add_library(${core_lib_target_name} SHARED ${srcs})

target_link_libraries(${core_lib_target_name} ${GORGON_LIBRARIES})

foreach(d ${target_installation_locations})
    install(TARGETS ${core_lib_target_name}
            DESTINATION ${CMAKE_BINARY_DIR}/${d}
            COMPONENT "Core"
            ) 
endforeach()
