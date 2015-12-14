function(install_wrapper)
    set(options)
    set(oneValueArgs COMPONENT)
    set(multiValueArgs TARGETS FILES PROGRAMS DIRECTORY DESTINATIONS)
    cmake_parse_arguments(p "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(p_TARGETS)
      set(type TARGETS)
    endif()
    
    if(p_FILES)
      set(type FILES)
    endif()
    
    if(p_PROGRAMS)
      set(type PROGRAMS)
    endif()

    if(p_DIRECTORY)
      set(type DIRECTORY)
    endif()

    set(type_contents p_${type})
    
    foreach(d ${p_DESTINATIONS})
      foreach(t ${${type_contents}})
          install(${type} ${t}
                  DESTINATION ${d}
                  COMPONENT ${p_COMPONENT}
                  )
      endforeach()
    endforeach()
endfunction()
# --------------------------------------------------------------------
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
function(add_subdirectory_wrapper proj)
    string(TOLOWER ${proj} proj_low)
    
    add_subdirectory(${proj_low})
    
    add_custom_target(${proj}
            COMMAND ${CMAKE_COMMAND} -DCOMPONENT=${proj} -P cmake_install.cmake
            DEPENDS py${proj_low}
            )
endfunction()
# --------------------------------------------------------------------
