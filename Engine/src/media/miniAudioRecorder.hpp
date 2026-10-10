#pragma once
#include <miniaudio.h>
#include "media/audioRecorder.hpp"
#include <atomic>
namespace scivibe{
    class SCIVIBE_API MiniAudioRecorder : public AudioRecorder {
    public:
        MiniAudioRecorder();
        ~MiniAudioRecorder() noexcept override;
        MiniAudioRecorder(const MiniAudioRecorder&) = delete;
        MiniAudioRecorder& operator=(const MiniAudioRecorder&) = delete;

        // Les operations signalent leurs erreurs par des exceptions.
        void Init(const std::string& filename) override;
        void StartRecording(const double time) override;
        void StopRecording() override;

    private:
        static void DataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
        void ReleaseResources() noexcept;
        ma_device m_Device{};
        ma_encoder m_Encoder{};
        bool m_DeviceInitialized = false;
        bool m_EncoderInitialized = false;
        double m_RecordingStartTime = 0.0;
        std::atomic<ma_result> m_WriteError{MA_SUCCESS};
    };
}