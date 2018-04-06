cmake_minimum_required(VERSION 3.9)
project(learn)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES glad.c main.cpp shader.cpp stb_image.cpp)

add_executable(learn ${SOURCE_FILES})
target_link_libraries(learn GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)