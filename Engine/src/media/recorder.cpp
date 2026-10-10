#include "media/recorder.hpp"
#include "core/timeStep.hpp"
// ici à suprimer
#include <filesystem>
namespace scivibe{
    Recorder::Recorder(){
        m_WindowRecorder = Scope<WindowRecorder>(WindowRecorder::Create());
        m_AudioRecorder = AudioRecorder::Create();
    }
    Recorder::~Recorder(){
        
    }
    void Recorder::Init(const std::string& filename,int width, int  height, int fps){
        if(!m_IsInit){
            m_WindowRecorder->Init(filename,width, height,fps);
            std::filesystem::path audioPath(filename);
            audioPath.replace_extension(".wav");
            m_AudioRecorder->Init(audioPath.string());//.wav à ajouter dedans 
            m_IsInit = true;
        }
    }
    void Recorder::StartRecording(){
        if(m_IsRecording) 
            throw std::logic_error("Un enregistrement est deja en cours");
        const double time = Timestep::GetTime();
        m_WindowRecorder->StartRecording(time);
        m_AudioRecorder->StartRecording(Timestep::GetTime());
        m_IsRecording = true;
    }
    void Recorder::Update(){

    }
    void Recorder::RecordImageFrame(const uint8_t* rgba, int strideBytes, int64_t pts){
        m_WindowRecorder->RecordFrame(rgba,strideBytes,pts);

    }
    void Recorder::StopRecording(){
        const double time = Timestep::GetTime();
        m_WindowRecorder->StopRecording();
        m_AudioRecorder->StopRecording();
        m_IsRecording = false;
        m_IsInit = false;
        
    }
    void Recorder::SubmitFrame(const CapturedFrame& frame){
        if (!IsRecording())
        return;

        if (frame.Width <= 0 || frame.Height <= 0 ||frame.StrideBytes <= 0 || frame.Pixels.empty())
            return;

        if (frame.Width != GetWidth() || frame.Height != GetHeight()) {
            StopRecording();
            return;
        }

        const int64_t pts = static_cast<int64_t>(
            (Timestep::GetTime() - GetRecordingStartTime()) * GetFPS()
        );

        if (pts <= GetLastRecordingPts())
            return;

        const uint8_t* topRow =frame.Pixels.data()+ static_cast<size_t>(frame.Height - 1) * frame.StrideBytes;
        RecordImageFrame(topRow, -frame.StrideBytes, pts);
        SetLastRecordingPts(pts);
    
    }
    

}