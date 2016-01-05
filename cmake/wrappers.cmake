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
function(add_custom_target_wrapper)
    set(options)
    set(oneValueArgs TARGET COMPONENT)
    set(multiValueArgs TARGETS FILES PROGRAMS DIRECTORY DESTINATIONS DEPENDS)
    cmake_parse_arguments(p "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    add_custom_target(${p_TARGET}
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=${p_TARGET} -P cmake_install.cmake
        DEPENDS ${p_DEPENDS}
        )
        
    set(types TARGETS FILES PROGRAMS DIRECTORY)
    
    foreach(t ${types})
        set(t_contents ${p_${t}})
        if(t_contents)
            install_wrapper(${t} ${t_contents}
                    DESTINATIONS ${p_DESTINATIONS}
                    COMPONENT ${p_COMPONENT}
                    )
        endif()
    endforeach()
endfunction()
# --------------------------------------------------------------------
function(add_module proj)
    set(proj_low ${${proj}_trgt_name})
    
    file(GLOB_RECURSE srcs "*.cpp")
    
    set(current_pylib "libpy${proj_low}")    
    configure_file(${CMAKE_CURRENT_LIST_DIR}/py${proj_low}.cpp.in
                    ${CMAKE_BINARY_DIR}/src/py${proj_low}.cpp
                   )
    
    add_library(${proj_low} SHARED ${srcs})

    list(APPEND includes
                ${GORGON_EXTERNAL_LIBRARIES_DIR}
                )
    list(APPEND libs
                ""
                )
                
    set_target_properties(${proj_low} PROPERTIES INCLUDE_DIRECTORIES ${includes})
    target_link_libraries(${proj_low}                                ${libs}    )
                   
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
function(cmakelist)
    set(proj ${CMAKE_CURRENT_SOURCE_DIR})
    string(REGEX REPLACE ".*/" "" proj ${proj})
    to_title_case(${proj} proj)
    set_proj_vars(${proj})
    
    add_module(${proj})
endfunction()
# --------------------------------------------------------------------
