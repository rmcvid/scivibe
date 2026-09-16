#include "scivibe.h"

int main(){
    scivibe::Log::Init();
    scivibe::Application app;
    app.Run();
    return 0;
}