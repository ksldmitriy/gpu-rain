cmake_minimum_required(VERSION 3.13.4)

project(gpu-rain)

# set
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-m64")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# find packages
find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS GLX OpenGL)
find_package(glm REQUIRED)
find_package(OpenEXR REQUIRED)

# libs
add_subdirectory(libs/glad)

# program
set(ASM_SOURCE resources/shader-resources.s)
enable_language(CXX C ASM)
set_property(SOURCE ${ASM_SOURCE} PROPERTY LANGUAGE ASM)
set_property(SOURCE ${ASM_SOURCE} APPEND PROPERTY COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/shaders")

file(GLOB_RECURSE MY_PROJECT_SRC src/*.cpp)

add_executable(gpu-rain ${MY_PROJECT_SRC} ${ASM_SOURCE})

target_link_libraries(gpu-rain PUBLIC ${X11_LIBRARIES} X11::Xrandr glad glm::glm OpenEXR::OpenEXR)
target_include_directories(gpu-rain PUBLIC "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_SOURCE_DIR}/libs/stb/include")
