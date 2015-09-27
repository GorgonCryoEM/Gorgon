file(GLOB srcs "*.cpp")

include_directories(${GORGON_INCLUDE_DIRS})

add_library(${core_lib_target_name} SHARED ${srcs})

target_link_libraries(${core_lib_target_name} ${GORGON_LIBRARIES})
