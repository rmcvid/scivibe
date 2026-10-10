#include "media/audioRecorder.hpp"
#include "media/miniAudioRecorder.hpp"

namespace scivibe{
    Scope<AudioRecorder> AudioRecorder::Create(){
        return CreateScope<MiniAudioRecorder>();
    }
}