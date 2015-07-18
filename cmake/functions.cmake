function(loop_list list dest)
    if(ENABLE_CMAKE_DEBUG_OUTPUT)
#        message("dest: ${dest}: ${${dest}}")
#        message("${${list}}")
    endif()
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
#    foreach(var ${libs})
#        message(STATUS "var: ${var}: ${${var}}")
#        set(libs ${${libs}})
#        list(APPEND GORGON_LIBRARIES    ${libs}  )
#    endforeach(var list)
    loop_list(libs GORGON_LIBRARIES)
#    loop_list(${libs})
#    message(STATUS "incs: ${incs}: ${${incs}}")
#    set(incs ${${incs}})
#    list(APPEND GORGON_INCLUDE_DIRS ${incs})
    loop_list(incs GORGON_INCLUDE_DIRS)
 
    set(GORGON_LIBRARIES ${GORGON_LIBRARIES} PARENT_SCOPE)
    set(GORGON_INCLUDE_DIRS ${GORGON_INCLUDE_DIRS} PARENT_SCOPE)
    
endfunction()
# --------------------------------------------------------------------
function(external_project_vars deps url config_cmd build_cmd install_cmd)
    set(file_name ${CMAKE_CURRENT_LIST_FILE})
    get_filename_component(trgt ${file_name} NAME_WE)

    string(TOLOWER ${trgt} proj)
    
#    set(${trgt}_root_type Gorgon CACHE STRING "${proj} root directory type")
#    set_property(CACHE ${trgt}_root_type PROPERTY STRINGS Custom Gorgon)

    set( ${proj}_name           ${trgt}                              CACHE INTERNAL "")
    set( ${trgt}_proj           ${proj}                              CACHE INTERNAL "")
    set(  proj_root             ${proj}_root)
    set( ${proj_root}           ${CMAKE_CURRENT_BINARY_DIR}/${proj}  CACHE INTERNAL "")
    set( ${proj}_url            ${url}                               CACHE INTERNAL "")
#    set( ${proj}_url_sha1                                            CACHE INTERNAL "")
#    set( ${proj}_url_md5                                             CACHE INTERNAL "")    
    set( ${proj}_deps           ${deps}                              CACHE INTERNAL "")
    set( ${proj}_config_cmd     ${config_cmd}                        CACHE INTERNAL "")
    set( ${proj}_build_cmd      ${build_cmd}                         CACHE INTERNAL "")
    set( ${proj}_install_cmd    ${install_cmd}                       CACHE INTERNAL "")
    set( ${proj}_install_prefix ${${proj_root}}                      CACHE INTERNAL "")
    set( ${proj}_extra_options  ""                                   CACHE INTERNAL "")
    
    if(ARGN)
        list(LENGTH ARGN N)
        math(EXPR N ${N}-1)
        
        foreach(i RANGE 0 ${N} 2)
            math(EXPR i1 ${i}+1)
            list(GET ARGN ${i} key)
            list(GET ARGN ${i1} val)
            set( ${proj}_${key} ${val}  CACHE INTERNAL "")
            
            list(APPEND ${proj}_extra_options --${key}=${val})
        endforeach()        
    endif()
    set( ${proj}_extra_options ${${proj}_extra_options}  CACHE INTERNAL "Additional options for ${${proj}_name}")
    
#    set( ${proj}_librarydir     ${${proj_root}}/lib                  CACHE INTERNAL "")
#    set( ${proj}_includedir     ${${proj_root}}/include              CACHE INTERNAL "")

    if(EXTERNAL_LIBS_BUILD)
        external_project_build(${trgt})
    endif()
    
endfunction()
# --------------------------------------------------------------------
# TODO: 
# 1. step targets
# 2. separate downloads-update-verify steps into other external projects
# 3. separate/group configure/build/install steps
# 4. Then, maybe prevent from re-downloading
# 5. Explore ExternalProject_Add_Step and similar new functions
# --------------------------------------------------------------------
function(external_project_build trgt)
    if(ENABLE_CMAKE_DEBUG_OUTPUT)
        message("Dependency - ${proj}: BUILD")
    endif()
    
    include(ExternalProject)
        
    set(proj ${${trgt}_proj})
    
    ExternalProject_Add( ${trgt}
    DEPENDS ${${proj}_deps}
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
     #--Download step--------------
        URL           ${${proj}_url}
#        URL_HASH SHA1=${boost_url_sha1}
        ${${proj}_url_sha1_cmd}
#        URL_MD5       ${boost_url_md5}
        ${${proj}_url_md5_cmd}
     #--Configure step-------------
        CONFIGURE_COMMAND  ${${proj}_config_cmd}
     #--Build step-----------------
        BUILD_COMMAND ${${proj}_build_cmd}
        BUILD_IN_SOURCE 1
     #--Install step---------------
        INSTALL_COMMAND ${${proj}_install_cmd}
        ${${proj}_install_cmd_extra}
     #--Output logging-------------
      LOG_DOWNLOAD  ${LOG_EXTERNAL_LIBRARY_BUILDS}           # Wrap download in script to log output
      LOG_UPDATE    ${LOG_EXTERNAL_LIBRARY_BUILDS}           # Wrap update in script to log output
      LOG_CONFIGURE ${LOG_EXTERNAL_LIBRARY_BUILDS}           # Wrap configure in script to log output
      LOG_BUILD     ${LOG_EXTERNAL_LIBRARY_BUILDS}           # Wrap build in script to log output
      LOG_TEST      ${LOG_EXTERNAL_LIBRARY_BUILDS}           # Wrap test in script to log output
      LOG_INSTALL   ${LOG_EXTERNAL_LIBRARY_BUILDS}           # Wrap install in script to log output
     #--Custom targets-------------
#     STEP_TARGETS install_name  # Generate custom targets for these steps
    )
    
#    if(${proj}_add_step_args)
#        ExternalProject_Add_Step(${trgt} ${${proj}_add_step_args})
#    endif()
endfunction() 
# --------------------------------------------------------------------
