cmake_minimum_required(VERSION 3.16)
project(scivibe LANGUAGES C CXX)

option(SCIVIBE_BUILD_SANDBOX "Build the SciVibe sandbox" ON)

add_subdirectory(Engine)
if(SCIVIBE_BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()
