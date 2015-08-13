file(GLOB srcs "*.cpp")

add_library(${core_lib_target_name} OBJECT ${srcs})
