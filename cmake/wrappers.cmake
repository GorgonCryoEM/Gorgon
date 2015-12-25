function(add_module proj)
    set(proj_low ${${proj}_trgt_name})
    
    file(GLOB srcs "*.cpp")
    
    set(current_pylib "libpy${proj_low}")    
    configure_file(${CMAKE_CURRENT_LIST_DIR}/py${proj_low}.cpp.in
                    ${CMAKE_BINARY_DIR}/src/py${proj_low}.cpp
                   )
    
    add_library(${proj_low} SHARED ${srcs})
                   
    add_library(py${proj_low} MODULE ${CMAKE_BINARY_DIR}/src/py${proj_low}.cpp)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    #target_link_libraries(py${proj_low} ${proj_low} ${Boost_LIBRARIES} ${PYTHON_LIBRARY})
    target_link_libraries(py${proj_low} ${proj_low} ${GORGON_LIBRARIES})
    
    install_wrapper(TARGETS ${proj_low} py${proj_low}
            DESTINATIONS ${target_installation_locations}
            COMPONENT ${${proj_low}_install_component}
            )
            
    if(WIN32)
        rename_target_windows(${${proj}_trgt_name})
        install_dlls(${proj_low})
    endif()    
endfunction()
# --------------------------------------------------------------------
