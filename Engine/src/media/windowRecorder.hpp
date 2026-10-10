#pragma once
#include "pch/pch.hpp"
#include "core/capturedFrame.hpp"

namespace scivibe{
    class WindowRecorder{
        public:
            virtual ~WindowRecorder() = default;
            static Scope<WindowRecorder> Create();
            virtual void StartRecording(const std::string& filename) = 0;
            virtual void StopRecording() = 0;
            virtual void RecordFrame(const uint8_t* rgba,int strideBytes,int64_t pts) = 0;
            virtual bool IsRecording() const = 0;
            virtual void SetCaptureData(int width, int height, int fps, double startTime) {
                m_FrameCaptureData.width = width;
                m_FrameCaptureData.height = height;
                m_FrameCaptureData.FPSRecorded = fps;
                m_FrameCaptureData.recordingStartTime = startTime;
                m_FrameCaptureData.lastRecordingPts = -1;
            };
            virtual void SetLastRecordingPts(int64_t pts) {m_FrameCaptureData.lastRecordingPts = pts;};

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