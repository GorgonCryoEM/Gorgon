if(APPLE)
    set(GORGON_TARGET_OSX_VERSION)
    option(GORGON_ENABLE_CROSS_COMPILE_MAC "enable cross-compile for Mac OSX")
    set(GORGON_OSX_SDK_ROOT)
    set(GORGON_OSX_SDK_PREFIX)
    
    mark_as_advanced(GORGON_ENABLE_CROSS_COMPILE_MAC)
    mark_as_advanced(GORGON_TARGET_OSX_VERSION)
    mark_as_advanced(GORGON_OSX_SDK_ROOT)
    mark_as_advanced(GORGON_OSX_SDK_PREFIX)
    
    if(GORGON_ENABLE_CROSS_COMPILE_MAC)
        set(GORGON_TARGET_OSX_VERSION "10.10" CACHE STRING "Target OSX version")
        set_property(CACHE GORGON_TARGET_OSX_VERSION PROPERTY STRINGS 10.10 10.9 10.8 10.7)
        
        set(CMAKE_OSX_SYSROOT "$ENV{HOME}/Documents/MacOSX_SDKs/MacOSX${GORGON_TARGET_OSX_VERSION}.sdk" CACHE PATH "" FORCE)
        set(MACOSX_DEPLOYMENT_TARGET "$ENV{HOME}/Documents/MacOSX_SDKs/MacOSX${GORGON_TARGET_OSX_VERSION}.sdk" CACHE PATH "" FORCE)
        
        set(CMAKE_OSX_DEPLOYMENT_TARGET ${GORGON_TARGET_OSX_VERSION} CACHE STRING "" FORCE)
        set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
    else()
        unset(GORGON_TARGET_OSX_VERSION          CACHE)
        unset(CMAKE_OSX_SYSROOT           CACHE)
        unset(MACOSX_DEPLOYMENT_TARGET    CACHE)
        unset(CMAKE_OSX_DEPLOYMENT_TARGET CACHE)
    endif()
endif()
