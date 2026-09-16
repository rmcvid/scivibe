#include "scivibe.h"

int main(){
    scivibe::Log::Init();
    SCIVIBE_CORE_WARN("Initialization done!");
    SCIVIBE_CORE_INFO("Hello, World!");
    SCIVIBE_CORE_ERROR("This is an error message!");
    SCIVIBE_CORE_FATAL("This is a fatal error message!");
    SCIVIBE_INFO("This is a client info message!");
    SCIVIBE_ERROR("This is a client error message!");
    SCIVIBE_FATAL("This is a client fatal error message!");
    scivibe::print();
    return 0;
}
