SET(run_dir     ${CMAKE_BINARY_DIR}/run/  CACHE PATH "The directory to place working Gorgon.")
SET(package_dir ${CMAKE_BINARY_DIR}/dist/ CACHE PATH "The directory to place Gorgon package")

configure_file(${CMAKE_SOURCE_DIR}/Gorgon/src_py/gorgon.pyw.in
                ${run_dir}/gorgon.pyw
               )

if(APPLE)
    set(package_setup_file ${CMAKE_SOURCE_DIR}/setup_mac.py.in)
endif()

if(WIN32)
    set(package_setup_file ${CMAKE_SOURCE_DIR}/setup_win.py.in)
endif()

if(APPLE OR WIN32)
    configure_file(${package_setup_file}
                    ${run_dir}/setup.py
                   )
endif()
