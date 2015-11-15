if(WIN32)
    set_target_properties(pyGORGON
                    PROPERTIES
                     PREFIX lib
                     SUFFIX .pyd
                     )
endif()
