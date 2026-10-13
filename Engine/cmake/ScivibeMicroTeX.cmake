# Build MicroTeX's layout core for a future Scivibe graphics adapter.
# Upstream CMake also creates platform demos and requires a system TinyXML2.
# Keep this integration outside the submodule so upstream remains unmodified.
set(SCIVIBE_MICROTEX_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../external/microtex")

function(scivibe_add_microtex_tinyxml2)
    set(BUILD_TESTING OFF)
    set(tinyxml2_BUILD_TESTING OFF)
    set(tinyxml2_SHARED_LIBS OFF)
    set(tinyxml2_INSTALL_PKGCONFIG OFF)
    add_subdirectory(
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../external/tinyxml2"
        "${CMAKE_CURRENT_BINARY_DIR}/external/tinyxml2"
        EXCLUDE_FROM_ALL
    )
endfunction()
scivibe_add_microtex_tinyxml2()

set(SCIVIBE_MICROTEX_SOURCES
    "${SCIVIBE_MICROTEX_SOURCE_DIR}/src/latex.cpp"
    "${SCIVIBE_MICROTEX_SOURCE_DIR}/src/render.cpp"
)
foreach(component IN ITEMS atom box core fonts res utils)
    file(GLOB_RECURSE component_sources CONFIGURE_DEPENDS
        "${SCIVIBE_MICROTEX_SOURCE_DIR}/src/${component}/*.cpp"
    )
    list(APPEND SCIVIBE_MICROTEX_SOURCES ${component_sources})
endforeach()

add_library(scivibe_microtex STATIC ${SCIVIBE_MICROTEX_SOURCES})
add_library(MicroTeX::Core ALIAS scivibe_microtex)
target_compile_features(scivibe_microtex PUBLIC cxx_std_17)
target_include_directories(scivibe_microtex SYSTEM PUBLIC
    "${SCIVIBE_MICROTEX_SOURCE_DIR}/src"
)
target_link_libraries(scivibe_microtex PUBLIC tinyxml2::tinyxml2)
if(MSVC)
    target_compile_options(scivibe_microtex PRIVATE /utf-8)
endif()

# No platform backend or no-op graphics implementation is selected here.
# Before calling tex::LaTeX::init/parse, Scivibe must implement
# tex::Font::create, tex::Font::_create and tex::TextLayout::create,
# plus a tex::Graphics2D adapter for drawing through Renderer2D.
# Use the checked-in resources during development, like the shader paths.
set(SCIVIBE_MICROTEX_RESOURCE_DIR "${SCIVIBE_MICROTEX_SOURCE_DIR}/res")
