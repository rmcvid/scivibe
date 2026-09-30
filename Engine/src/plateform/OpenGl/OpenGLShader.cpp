#include "glm/gtc/type_ptr.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"
namespace scivibe{
    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc){
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;

        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            vShaderFile.open(vertexSrc);
            fShaderFile.open(fragmentSrc);
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
        unsigned int vertex, fragment, geometry;
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

        if(!geometrySrc.empty()){
            try {
                gShaderFile.open(geometrySrc);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode   = gShaderStream.str();
            }
            catch (std::ifstream::failure& e)
            {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            }
            const char * gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "Geometry");
            glAttachShader(m_ID,geometry);
        }
        glLinkProgram(m_ID);
        checkCompileErrors(m_ID, "PROGRAM");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(geometry);
    }
    OpenGLShader::~OpenGLShader(){
        glDeleteProgram(m_ID);
    }
    void OpenGLShader::use(){ 
        //bind and use are similar
        glUseProgram(m_ID); 
    }
    void OpenGLShader::setBool(const std::string &name, bool value) const{         
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value); 
    }
    void OpenGLShader::setInt(const std::string &name, int value) const{ 
            glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value); 
    }
    void OpenGLShader::UploadUniformInt( const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }
    void OpenGLShader::UploadUniformFloat(const std::string &name, const float value) const{ 
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value); 
    }
    void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2& value) const{
        glUniform2f(glGetUniformLocation(m_ID, name.c_str()), value.x,value.y); 
    }
    void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3& value) const{
        glUniform3f(glGetUniformLocation(m_ID, name.c_str()), value.x,value.y,value.z); 
    }
    void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4& value) const{
        glUniform4f(glGetUniformLocation(m_ID, name.c_str()), value.x,value.y,value.z,value.w); 
    }

    
    void OpenGLShader::Bind() const{
        // bind and use are similar
        glUseProgram(m_ID);

    }
    void OpenGLShader::UnBind() const{
        glUseProgram(0);

    }
    void OpenGLShader::setUniformMat3(const std::string& name, const glm::mat3& mat){
        GLint location = glGetUniformLocation(m_ID,name.c_str());
        glUniformMatrix3fv(location,1,GL_FALSE,glm::value_ptr(mat));
    }
    void OpenGLShader::setUniformMat4(const std::string& name, const glm::mat4& mat ){
        
        GLint location = glGetUniformLocation(m_ID,name.c_str());
        glUniformMatrix4fv(location,1,GL_FALSE,glm::value_ptr(mat));
    }

}