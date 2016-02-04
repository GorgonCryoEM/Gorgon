if(WIN32)
    install_wrapper(FILES ${win_dlls}
            DESTINATIONS ${target_installation_locations}
            COMPONENT Programs
            )
endif()
