function(install_to_destinations)
    set(types TARGETS FILES PROGRAMS DIRECTORY)
    set(options)
    set(oneValueArgs COMPONENT)
    set(multiValueArgs ${types} DESTINATIONS)
    cmake_parse_arguments(p "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    foreach(d ${p_DESTINATIONS})
        foreach(t ${types})
            set(contents ${p_${t}})
            if(contents)
                install(${t} ${contents}
                      DESTINATION ${d}
                      COMPONENT ${p_COMPONENT}
                      )
            endif()
        endforeach()
    endforeach()
endfunction()
# --------------------------------------------------------------------
function(add_custom_target_wrapper)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(p "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    add_custom_target(${p_TARGET}
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=${p_TARGET} -P cmake_install.cmake
        DEPENDS ${p_DEPENDS}
        )
endfunction()
# --------------------------------------------------------------------
function(setup_libpy Proj)
    set(proj ${${Proj}_trgt_name})
    set(pyproj py${proj})
    
    file(GLOB_RECURSE srcs "*.cpp")
    
    set(current_pylib "lib${pyproj}")    
    configure_file(${CMAKE_CURRENT_LIST_DIR}/pylib.cpp.in
                    ${CMAKE_BINARY_DIR}/src/${pyproj}.cpp
                   )

    add_library(${pyproj} SHARED ${CMAKE_BINARY_DIR}/src/${pyproj}.cpp ${srcs})

    list(APPEND pyincludes
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${GORGON_INCLUDE_DIRS}
                )
    list(APPEND pylibs
                ${GORGON_LIBRARIES}
                )
                
    if(NOT WIN32)
        set(libsuffix .so)
    else()
        set(libsuffix .pyd)
    endif()

    set_target_properties(${pyproj} PROPERTIES
                                     PREFIX lib
                                     SUFFIX ${libsuffix}
                                     INCLUDE_DIRECTORIES "${pyincludes}"
                                    )
    target_link_libraries(${pyproj} ${pylibs})
    
    if(ENABLE_CMAKE_DEBUG_OUTPUT)
        message("Debug: ${pyproj}")
        message("Current list file: ${CMAKE_CURRENT_LIST_FILE}")
    
        get_target_property(includes ${pyproj} INCLUDE_DIRECTORIES)
        get_target_property(libs     ${pyproj} LINK_LIBRARIES)
    
        message("includes")
        foreach(i ${includes})
          message(STATUS "${i}")
        endforeach()
        
        message("libs")
        foreach(i ${libs})
          message(STATUS "${i}")
        endforeach()
        
        message("Debug: ${pyproj} END\n")
    endif()
    
    install_to_destinations(TARGETS ${pyproj}
            DESTINATIONS ${CMAKE_BINARY_DIR}/${lib_install_dir}/${proj}
            COMPONENT ${${proj}_install_component}
            )
            
    if(WIN32)
        install_dlls(${proj})
    endif()    
endfunction()
# --------------------------------------------------------------------
function(add_custom_targets_trgt_and_trgt_only Proj)
    add_custom_target(${Proj}
            COMMAND ${CMAKE_COMMAND} -DCOMPONENT=${Proj} -P cmake_install.cmake
            DEPENDS py${${Proj}_trgt_name}
            )
    add_custom_target(${Proj}-only
            COMMAND ${CMAKE_COMMAND} -DCOMPONENT=${Proj} -P cmake_install.cmake
            )
endfunction()
# --------------------------------------------------------------------
function(init)
    set(proj ${CMAKE_CURRENT_SOURCE_DIR})
    string(REGEX REPLACE ".*/" "" proj ${proj})
    to_title_case(${proj} Proj)
    set_proj_vars(${Proj})
    set(current_trgt_name ${Proj} PARENT_SCOPE)
    
    setup_libpy(${Proj})
    add_custom_targets_trgt_and_trgt_only(${Proj})
endfunction()
# --------------------------------------------------------------------
