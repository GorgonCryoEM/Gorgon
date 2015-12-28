if(WIN32)
    rename_target_windows(${legacy_trgt_name})
    
    install_wrapper(FILES ${win_dlls}
            DESTINATIONS ${run_dir}
            COMPONENT Gorgon
            )
endif()
