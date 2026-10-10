# Recheck src on every build so new source files are picked up automatically.
file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
file(GLOB_RECURSE SANDBOX_SHADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.glsl"
)
if(SANDBOX_SHADERS)
    set_source_files_properties(${SANDBOX_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
scivibe_add_executable(sandbox test.cpp ${SANDBOX_SOURCES} ${SANDBOX_SHADERS})
target_include_directories(sandbox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_definitions(sandbox PRIVATE
    "IMAGE_SANDBOX_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/image/\""
    "SHADER_SANDBOX_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/src/\""
    "VIDEO_SANDBOX_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/video/\""
)

# Keep a separate PCH: the sandbox imports the DLL, while the engine exports it.
target_precompile_headers(sandbox PRIVATE
    <pch/pch.hpp>
    <glm/glm.hpp>
    <glm/gtc/matrix_transform.hpp>
    <glm/gtc/type_ptr.hpp>
    <spdlog/sinks/stdout_color_sinks.h>
    <imgui.h>
    <miniaudio.h>
)
