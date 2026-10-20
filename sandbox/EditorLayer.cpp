#include <scivibe.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <imgui.h>
#include "sandbox2D.hpp"
#include "core/entryPoint.hpp"


/*ca peut etre introduit dans scivibe en tant qu'editor*/
class Sandbox : public scivibe::Application{
    public:
        Sandbox(): scivibe::Application("Scivibe Editor"){
            PushLayer(new Sandbox2D());
        }
        ~Sandbox(){}
};

scivibe::Application* scivibe::CreateApplication(){
    return new Sandbox();
}
