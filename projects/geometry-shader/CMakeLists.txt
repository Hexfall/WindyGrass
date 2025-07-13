set(libraries glad glfw assimp imgui itugl ${APPLE_LIBRARIES})

file(GLOB_RECURSE target_inc "*.h" )
file(GLOB_RECURSE target_src "*.cpp" )

file(GLOB_RECURSE shaders "*.vert" "*.frag" "*.geom" "*.glsl")
source_group("Shaders" FILES ${shaders})

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/models")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/models" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/textures")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/textures" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

add_executable(${TARGETNAME} ${target_inc} ${target_src} ${shaders})
target_link_libraries(${TARGETNAME} ${libraries})