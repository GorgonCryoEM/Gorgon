install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${run_dir}/setup.py py2app
                                                    --bdist-base=${CMAKE_BINARY_DIR}/build
                                                    --dist-dir=${package_dir}
    WORKING_DIRECTORY ${run_dir})"
    COMPONENT "Package"
    )
    
set(trgt_file $<TARGET_FILE:${legacy_trgt_name}>)

add_custom_command(TARGET Package
    COMMAND ${CMAKE_COMMAND}
                -DTRGT=${trgt_file}
                -DPKGDIR=${package_dir}
                -P ${CMAKE_CURRENT_LIST_DIR}/MoveLib.cmake
    )
