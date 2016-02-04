cmakelist()
            
include_directories(${FFTW3F_INCLUDE_DIRS})
target_link_libraries(${core_trgt_name} ${FFTW3F_LIBRARIES})
