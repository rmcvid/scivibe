#include "media/miniAudioRecorder.hpp"
#include "pch/pch.hpp"
#include "core/log.hpp"
#define RECORD_FREQUENCY 48000

namespace scivibe{
    namespace {
        void CheckAudio(ma_result result, const char* operation) {
            if (result != MA_SUCCESS) {
                throw std::runtime_error(std::string(operation) + " : " + ma_result_description(result)
                );
            }
        }
    }

    MiniAudioRecorder::MiniAudioRecorder() = default;

    MiniAudioRecorder::~MiniAudioRecorder() noexcept {
        ReleaseResources();
    }
   
    void MiniAudioRecorder::Init(const std::string& filename){
        if(m_IsRecording)
            throw std::logic_error("Enregistrement audio déjà en cours");
        if( filename.empty())
            throw std::invalid_argument("Nom de fichier audio vide");
        ReleaseResources();
        m_WriteError.store(MA_SUCCESS);

        try{
            const ma_encoder_config encoderConfig = 
            ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32,1,RECORD_FREQUENCY);
            CheckAudio( ma_encoder_init_file(filename.c_str(), &encoderConfig, &m_Encoder),"Ouverture du WAV");
            m_EncoderInitialized = true;
            ma_device_config config = ma_device_config_init(ma_device_type_capture);
            config.capture.format = ma_format_f32;
            config.capture.channels = 1;
            config.sampleRate = RECORD_FREQUENCY;
            config.dataCallback = DataCallback;
            config.pUserData = this;
            CheckAudio(ma_device_init(nullptr, &config, &m_Device), "Initialisation du microphone");
            m_DeviceInitialized = true;
        } catch (...) {
                ReleaseResources();
                throw;
        }
    }
    void MiniAudioRecorder::StartRecording(const double time){
        if(m_IsRecording) throw std::logic_error("Un enregistrement audio est deja en cours");
        if (!m_DeviceInitialized || !m_EncoderInitialized) throw std::logic_error("Appeler Init avant StartRecording");
        m_RecordingStartTime = time;
        CheckAudio(ma_device_start(&m_Device), "Demarrage du microphone");
        m_IsRecording = true;
        SCIVIBE_CORE_INFO("L'enregistement audio a commencer");
    }
    void MiniAudioRecorder::StopRecording() {
        ReleaseResources();
        CheckAudio(m_WriteError.load(),"Ecriture des echantillons audio");
        SCIVIBE_CORE_INFO("L'enregistement audio a terminer");
    }

    void MiniAudioRecorder::DataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount){
        (void) output;
        auto* recorder = static_cast<MiniAudioRecorder*>(device->pUserData);
        if(!input || recorder->m_WriteError.load()) return;
        ma_uint64 framesWritten = 0;
        const ma_result result = ma_encoder_write_pcm_frames(
        &recorder->m_Encoder,input,frameCount, &framesWritten );
        if (result != MA_SUCCESS) {
            recorder->m_WriteError.store(result);
        } else if (framesWritten != frameCount) {
            recorder->m_WriteError.store(MA_ERROR);
        }
    }

    void MiniAudioRecorder::ReleaseResources() noexcept{
        if (m_DeviceInitialized) {
        ma_device_uninit(&m_Device);
        m_DeviceInitialized = false;
        }
        if (m_EncoderInitialized) {
            ma_encoder_uninit(&m_Encoder);
            m_EncoderInitialized = false;
        }
        m_IsRecording = false;
    }

}
