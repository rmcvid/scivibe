#include "media/ffmpegRecorder.hpp"

#include <cerrno>
#include <exception>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include "pch/pch.hpp"
#include "core/log.hpp"

namespace scivibe {
    namespace {
        void CheckFFmpeg(int result, const char* operation) {
            if (result >= 0)
                return;

            char message[AV_ERROR_MAX_STRING_SIZE] = {};
            av_strerror(result, message, sizeof(message));
            throw std::runtime_error(std::string(operation) + " : " + message);
        }
    }
    ffmpegRecorder::ffmpegRecorder() 
    {
        SCIVIBE_CORE_INFO("Video Recorder Created");
    }
    Scope<WindowRecorder> WindowRecorder::Create(){
        return CreateScope<ffmpegRecorder>();
    }

    ffmpegRecorder::~ffmpegRecorder() noexcept {
        try {
            StopRecording();
            SCIVIBE_CORE_INFO("Video Recorder Destroyed");
        } catch (...) {
            ReleaseResources();
        }
    }

    void ffmpegRecorder::StartRecording(const std::string& filename) {
        if (m_IsRecording)
            throw std::logic_error("Un enregistrement est deja en cours");
        if (filename.empty() || m_FrameCaptureData.width  <= 0 || m_FrameCaptureData.height <= 0 || m_FrameCaptureData.FPSRecorded <= 0)
            throw std::invalid_argument("Nom de fichier, dimensions ou FPS invalides");
        if ((m_FrameCaptureData.width % 2) != 0 || (m_FrameCaptureData.height % 2) != 0)
            throw std::invalid_argument("YUV420P exige une largeur et une hauteur paires");
        if (m_FrameCaptureData.width > (std::numeric_limits<int>::max)() / 4)
            throw std::invalid_argument("Largeur trop grande pour un stride RGBA");

        ReleaseResources();
        try {
            CheckFFmpeg(avformat_alloc_output_context2(
                &m_FormatContext, nullptr, nullptr, filename.c_str()),
                "Creation du contexte de sortie");
            if (!m_FormatContext)
                SCIVIBE_CORE_ERROR("Contexte de sortie introuvable");

            // MPEG-4 est disponible dans le build FFmpeg actuel, contrairement a H.264.
            const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
            if (!codec)
                SCIVIBE_CORE_ERROR("Encodeur MPEG-4 introuvable");

            m_Stream = avformat_new_stream(m_FormatContext, nullptr);
            m_CodecContext = avcodec_alloc_context3(codec);
            if (!m_Stream || !m_CodecContext)
                throw std::bad_alloc();

            m_CodecContext->codec_id = codec->id;
            m_CodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
            m_CodecContext->width = m_FrameCaptureData.width;
            m_CodecContext->height = m_FrameCaptureData.height;
            m_CodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
            m_CodecContext->time_base = AVRational{1, m_FrameCaptureData.FPSRecorded};
            m_CodecContext->framerate = AVRational{m_FrameCaptureData.FPSRecorded, 1};
            m_CodecContext->bit_rate = 8'000'000;
            m_CodecContext->gop_size = 12;
            m_CodecContext->max_b_frames = 0;
            m_Stream->time_base = m_CodecContext->time_base;
            m_Stream->avg_frame_rate = m_CodecContext->framerate;

            if (m_FormatContext->oformat->flags & AVFMT_GLOBALHEADER)
                m_CodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            CheckFFmpeg(avcodec_open2(m_CodecContext, codec, nullptr),
                        "Ouverture de l'encodeur MPEG-4");
            CheckFFmpeg(avcodec_parameters_from_context(m_Stream->codecpar, m_CodecContext),
                        "Configuration de la piste video");

            m_Frame = av_frame_alloc();
            m_Packet = av_packet_alloc();
            if (!m_Frame || !m_Packet)
                throw std::bad_alloc();

            m_Frame->format = m_CodecContext->pix_fmt;
            m_Frame->width = m_FrameCaptureData.width;
            m_Frame->height = m_FrameCaptureData.height;
            CheckFFmpeg(av_frame_get_buffer(m_Frame, 32), "Allocation des pixels video");

            m_SwsContext = sws_getContext(
                m_FrameCaptureData.width, m_FrameCaptureData.height, AV_PIX_FMT_RGBA,
                m_FrameCaptureData.width, m_FrameCaptureData.height, m_CodecContext->pix_fmt,
                SWS_BILINEAR, nullptr, nullptr, nullptr);
            if (!m_SwsContext)
                SCIVIBE_CORE_ERROR("Creation du convertisseur RGBA vers YUV420P impossible");

            if (!(m_FormatContext->oformat->flags & AVFMT_NOFILE)) {
                CheckFFmpeg(avio_open(&m_FormatContext->pb, filename.c_str(), AVIO_FLAG_WRITE),
                            "Ouverture du fichier video");
            }

            CheckFFmpeg(avformat_write_header(m_FormatContext, nullptr), "Ecriture de l'en-tete video");
            m_LastPts = AV_NOPTS_VALUE;
            m_IsRecording = true;
        } catch (...) {
            ReleaseResources();
            throw;
        }
    }

