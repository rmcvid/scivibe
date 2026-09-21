#include <scivibe.h>

class ExampleLayer : public scivibe::Layer
{
    public :
    ExampleLayer()
        : Layer("Example")
        {

        }
    
        void OnUpdate() override{
           
        }

        void OnEvent(scivibe::Event& event) override{
            SCIVIBE_TRACE("{0}",event);
        }
};
class Sandbox : public scivibe::Application{
    public:
        Sandbox(){
            PushLayer(new ExampleLayer());
        }
        ~Sandbox(){}
};

scivibe::Application* scivibe::CreateApplication(){
    return new Sandbox();
}



/*
int main(){
    scivibe::Log::Init();
    scivibe::Application app;
    app.Run();
    return 0;

}*/