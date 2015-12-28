if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
    
    add_dll(${Boost_PYTHON_LIBRARY_RELEASE})
    add_dll(${FFTW3F_LIBRARIES})
    
    if(MSVC AND GORGON_TARGET_ARCH EQUAL 64)
    	ADD_DEFINITIONS(/bigobj)
    endif()
endif()
