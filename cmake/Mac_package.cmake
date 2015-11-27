install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${run_dir}/setup.py py2app
                                                    --bdist-base=${CMAKE_BINARY_DIR}/build
                                                    --dist-dir=${package_dir}
    WORKING_DIRECTORY ${run_dir})"
    COMPONENT "Package"
    )
    
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E rename
                                        ${package_dir}/Gorgon.app/Contents/Frameworks/libpyGORGON.so
                                        ${package_dir}/Gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/libpyGORGON.so
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
    COMPONENT "Package"
    )
