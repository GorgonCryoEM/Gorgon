function(loop_list list dest)
    foreach(var IN LISTS ${list})
        if(ENABLE_CMAKE_DEBUG_OUTPUT)
            message(STATUS "var: ${var}: ${${var}}")
        endif()

        set(var ${${var}})
        list(APPEND ${dest} ${var})
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
function(to_title_case in out)
    string(LENGTH ${in} len)
    string(SUBSTRING ${in} 0 1 first_letter)
    math(EXPR len ${len}-1)
    string(SUBSTRING ${in} 1 ${len} remaining_word)
    string(TOUPPER ${first_letter} first_letter)
    string(CONCAT word ${first_letter} ${remaining_word})
    set(${out} ${word} PARENT_SCOPE)
endfunction()
# --------------------------------------------------------------------
function(set_proj_vars Proj)
    string(TOLOWER ${Proj} proj)
    
    set( ${proj}_trgt_name         ${proj} CACHE INTERNAL "")
    set( ${Proj}_trgt_name         ${proj} CACHE INTERNAL "")
    set( ${proj}_install_component ${Proj} CACHE INTERNAL "")
    set( ${Proj}_install_component ${Proj} CACHE INTERNAL "")
endfunction()
# --------------------------------------------------------------------
