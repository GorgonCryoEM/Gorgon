function(loop_list list dest)
    foreach(var IN LISTS ${list})
        if(ENABLE_CMAKE_DEBUG_OUTPUT)
            message(STATUS "var: ${var}: ${${var}}")
        endif()

        set(var ${${var}})
        list(APPEND ${dest}    ${var}  )
    endforeach()
    
    set(${dest} ${${dest}} PARENT_SCOPE)
endfunction()
# --------------------------------------------------------------------
function(update_libs_includes libs incs)
    loop_list(libs GORGON_LIBRARIES)
    loop_list(incs GORGON_INCLUDE_DIRS)
 
    set(GORGON_LIBRARIES ${GORGON_LIBRARIES} PARENT_SCOPE)
    set(GORGON_INCLUDE_DIRS ${GORGON_INCLUDE_DIRS} PARENT_SCOPE)
endfunction()
# --------------------------------------------------------------------
function(add_dll lib)
    get_filename_component(dll_name ${lib} NAME_WE)
    get_filename_component(dll_dir  ${lib} DIRECTORY)
    
    list(APPEND win_dlls ${dll_dir}/${dll_name}.dll)
    set(win_dlls ${win_dlls} PARENT_SCOPE)
endfunction()
# --------------------------------------------------------------------
