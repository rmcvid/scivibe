include_guard(GLOBAL)
include(ExternalProject)
include(ProcessorCount)

set(_ffmpeg_source "${CMAKE_CURRENT_LIST_DIR}/../external/ffmpeg")
if(NOT EXISTS "${_ffmpeg_source}/configure")
    message(FATAL_ERROR "Missing FFmpeg: run git submodule update --init --recursive")
endif()

if(WIN32 AND NOT MINGW)
    message(FATAL_ERROR "The FFmpeg source build currently requires MinGW/MSYS2 on Windows.")
endif()

get_filename_component(_compiler_bin "${CMAKE_C_COMPILER}" DIRECTORY)
get_filename_component(_toolchain_prefix "${_compiler_bin}" DIRECTORY)
get_filename_component(_msys_root "${_toolchain_prefix}" DIRECTORY)
find_program(SCIVIBE_FFMPEG_BASH NAMES bash
    HINTS "${_msys_root}/usr/bin" REQUIRED)
find_program(SCIVIBE_FFMPEG_MAKE NAMES gmake make
    HINTS "${_msys_root}/usr/bin" REQUIRED)
find_program(SCIVIBE_FFMPEG_NASM NAMES nasm HINTS "${_compiler_bin}")
if(NOT SCIVIBE_FFMPEG_NASM)
    set(SCIVIBE_FFMPEG_NASM "")
    message(STATUS "FFmpeg: NASM not found; building without x86 assembly optimizations")
endif()

ProcessorCount(_ffmpeg_jobs)
if(_ffmpeg_jobs LESS 1)
    set(_ffmpeg_jobs 1)
elseif(_ffmpeg_jobs GREATER 8)
    set(_ffmpeg_jobs 8)
endif()
set(SCIVIBE_FFMPEG_JOBS "${_ffmpeg_jobs}" CACHE STRING "Parallel jobs for the FFmpeg source build")

set(_ffmpeg_root "${CMAKE_CURRENT_BINARY_DIR}/ffmpeg")
set(_ffmpeg_install "${_ffmpeg_root}/install")
set(_ffmpeg_script "${CMAKE_CURRENT_LIST_DIR}/BuildFFmpeg.sh")
# Imported include paths must exist when CMake generates the build system.
file(MAKE_DIRECTORY "${_ffmpeg_install}/include")

set(_ffmpeg_components avutil swresample swscale avcodec avformat avfilter avdevice)
set(_ffmpeg_byproducts)
foreach(component IN LISTS _ffmpeg_components)
    string(TOUPPER "${component}" upper_component)
    set(version_header "${_ffmpeg_source}/lib${component}/version_major.h")
    file(STRINGS "${version_header}" major_line
        REGEX "^#define LIB${upper_component}_VERSION_MAJOR +[0-9]+")
    if(NOT major_line)
        set(version_header "${_ffmpeg_source}/lib${component}/version.h")
        file(STRINGS "${version_header}" major_line
            REGEX "^#define LIB${upper_component}_VERSION_MAJOR +[0-9]+")
    endif()
    string(REGEX MATCH "[0-9]+$" major "${major_line}")
    if(NOT major)
        message(FATAL_ERROR "Cannot determine FFmpeg ABI version from ${version_header}")
    endif()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${version_header}")

    add_library(FFmpeg::${component} SHARED IMPORTED GLOBAL)
    set_target_properties(FFmpeg::${component} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_ffmpeg_install}/include")
    if(WIN32)
        set(runtime "${_ffmpeg_install}/bin/${component}-${major}.dll")
        set(import_library "${_ffmpeg_install}/lib/lib${component}.dll.a")
        set_target_properties(FFmpeg::${component} PROPERTIES
            IMPORTED_LOCATION "${runtime}"
            IMPORTED_IMPLIB "${import_library}")
        list(APPEND _ffmpeg_byproducts "${runtime}" "${import_library}")
    elseif(APPLE)
        set(runtime "${_ffmpeg_install}/lib/lib${component}.${major}.dylib")
        set_target_properties(FFmpeg::${component} PROPERTIES IMPORTED_LOCATION "${runtime}")
        list(APPEND _ffmpeg_byproducts "${runtime}")
    else()
        set(runtime "${_ffmpeg_install}/lib/lib${component}.so.${major}")
        set_target_properties(FFmpeg::${component} PROPERTIES
            IMPORTED_LOCATION "${runtime}" IMPORTED_SONAME "lib${component}.so.${major}")
        list(APPEND _ffmpeg_byproducts "${runtime}")
    endif()
endforeach()

# Each command runs in MSYS2 on Windows, using the exact compiler selected by CMake.
set(_ffmpeg_args "${_ffmpeg_source}" "${_ffmpeg_root}/build" "${_ffmpeg_install}"
    "${CMAKE_C_COMPILER}" "${CMAKE_AR}" "${CMAKE_RANLIB}" "${CMAKE_STRIP}"
    "${SCIVIBE_FFMPEG_MAKE}" "${SCIVIBE_FFMPEG_JOBS}" "${SCIVIBE_FFMPEG_NASM}")
ExternalProject_Add(scivibe_ffmpeg_build
    PREFIX "${_ffmpeg_root}/project"
    SOURCE_DIR "${_ffmpeg_source}"
    BINARY_DIR "${_ffmpeg_root}/build"
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND "${SCIVIBE_FFMPEG_BASH}" "${_ffmpeg_script}" configure ${_ffmpeg_args}
    BUILD_COMMAND "${SCIVIBE_FFMPEG_BASH}" "${_ffmpeg_script}" build ${_ffmpeg_args}
    INSTALL_COMMAND "${SCIVIBE_FFMPEG_BASH}" "${_ffmpeg_script}" install ${_ffmpeg_args}
    BUILD_BYPRODUCTS ${_ffmpeg_byproducts}
    LOG_CONFIGURE TRUE
    # Show compiler progress instead of appearing frozen at a single percentage.
    LOG_BUILD FALSE
    LOG_INSTALL TRUE
    LOG_OUTPUT_ON_FAILURE TRUE)
ExternalProject_Add_StepDependencies(scivibe_ffmpeg_build configure
    "${_ffmpeg_script}" "${_ffmpeg_source}/configure")

foreach(component IN LISTS _ffmpeg_components)
    add_dependencies(FFmpeg::${component} scivibe_ffmpeg_build)
endforeach()
target_link_libraries(FFmpeg::swresample INTERFACE FFmpeg::avutil)
target_link_libraries(FFmpeg::swscale INTERFACE FFmpeg::avutil)
target_link_libraries(FFmpeg::avcodec INTERFACE FFmpeg::swresample FFmpeg::avutil)
target_link_libraries(FFmpeg::avformat INTERFACE FFmpeg::avcodec FFmpeg::avutil)
target_link_libraries(FFmpeg::avfilter INTERFACE FFmpeg::avformat FFmpeg::avcodec
    FFmpeg::swscale FFmpeg::swresample FFmpeg::avutil)
target_link_libraries(FFmpeg::avdevice INTERFACE FFmpeg::avfilter FFmpeg::avformat FFmpeg::avutil)

add_library(scivibe_ffmpeg INTERFACE)
add_library(FFmpeg::FFmpeg ALIAS scivibe_ffmpeg)
target_link_libraries(scivibe_ffmpeg INTERFACE FFmpeg::avdevice FFmpeg::avfilter
    FFmpeg::avformat FFmpeg::avcodec FFmpeg::swscale FFmpeg::swresample FFmpeg::avutil)
