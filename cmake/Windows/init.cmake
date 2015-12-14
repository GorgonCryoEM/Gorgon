if(WIN32)
    add_dll(${Boost_PYTHON_LIBRARY_RELEASE})
    add_dll(${FFTW3F_LIBRARIES})
    
    if(MSVC AND GORGON_TARGET_ARCH EQUAL 64)
    	ADD_DEFINITIONS(/bigobj)
    endif()
endif()
