cmake_minimum_required(VERSION 3.26)
project(shader_playground C)

set(CMAKE_C_STANDARD 99)

find_package(glfw3 3.3 REQUIRED)

include_directories(include)

add_executable(shader-playground src/main.c src/gl.c)
target_link_libraries(shader-playground glfw)
if (UNIX)
    target_link_libraries(shader-playground GL)
endif (UNIX)
if (WIN32)
    set_target_properties(shader-playground PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
endif (WIN32)
add_custom_command(TARGET shader-playground POST_BUILD COMMAND $<$<CONFIG:release>:${CMAKE_STRIP}> ARGS $<TARGET_FILE:shader-playground>)
