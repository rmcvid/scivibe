cmake_minimum_required(VERSION 3.21)
project(scivibe LANGUAGES C CXX)

option(SCIVIBE_BUILD_SANDBOX "Build the SciVibe sandbox" ON)
option(SCIVIBE_BUILD_TESTS "Build the SciVibe shared-library tests" OFF)

add_subdirectory(Engine)
if(SCIVIBE_BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()

if(SCIVIBE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
