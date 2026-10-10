#pragma once
#include "pch/pch.hpp"
#include "core/capturedFrame.hpp"
#include "media/windowRecorder.hpp"
#include "media/audioRecorder.hpp"
namespace scivibe{
    class Recorder{
        public:
            Recorder();
            ~Recorder();
            void Init(const std::string& filename,int width, int  height, int fps);
            void StartRecording();
            void Update();
            void StopRecording();

            bool IsRecording() const{ return m_IsRecording;};
            bool AudioIsRecording() const{ return m_AudioIsRecording;};
            bool WindowIsRecording() const{ return m_WindowIsRecording;};

            const double GetRecordingStartTime(){return m_WindowRecorder->GetRecordingStartTime();};
            int64_t GetLastRecordingPts() const {return m_WindowRecorder->GetLastRecordingPts();};
            void SetLastRecordingPts(const int64_t pts){ m_WindowRecorder->SetLastRecordingPts(pts);};
            void RecordAudioFrame();
            void SubmitFrame(const CapturedFrame& frame);
            void RecordImageFrame(const uint8_t* rgba, int strideBytes, int64_t pts);
            void RecordFrame();

            int GetWidth() const{return m_WindowRecorder->GetWidth();};
            int GetHeight() const{return m_WindowRecorder->GetHeight();};
            int GetFPS() const {return m_WindowRecorder->GetFPS();};
        private:
            Scope<WindowRecorder> m_WindowRecorder;
            Scope<AudioRecorder>  m_AudioRecorder;
            bool m_IsRecording = false;
            bool m_AudioIsRecording = false;
            bool m_WindowIsRecording = false;
            bool m_IsInit = false;

    };

}
