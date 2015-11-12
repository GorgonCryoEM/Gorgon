configure_file(${CMAKE_SOURCE_DIR}/Gorgon/src_py/gorgon.pyw.in
                ${CMAKE_BINARY_DIR}/run/gorgon.pyw
               )


if(WIN32)
    set(package_setup_file ${CMAKE_SOURCE_DIR}/setup_win.py.in)
endif()

if(WIN32)
    configure_file(${package_setup_file}
                    ${CMAKE_BINARY_DIR}/run/setup.py
                   )
endif()
