cmakelist()
            
include_directories(${GORGON_INCLUDE_DIRS})
target_link_libraries(${toolkit_trgt_name} ${GORGON_LIBRARIES})
