#include "shader/shader.hpp"
#include "glm/gtc/type_ptr.hpp"
namespace scivibe{
    Shader::Shader(const char* vertexPath, const char* fragmentPath){
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertex);
        glAttachShader(m_ID, fragment);
        glLinkProgram(m_ID);
        checkCompileErrors(m_ID, "PROGRAM");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    Shader::~Shader(){
        glDeleteProgram(m_ID);
    }
    void Shader::use(){ 
        //bind and use are similar
        glUseProgram(m_ID); 
    }
    void Shader::setBool(const std::string &name, bool value) const{         
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value); 
    }
    void Shader::setInt(const std::string &name, int value) const{ 
            glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value); 
    }
    void Shader::setFloat(const std::string &name, float value) const{ 
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value); 
    }
    void Shader::Bind() const{
        // bind and use are similar
        glUseProgram(m_ID);

    }
    void Shader::UnBind() const{
        glUseProgram(0);

    }

    void Shader::setUniformMat4(const std::string& name, const glm::mat4& mat ){
        
        GLint location = glGetUniformLocation(m_ID,name.c_str());
        glUniformMatrix4fv(location,1,GL_FALSE,glm::value_ptr(mat));
    }

}