include(cmake/Debug.cmake)

function(external_project proj)

    set(${proj}_root_type "Gorgon" CACHE STRING "${proj} root directory type")
    set_property(CACHE ${proj}_root_type PROPERTY STRINGS Custom Gorgon)
    if(${proj}_root_type STREQUAL Gorgon)
        string(TOLOWER ${proj}_root root_low)
        set(${proj}_root ${CMAKE_BINARY_DIR}/${root_low})
        
#        include(${CMAKE_SOURCE_DIR}/cmake/${proj}.cmake)
    endif()

    if(${${proj}_find})
        set(CMAKE_PREFIX_PATH ${root_low})
        find_package(${${proj}_find} ${_pkg_arg})
    endif()

    string(TOUPPER ${proj} projup)
    string(TOLOWER ${proj} projlo)
    
    list(APPEND gor_inc ${${proj}_INCLUDE_DIR})
    list(APPEND gor_inc ${${proj}_INCLUDE_PATH})
    list(APPEND gor_inc ${${proj}_INCLUDE_DIRS})
    list(APPEND gor_inc ${${proj}_INCLUDE_PATHS})
    include_directories(${gor_inc})
#    set(GORGON_INCLUDE_DIRS ${GORGON_INCLUDE_DIRS} ${gor_inc} PARENT_SCOPE)

#    list(APPEND gor_lib ${${proj}_LIBRARY})
#    list(APPEND gor_lib ${${proj}_LIBRARIES})
    getListOfVarsWith2(${proj} LIBRARY gor_lib)
    message("${proj}")
    message("${gor_lib}")
    list(APPEND ggg ${gor_lib})
    
    getListOfVarsWith2(${proj} LIBRARIES gor_lib)
    message("${proj}")
    message("${gor_lib}")
    list(APPEND ggg ${gor_lib})
    
    getListOfVarsWith2(${projup} LIBRARY gor_lib)
    message("${projup}")
    message("${gor_lib}")
    list(APPEND ggg ${gor_lib})
    
    getListOfVarsWith2(${projup} LIBRARIES gor_lib)
    message("${projup}")
    message("${gor_lib}")
    list(APPEND ggg ${gor_lib})
    
#    getListOfVarsWith2(${projlo} LIBRAR gor_lib)
#    message("${projlo}")
#    message("${gor_lib}")
#    list(APPEND ggg ${gor_lib})    
    
#    set_property(TARGET pyGORGON APPEND PROPERTY LINK_LIBRARIES  ${gor_lib})
    set(GORGON_LIBRARIES ${GORGON_LIBRARIES} ${ggg} PARENT_SCOPE)
    
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

function(external_project_build trgt deps sep url config build instll)
      include(ExternalProject)
      
      string(TOLOWER ${trgt} proj)
if(EXTERNAL_LIBS_BUILD)  
        ExternalProject_Add( ${trgt}
        DEPENDS ${deps}
        	PREFIX ${proj}
    #    	TMP_DIR = <prefix>/tmp
    #        STAMP_DIR = <prefix>/src/<name>-stamp
    #        DOWNLOAD_DIR = <prefix>/src
        	DOWNLOAD_DIR ${GORGON_EXTERNAL_LIBRARIES_DOWNLOAD_DIR}/${proj}
    #        SOURCE_DIR = <prefix> /src/<name>
        	SOURCE_DIR   ${GORGON_EXTERNAL_LIBRARIES_DOWNLOAD_DIR}/${proj}/${trgt}
    #        BINARY_DIR = <prefix>/src/<name>-build
    #        BINARY_DIR = ${GORGON_EXTERNAL_LIBRARIES_DOWNLOAD_DIR}/${proj}/${trgt}
    #        INSTALL_DIR = <prefix>
        	LIST_SEPARATOR ${sep}
         #--Download step--------------
            URL           ${url}
    #        URL_HASH SHA1=${boost_url_sha1}
    #        URL_MD5       ${boost_url_md5}
         #--Configure step-------------
            CONFIGURE_COMMAND  ${config}
         #--Build step-----------------
            BUILD_COMMAND ${build}
            BUILD_IN_SOURCE 1
         #--Install step---------------
            INSTALL_COMMAND ${instll}
         #--Output logging-------------
          LOG_DOWNLOAD  ${LOG_EXTERNAL_LIBRARY_BUILDS}            # Wrap download in script to log output
          LOG_UPDATE    ${LOG_EXTERNAL_LIBRARY_BUILDS}              # Wrap update in script to log output
          LOG_CONFIGURE ${LOG_EXTERNAL_LIBRARY_BUILDS}           # Wrap configure in script to log output
          LOG_BUILD     ${LOG_EXTERNAL_LIBRARY_BUILDS}               # Wrap build in script to log output
          LOG_TEST      ${LOG_EXTERNAL_LIBRARY_BUILDS}                # Wrap test in script to log output
          LOG_INSTALL   ${LOG_EXTERNAL_LIBRARY_BUILDS}             # Wrap install in script to log output
         #--Custom targets-------------
    #     STEP_TARGETS install_name  # Generate custom targets for these steps
        )
endif()

endfunction() 