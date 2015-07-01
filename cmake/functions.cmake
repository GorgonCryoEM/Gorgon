include(cmake/Debug.cmake)

function(external_project proj prefix _incl _lib)
    find_package(${proj} REQUIRED)
    message(${prefix})

    string(TOUPPER ${prefix} prefixup)
    string(TOLOWER ${prefix} prefixlo)
    
    message("${prefix}_LIBRARY     = ${${prefix}_LIBRARY}    ")
    message("${prefix}_LIBRARIES   = ${${prefix}_LIBRARIES}  ")
    message("${prefixup}_LIBRARY   = ${${prefixup}_LIBRARY}  ")
    message("${prefixup}_LIBRARIES = ${${prefixup}_LIBRARIES}")
    
    list(APPEND ${_incl} ${${prefix}_INCLUDE_DIR})
    list(REMOVE_DUPLICATES ${_incl})
    set(${_incl} ${${_incl}} PARENT_SCOPE)

    list(APPEND ${_lib} ${${prefix}_LIBRARY})
    list(APPEND ${_lib} ${${prefix}_LIBRARIES})
    list(APPEND ${_lib} ${${prefixup}_LIBRARY})
    list(APPEND ${_lib} ${${prefixup}_LIBRARIES})
#    list(APPEND ${_lib} ${${prefixlo}_LIBRARY})
#    list(APPEND ${_lib} ${${prefixlo}_LIBRARIES})
list(REMOVE_DUPLICATES ${_lib})
    set(${_lib} ${${_lib}} PARENT_SCOPE)
        
#    if(${proj}_FOUND OR ${projup}_FOUND OR ${projlo}_FOUND)
#        message(STATUS "${prefix}_INCLUDE_DIR:= ${${prefix}_INCLUDE_DIR}")
#        message(STATUS "${prefix}_LIBRARY:= ${${prefix}_LIBRARY}")
#        
#        message( "${_incl}:= ${${_incl}}")
#        message( "${_lib}:= ${${_lib}}")
#       
#        print_vars("${prefix}_INCLUDE_DIR")
#        
#    else()
#        message("UH-OH")
#    endif()
    
endfunction()