SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})

FOREACH(subdir ${SUBDIRS})
    set(TARGETNAME ${subdir})
    add_subdirectory(${subdir})
    if (TARGET ${TARGETNAME})
        set_target_properties(
                ${TARGETNAME} PROPERTIES
                # on Visual Studio, sets the debugger working directory to the source directory,
                # to avoid manually rcopying resource files to the build directory
                # BROKEN ON VS22!
                VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${subdir}
        )
    endif()
ENDFOREACH()