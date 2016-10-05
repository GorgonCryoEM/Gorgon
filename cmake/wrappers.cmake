function(install_to_destinations)
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
            install_to_destinations(${t} ${t_contents}
                    DESTINATIONS ${p_DESTINATIONS}
                    COMPONENT ${p_COMPONENT}
                    )
        endif()
    endforeach()
endfunction()
# --------------------------------------------------------------------
function(setup_libpy proj)
    set(proj_low ${${proj}_trgt_name})
    set(py_proj py${proj_low})
    
    file(GLOB_RECURSE srcs "*.cpp")
    
    set(current_pylib "lib${py_proj}")    
    configure_file(${CMAKE_CURRENT_LIST_DIR}/pylib.cpp.in
                    ${CMAKE_BINARY_DIR}/src/${py_proj}.cpp
                   )

    add_library(${py_proj} SHARED ${CMAKE_BINARY_DIR}/src/${py_proj}.cpp ${srcs})

    list(APPEND pyincludes
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${Boost_INCLUDE_DIR}
                ${PYTHON_INCLUDE_DIR}
                ${GORGON_EXTERNAL_LIBRARIES_DIR}
                )
    list(APPEND pylibs
                ${Boost_LIBRARIES}
                ${PYTHON_LIBRARY}
                )
                
    if(NOT WIN32)
        set(libsuffix .so)
    else()
        set(libsuffix .pyd)
    endif()

    set_target_properties(${py_proj} PROPERTIES
                                     PREFIX lib
                                     SUFFIX ${libsuffix}
                                     INCLUDE_DIRECTORIES "${pyincludes}"
                                    )
    target_link_libraries(${py_proj} ${pylibs})
    
    if(ENABLE_CMAKE_DEBUG_OUTPUT)
        message("Debug: ${py_proj}")
    
        get_target_property(includes ${py_proj} INCLUDE_DIRECTORIES)
        get_target_property(libs     ${py_proj} LINK_LIBRARIES)
    
        message("includes")
        foreach(i ${includes})
          message(STATUS "${i}")
        endforeach()
        
        message("libs")
        foreach(i ${libs})
          message(STATUS "${i}")
        endforeach()
        
        message("Debug: ${py_proj} END\n")
    endif()
    
    install_to_destinations(TARGETS ${py_proj}
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
function(init)
    set(proj ${CMAKE_CURRENT_SOURCE_DIR})
    string(REGEX REPLACE ".*/" "" proj ${proj})
    to_title_case(${proj} proj)
    set_proj_vars(${proj})
    
    setup_libpy(${proj})
endfunction()
# --------------------------------------------------------------------
