get_filename_component(trgt_file ${TRGT} NAME)

execute_process(COMMAND ${CMAKE_COMMAND} -E rename
                            ${PKGDIR}/Gorgon.app/Contents/Frameworks/${trgt_file}
                            ${PKGDIR}/Gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/${trgt_file}
                )
