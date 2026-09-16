include_guard(GLOBAL)

function(_scivibe_copy_mingw_runtime target)
    if(MINGW)
        get_filename_component(compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
        foreach(runtime_dll IN ITEMS libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
            set(runtime_path "${compiler_directory}/${runtime_dll}")
            if(EXISTS "${runtime_path}")
                add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${runtime_path}" "$<TARGET_FILE_DIR:${target}>"
                    VERBATIM
                )
            endif()
        endforeach()
    endif()
endfunction()

# Usage: scivibe_add_executable(my_application main.cpp other.cpp)
macro(scivibe_add_executable target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} PRIVATE scivibe::engine)
    _scivibe_copy_mingw_runtime(${target})
endmacro()
