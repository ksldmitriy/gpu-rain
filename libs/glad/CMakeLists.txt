cmake_minimum_required(VERSION 3.13.4)

project(glad)

# set
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -m64)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# find packages
find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS GLX OpenGL)

# program
file(GLOB_RECURSE MY_PROJECT_SRC src/*.c)

add_library(glad ${MY_PROJECT_SRC})

target_link_libraries(glad PUBLIC ${X11_LIBRARIES} libXi.so OpenGL::OpenGL OpenGL::GLX)

target_include_directories(glad PUBLIC include)

