cmake_minimum_required(VERSION 3.28)
project(Dorimeji)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_CXX_FLAGS "-static -Wall -Wextra -Wundef -Wdouble-promotion -Wformat=2 -fno-common -Wconversion -pedantic -Wno-unused-parameter") # Bundle the necessary static libraries, as well as enable some useful warnings, and disable some nasty ones
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -Og -DDEBUG") # Enable debug symbols and optimisations for debug builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3") # Enable full optimisations for release builds

add_subdirectory(libs/glfw)

include_directories(libs libs/glad/include)

add_executable(Dorimeji src/main.cpp libs/glad/src/glad.c libs/stb_image.h
        src/shader/shader.hpp
        src/creature/creature.cpp
        src/creature/creature.hpp)
target_link_libraries(Dorimeji glfw)
