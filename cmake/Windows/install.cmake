if(WIN32)
    rename_target_windows(${legacy_trgt_name})
    
    install_to_destinations(FILES ${win_dlls}
            DESTINATIONS ${run_dir}
            COMPONENT Gorgon
            )
endif()
