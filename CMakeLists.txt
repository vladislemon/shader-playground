cmake_minimum_required(VERSION 3.26)
project(shader_playground C)

set(CMAKE_C_STANDARD 99)

find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(shader-playground WIN32 src/main.c)
target_link_libraries(shader-playground GLEW::GLEW glfw)
add_custom_command(TARGET shader-playground POST_BUILD COMMAND $<$<CONFIG:release>:${CMAKE_STRIP}> ARGS $<TARGET_FILE:shader-playground>)
