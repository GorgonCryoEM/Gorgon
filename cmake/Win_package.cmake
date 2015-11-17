# For list of options:
# http://www.py2exe.org/index.cgi/ListOfOptions

install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${run_dir}/setup.py py2exe
                                                    --dist-dir=${CMAKE_BINARY_DIR}/dist
    WORKING_DIRECTORY ${run_dir})"
    COMPONENT "Package"
    )
