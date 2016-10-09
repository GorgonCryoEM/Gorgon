init()

install(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/hello.py
        DESTINATION ${CMAKE_BINARY_DIR}/bin
        COMPONENT Hello
        )
