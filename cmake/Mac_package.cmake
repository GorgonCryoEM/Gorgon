install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/run/setup.py py2app
                                                    --bdist-base=${CMAKE_BINARY_DIR}/package/build
                                                    --dist-dir=${CMAKE_BINARY_DIR}/package/dist
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/run/)"
    COMPONENT "Package"
    )
    
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E rename
                                        package/dist/Gorgon.app/Contents/Frameworks/libpyGORGON.so
                                        package/dist/Gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/libpyGORGON.so
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})"
    COMPONENT "Package"
    )
