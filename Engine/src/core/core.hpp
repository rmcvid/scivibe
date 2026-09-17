#pragma once

// SCIVIBE_BUILD_DLL is defined only while compiling the engine itself.
#if defined(_WIN32)
    #if defined(SCIVIBE_BUILD_DLL)
        #define SCIVIBE_API __declspec(dllexport)
    #else
        #define SCIVIBE_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define SCIVIBE_API __attribute__((visibility("default")))
#else
    #define SCIVIBE_API
#endif

