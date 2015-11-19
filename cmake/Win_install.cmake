if(WIN32)
    set_target_properties(pyGORGON
                    PROPERTIES
                     PREFIX lib
                     SUFFIX .pyd
                     )
# --------------------------------------------------------------------
    function(install_dll lib)
        set(dll ${lib})
        
#        message(STATUS "dll: ${dll}")
        
        string(REGEX REPLACE "[.]lib"
               ".dll"
               dll
               ${dll}
              )
        
#        message(STATUS "dll: ${dll}")
        
        install(FILES ${dll}
                DESTINATION ${CMAKE_BINARY_DIR}/run
                COMPONENT "Gorgon"
                )
        
    endfunction()
# --------------------------------------------------------------------
    install_dll(${Boost_PYTHON_LIBRARY_RELEASE})
    install_dll(${FFTW3F_LIBRARIES})
endif()
