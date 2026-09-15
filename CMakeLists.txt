cmake_minimum_required(VERSION 3.16)

project(scivibe LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

# permet d'afficher l'emplacement
function(print_imported_target_info target_name)
    if(TARGET ${target_name})
        get_target_property(target_type ${target_name} TYPE)
        get_target_property(imported_location ${target_name} IMPORTED_LOCATION)
        get_target_property(imported_implib ${target_name} IMPORTED_IMPLIB)
        get_target_property(imported_configs ${target_name} IMPORTED_CONFIGURATIONS)
        get_target_property(interface_includes ${target_name} INTERFACE_INCLUDE_DIRECTORIES)

        message(STATUS "${target_name} type: ${target_type}")

        if(imported_location)
            message(STATUS "${target_name} IMPORTED_LOCATION: ${imported_location}")
        endif()

        if(imported_implib)
            message(STATUS "${target_name} IMPORTED_IMPLIB: ${imported_implib}")
        endif()

        if(imported_configs)
            foreach(config IN LISTS imported_configs)
                get_target_property(config_location ${target_name} "IMPORTED_LOCATION_${config}")
                get_target_property(config_implib ${target_name} "IMPORTED_IMPLIB_${config}")

                if(config_location)
                    message(STATUS "${target_name} IMPORTED_LOCATION_${config}: ${config_location}")
                endif()

                if(config_implib)
                    message(STATUS "${target_name} IMPORTED_IMPLIB_${config}: ${config_implib}")
                endif()
            endforeach()
        endif()

        if(interface_includes)
            message(STATUS "${target_name} INTERFACE_INCLUDE_DIRECTORIES: ${interface_includes}")
        endif()
    else()
        message(STATUS "Target ${target_name} not found")
    endif()
endfunction()


print_imported_target_info(glfw)
print_imported_target_info(OpenGL::GL)

add_executable(scivibe
    src/main.cpp
    include/glad/src/glad.c
)

target_include_directories(scivibe PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glad/include
)

target_link_libraries(scivibe PRIVATE
    glfw
    OpenGL::GL
)

# Copie les DLL de MinGW UCRT64 deja installees a cote de l'executable.
# Windows peut ainsi les trouver quel que soit le PATH du terminal.
if(MINGW)
    get_filename_component(mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    foreach(runtime_dll IN ITEMS libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
        set(runtime_path "${mingw_bin_dir}/${runtime_dll}")
        if(NOT EXISTS "${runtime_path}")
            message(FATAL_ERROR "DLL MinGW introuvable : ${runtime_path}")
        endif()
        add_custom_command(TARGET scivibe POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${runtime_path}" "$<TARGET_FILE_DIR:scivibe>"
            VERBATIM
        )
    endforeach()
endif()

if(WIN32)
    get_target_property(glfw_type glfw TYPE)
    if(glfw_type STREQUAL "SHARED_LIBRARY")
        add_custom_command(TARGET scivibe POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:glfw>" "$<TARGET_FILE_DIR:scivibe>"
            VERBATIM
        )
    endif()
endif()
