include(cmake/Debug.cmake)

function(external_project proj)

    set(CMAKE_PREFIX_PATH ${_root})
    find_package(${proj} ${_pkg_arg})

#    string(TOUPPER ${_prefix} prefixup)
#    string(TOLOWER ${_prefix} prefixlo)
    
    list(APPEND gor_inc ${${ARGV1}_INCLUDE_DIR})
    list(APPEND gor_inc ${${ARGV1}_INCLUDE_PATH})
    list(APPEND gor_inc ${${ARGV1}_INCLUDE_DIRS})
    list(APPEND gor_inc ${${ARGV1}_INCLUDE_PATHS})
    include_directories(${gor_inc})

    list(APPEND gor_lib ${${ARGV1}_LIBRARY})
    list(APPEND gor_lib ${${ARGV1}_LIBRARIES})
    set_property(TARGET pyGORGON APPEND PROPERTY LINK_LIBRARIES  ${gor_lib})
    
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