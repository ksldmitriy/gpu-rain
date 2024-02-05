cmake_minimum_required(VERSION 3.13.4)

project(lofi-rain)

# set
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -m64)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# find packages
find_package(X11)

# program
file(GLOB_RECURSE MY_PROJECT_SRC src/*.cpp)

add_executable(lofi-rain ${MY_PROJECT_SRC})

target_link_libraries(lofi-rain PUBLIC ${X11_LIBRARIES} libXi.so)
