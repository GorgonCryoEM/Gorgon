install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/run/setup.py py2exe
                                                    --dist-dir=${CMAKE_BINARY_DIR}/package/dist
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/run/)"
    COMPONENT "Package"
    )
