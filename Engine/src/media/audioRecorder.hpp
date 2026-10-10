#pragma once
#include "pch/pch.hpp"
#include "core/capturedFrame.hpp"

namespace scivibe{
    class AudioRecorder{
        public:
            virtual ~AudioRecorder() = default;
            static Scope<AudioRecorder> Create();
            virtual void Init(const std::string& filename ) = 0;
            virtual void StartRecording(const double time) = 0;
            virtual void StopRecording() = 0;
            virtual bool IsRecording() const {return m_IsRecording;};
        protected:
            bool m_IsRecording =  false;
    };
}