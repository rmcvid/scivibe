scivibe_add_executable(sandbox test.cpp)

target_compile_definitions(sandbox PRIVATE
    "IMAGE_SANDBOX_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/image/\""
)

# Keep a separate PCH: the sandbox imports the DLL, while the engine exports it.
target_precompile_headers(sandbox PRIVATE
    <pch/pch.hpp>
    <glm/glm.hpp>
    <miniaudio.h>
)
