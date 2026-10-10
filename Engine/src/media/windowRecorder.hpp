#pragma once
#include "pch/pch.hpp"
#include "core/capturedFrame.hpp"

namespace scivibe{
    class WindowRecorder{
        public:
            virtual ~WindowRecorder() = default;
            static Scope<WindowRecorder> Create();
            virtual void Init(const std::string& filename, const int width, const int height, const int fps) = 0;
            virtual void StartRecording(const double time) = 0;
            virtual void StopRecording() = 0;
            virtual void RecordFrame(const uint8_t* rgba,int strideBytes,int64_t pts) = 0;
            virtual bool IsRecording() const = 0;
            virtual void SetCaptureData(const int width,const int height,const int fps) {
                m_FrameCaptureData.width = width;
                m_FrameCaptureData.height = height;
                m_FrameCaptureData.FPSRecorded = fps;
                m_FrameCaptureData.lastRecordingPts = -1;
            };
            virtual void SetLastRecordingPts(const int64_t pts) {m_FrameCaptureData.lastRecordingPts = pts;};
            virtual void SetRecordingStartTime(const double time){m_FrameCaptureData.recordingStartTime = time;};
            virtual int GetWidth() const {return m_FrameCaptureData.width;};
            virtual int GetHeight() const {return m_FrameCaptureData.height;};
            virtual int64_t GetLastRecordingPts() const {return m_FrameCaptureData.lastRecordingPts;};
            virtual double GetRecordingStartTime() const {return m_FrameCaptureData.recordingStartTime;};
            virtual int GetFPS() const {return m_FrameCaptureData.FPSRecorded;};

        protected:
            struct FrameCaptureData{
                int width;
                int height;
                int FPSRecorded = FPS_RECORD;
                double recordingStartTime;
                int64_t lastRecordingPts = -1;
            };
            FrameCaptureData m_FrameCaptureData;
        };

}