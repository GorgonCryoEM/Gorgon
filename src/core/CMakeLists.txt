file(GLOB srcs "*.cpp")

add_library(${core_lib_target_name} SHARED ${srcs})

target_link_libraries(${core_lib_target_name} ${GORGON_LIBRARIES})