    void ffmpegRecorder::StopRecording() {
        if (!m_IsRecording) { 
            ReleaseResources();
            SCIVIBE_CORE_INFO("Recording stopped:");
            return;
        }

        m_IsRecording = false;
        std::exception_ptr error;
        try {
            EncodeFrame(nullptr);
        } catch (...) {
            error = std::current_exception();
        }

        try {
            CheckFFmpeg(av_write_trailer(m_FormatContext), "Finalisation de la video");
            if (!(m_FormatContext->oformat->flags & AVFMT_NOFILE) && m_FormatContext->pb)
                CheckFFmpeg(avio_closep(&m_FormatContext->pb), "Fermeture du fichier video");
        } catch (...) {
            if (!error)
                error = std::current_exception();
        }

        ReleaseResources();
        if (error) std::rethrow_exception(error);
    }

    void ffmpegRecorder::RecordFrame(const uint8_t* rgba, int strideBytes, int64_t pts) {
        if (!m_IsRecording) throw std::logic_error("Aucun enregistrement en cours");

        const int64_t stride = strideBytes;
        const int64_t absoluteStride = stride < 0 ? -stride : stride;
        if (!rgba || absoluteStride < static_cast<int64_t>(m_CodecContext->width) * 4)
            throw std::invalid_argument("Buffer RGBA ou stride invalide");
        if (pts < 0 || (m_LastPts != AV_NOPTS_VALUE && pts <= m_LastPts))
            throw std::invalid_argument("Les PTS doivent etre positifs ou nuls et strictement croissants");

        CheckFFmpeg(av_frame_make_writable(m_Frame), "Preparation de l'image video");
        const uint8_t* sourceData[4] = {rgba, nullptr, nullptr, nullptr};
        int sourceStride[4] = {strideBytes, 0, 0, 0};
        const int rows = sws_scale(
            m_SwsContext, sourceData, sourceStride, 0, m_CodecContext->height,
            m_Frame->data, m_Frame->linesize);
        CheckFFmpeg(rows, "Conversion RGBA vers YUV420P");
        if (rows != m_CodecContext->height)
            SCIVIBE_CORE_ERROR("Conversion incomplete de l'image video");

        m_Frame->pts = pts;
        m_Frame->duration = 1;
        EncodeFrame(m_Frame);
    }

    void ffmpegRecorder::EncodeFrame(AVFrame* frame) {
        int result = avcodec_send_frame(m_CodecContext, frame);
        if (result == AVERROR(EAGAIN)) {
            DrainPackets();
            result = avcodec_send_frame(m_CodecContext, frame);
        }
        CheckFFmpeg(result, "Envoi de l'image a l'encodeur");

        // L'image est acceptee, meme si l'ecriture d'un paquet echoue ensuite.
        if (frame)
            m_LastPts = frame->pts;

        const int status = DrainPackets();
        if (!frame && status != AVERROR_EOF)
            SCIVIBE_CORE_ERROR("L'encodeur n'a pas termine sa finalisation");
    }

    int ffmpegRecorder::DrainPackets() {
        for (;;) {
            const int result = avcodec_receive_packet(m_CodecContext, m_Packet);
            if (result == AVERROR(EAGAIN) || result == AVERROR_EOF)
                return result;
            CheckFFmpeg(result, "Reception d'un paquet video");

            av_packet_rescale_ts(m_Packet, m_CodecContext->time_base, m_Stream->time_base);
            m_Packet->stream_index = m_Stream->index;

            const int writeResult = av_interleaved_write_frame(m_FormatContext, m_Packet);
            av_packet_unref(m_Packet);
            CheckFFmpeg(writeResult, "Ecriture d'un paquet video");
        }
    }

    void ffmpegRecorder::ReleaseResources() noexcept {
        av_frame_free(&m_Frame);
        av_packet_free(&m_Packet);
        avcodec_free_context(&m_CodecContext);
        sws_freeContext(m_SwsContext);
        m_SwsContext = nullptr;

        if (m_FormatContext) {
            if (!(m_FormatContext->oformat->flags & AVFMT_NOFILE) && m_FormatContext->pb)
                avio_closep(&m_FormatContext->pb);
            avformat_free_context(m_FormatContext);
        }

        m_FormatContext = nullptr;
        m_Stream = nullptr;
        m_LastPts = AV_NOPTS_VALUE;
        m_IsRecording = false;
    }
}

