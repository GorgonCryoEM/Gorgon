set(target_osx_version "10.10" CACHE STRING "Target OSX version")
set_property(CACHE target_osx_version PROPERTY STRINGS 10.10 10.9 10.8 10.7)

set(CMAKE_OSX_SYSROOT "$ENV{HOME}/Documents/MacOSX_SDKs/MacOSX${target_osx_version}.sdk" CACHE PATH "" FORCE)
set(MACOSX_DEPLOYMENT_TARGET "$ENV{HOME}/Documents/MacOSX_SDKs/MacOSX${target_osx_version}.sdk" CACHE PATH "" FORCE)

set(CMAKE_OSX_DEPLOYMENT_TARGET ${target_osx_version} CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
