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
    configure_file(${CMAKE_CURRENT_LIST_DIR}/pylib.cpp.in
                    ${CMAKE_BINARY_DIR}/src/py${proj_low}.cpp
                   )

    list(APPEND includes
                ${GORGON_EXTERNAL_LIBRARIES_DIR}
                )
    list(APPEND libs
                ""
                )
                
                   
    add_library(py${proj_low} SHARED ${CMAKE_BINARY_DIR}/src/py${proj_low}.cpp ${srcs})

    list(APPEND pyincludes
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${Boost_INCLUDE_DIR}
                ${PYTHON_INCLUDE_DIR}
                ${includes}
                )
    list(APPEND pylibs
                ${Boost_LIBRARIES}
                ${PYTHON_LIBRARY}
                )
                
    set_target_properties(py${proj_low} PROPERTIES INCLUDE_DIRECTORIES "${pyincludes}")
    target_link_libraries(py${proj_low}                                 ${pylibs}     )
    
    if(ENABLE_CMAKE_DEBUG_OUTPUT)
        message("Debug: py${proj_low}")
    
        get_target_property(includes py${proj_low} INCLUDE_DIRECTORIES)
        get_target_property(libs     py${proj_low} LINK_LIBRARIES)
    
        message("includes")
        foreach(i ${includes})
          message(STATUS "${i}")
        endforeach()
        
        message("libs")
        foreach(i ${libs})
          message(STATUS "${i}")
        endforeach()
        
        message("Debug: py${proj_low} END\n")
    endif()
    
    install_wrapper(TARGETS py${proj_low}
            DESTINATIONS ${target_installation_locations}
            COMPONENT ${${proj_low}_install_component}
            )
            
    if(WIN32)
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
