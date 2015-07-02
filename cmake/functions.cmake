include(cmake/Debug.cmake)

function(external_project proj)

    set(${proj}_root_type "Gorgon" CACHE STRING "${proj} root directory type")
    set_property(CACHE ${proj}_root_type PROPERTY STRINGS Custom Gorgon)
    if(${proj}_root_type STREQUAL Gorgon)
        string(TOLOWER ${proj}_root root_low)
        set(${proj}_root ${CMAKE_BINARY_DIR}/${root_low})
        
        include(${CMAKE_SOURCE_DIR}/cmake/${proj}.cmake)
    endif()

    set(CMAKE_PREFIX_PATH ${root_low})
    find_package(${${proj}_find} ${_pkg_arg})

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