find_package(OpenGL REQUIRED)

list(APPEND GORGON_LIBRARIES   
            ${OPENGL_gl_LIBRARY}
            ${OPENGL_glu_LIBRARY}
        	)
list(APPEND GORGON_INCLUDE_DIRS
        	${GLU_INCLUDE_PATH}
        	${GL_INCLUDE_PATH}
        	${OPENGL_INCLUDE_DIR}
            )
