cmake_minimum_required(VERSION 3.16)
project(scivibe_engine LANGUAGES C CXX)

# Keep dependencies local and reproducible through the existing submodules.
foreach(dependency IN ITEMS glfw glm miniaudio spdlog)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/${dependency}/CMakeLists.txt")
        message(FATAL_ERROR "Missing ${dependency}: run git submodule update --init --recursive")
    endif()
endforeach()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Install GLFW")
set(GLFW_LIBRARY_TYPE STATIC)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "Build GLM as a compiled library")
set(GLM_BUILD_TESTS OFF CACHE BOOL "Build GLM tests")
set(GLM_BUILD_INSTALL OFF CACHE BOOL "Install GLM")
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/spdlog)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# The generated OpenGL 3.3 core loader is bundled with the engine.
add_library(scivibe_glad STATIC include/glad/src/glad.c)
target_include_directories(scivibe_glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/include")
target_link_libraries(scivibe_glad PUBLIC ${CMAKE_DL_LIBS})

# Compile miniaudio's implementation once, rather than in every application.
add_library(scivibe_miniaudio STATIC src/miniaudio.c)
target_include_directories(scivibe_miniaudio PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio")
target_link_libraries(scivibe_miniaudio PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
if(UNIX)
    target_link_libraries(scivibe_miniaudio PUBLIC m)
endif()

add_library(scivibe STATIC 
    src/scivibe.cpp
    include/log/log.cpp
)

add_library(scivibe::engine ALIAS scivibe)
target_compile_features(scivibe PUBLIC cxx_std_17)
target_compile_definitions(scivibe PUBLIC GLFW_INCLUDE_NONE)
target_include_directories(scivibe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(scivibe PUBLIC
    scivibe_glad
    glfw glm::glm
    scivibe_miniaudio
    OpenGL::GL
    spdlog::spdlog
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ScivibeApplication.cmake")
