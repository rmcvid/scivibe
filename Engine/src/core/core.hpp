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

// à definir dans le cmake ?
#ifdef SCIVIBE_ENABLE_ASSERT
    #define SCIVIBE_ASSERT(x,...){ if(!(x)){SCIVIBE_ERROR("Assertion failled:{0}",__VA_ARGS__); __debugbreak(); }}
    #define SCIVIBE_CORE_ASSERT(x,...){ if(!(x)){SCIVIBE_CORE_ERROR("Assertion failled:{0}",__VA_ARGS__); __debugbreak(); }}
#else
    #define SCIVIBE_ASSERT(x,...)
    #define SCIVIBE_CORE_ASSERT(x,...)    
#endif

#define SCIVIBE_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)


