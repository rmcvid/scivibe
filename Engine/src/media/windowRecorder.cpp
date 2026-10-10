#include "media/windowRecorder.hpp"
#include "media/ffmpegRecorder.hpp"

namespace scivibe{
    Scope<WindowRecorder> WindowRecorder::Create(){
        return CreateScope<ffmpegRecorder>();
    }
}