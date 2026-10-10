#pragma once
#include "media/windowRecorder.hpp"
#include "media/ffmpeg.hpp"
#include "core/core.hpp"
#include <cstdint>
#include <string>

namespace scivibe {
    class SCIVIBE_API ffmpegRecorder : public WindowRecorder {
    public:
        ffmpegRecorder();
        ~ffmpegRecorder() noexcept override;
        ffmpegRecorder(const ffmpegRecorder&) = delete;
        ffmpegRecorder& operator=(const ffmpegRecorder&) = delete;

        // Les operations signalent leurs erreurs par des exceptions.
        void Init(const std::string& filename, const int width, const int height, const int fps) override;
        void StartRecording(const double time) override;
        void StopRecording() override;
        // pts est strictement croissant, en unites de 1/fps, et commence a zero ou plus.
        void RecordFrame(const uint8_t* rgba, int strideBytes, int64_t pts) override;
        bool IsRecording() const override { return m_IsRecording; }

    private:
        bool m_IsRecording = false;
        AVFormatContext* m_FormatContext = nullptr;
        AVCodecContext* m_CodecContext = nullptr;
        AVStream* m_Stream = nullptr;
        AVFrame* m_Frame = nullptr;
        AVPacket* m_Packet = nullptr;
        SwsContext* m_SwsContext = nullptr;
        int64_t m_LastPts = AV_NOPTS_VALUE;

        void EncodeFrame(AVFrame* frame);
        int DrainPackets();
        void ReleaseResources() noexcept;
    };
}
