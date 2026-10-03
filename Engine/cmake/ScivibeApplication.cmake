include_guard(GLOBAL)

function(_scivibe_copy_mingw_runtime target)
    if(MINGW)
        get_filename_component(compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(runtime_paths)
        foreach(runtime_dll IN ITEMS libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
            set(runtime_path "${compiler_directory}/${runtime_dll}")
            if(EXISTS "${runtime_path}")
                list(APPEND runtime_paths "${runtime_path}")
            endif()
        endforeach()
        if(runtime_paths)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    ${runtime_paths} "$<TARGET_FILE_DIR:${target}>"
                VERBATIM
            )
        endif()
    endif()
endfunction()

# Usage: scivibe_add_executable(my_application main.cpp other.cpp)
macro(scivibe_add_executable target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} PRIVATE scivibe::engine)
    if(WIN32)
        # Run even when only the engine changed and the executable is up to date.
        add_custom_target(${target}_runtime
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${target}>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${target}> "$<TARGET_FILE_DIR:${target}>"
            DEPENDS scivibe
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
        add_dependencies(${target} ${target}_runtime)
    endif()
    _scivibe_copy_mingw_runtime(${target})
endmacro()
