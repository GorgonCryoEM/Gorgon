macro(print_vars args)
    foreach(arg IN LISTS ${ARGV})
        message("${arg}=   ${${arg}}")
    endforeach()
endmacro()



function (getListOfVarsStartingWith _prefix _varResult)
    get_cmake_property(_vars VARIABLES)
    string (REGEX MATCHALL "(^|;)${_prefix}[A-Za-z0-9_]*" _matchedVars "${_vars}")
    set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()

function (getListOfVarsWith _prefix _varResult)
    get_cmake_property(_vars VARIABLES)
    string (REGEX MATCHALL "[A-Za-z0-9_]*${_prefix}[A-Za-z0-9_]*" _matchedVars "${_vars}")
    set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()

function (getListOfVarsWith2 _w1 _w2 _varResult)
    get_cmake_property(_vars VARIABLES)
    string (REGEX MATCHALL "${_w1}[A-Za-z0-9_]*${_w2}[A-Za-z0-9_]*" _matchedVars "${_vars}")
    set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()

if(ENABLE_CMAKE_DEBUG_OUTPUT)
    set(SEARCH_STRING "@" CACHE STRING "String to search for in all variables")
    mark_as_advanced(CLEAR SEARCH_STRING)
    
    getListOfVarsWith(${SEARCH_STRING} ddd)
    message("\nBEGIN print_vars()")
    message(  "------------------")
    print_vars(ddd)
    message("----------------")
    message("END print_vars()\n")
else()
    unset(SEARCH_STRING CACHE)
endif()
