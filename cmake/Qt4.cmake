#external_project_vars(
#        ""
#        http://download.qt.io/archive/qt/4.8/4.8.6/qt-everywhere-opensource-src-4.8.6.tar.gz
#        ""
#        "./configure;-confirm-license;-prefix ${CMAKE_BINARY_DIR}/qt4;-opensource;-release;-no-phonon;-no-multimedia;-no-phonon-backend;-no-webkit;-no-javascript-jit"
#        "${CMAKE_MAKE_PROGRAM};-j${NUMBER_OF_PARALLEL_JOBS}"
#        "${CMAKE_MAKE_PROGRAM};install"
#    )

#set(qt4_url_sha1_cmd       URL_HASH SHA1=ddf9c20ca8309a116e0466c42984238009525da6  CACHE INTERNAL "")
#set(qt4_url_md5_cmd        URL_MD5       2edbe4d6c2eff33ef91732602f3518eb          CACHE INTERNAL "")

find_package(Qt4)
