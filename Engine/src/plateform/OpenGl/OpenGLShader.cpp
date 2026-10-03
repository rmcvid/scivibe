#include "pch/pch.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <array>
#include  <fstream>
namespace scivibe{

    OpenGLShader::OpenGLShader(const std::string& filePath)
    {
        std::string source = ReadFile(filePath);
        auto shaderSource = PreProcess(source);
        Compile(shaderSource);
        
        // extract name from the filePath
        auto lastSlash = filePath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = filePath.rfind('.');
        auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filePath.substr(lastSlash,count);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, 
        const std::string& fragmentSrc, const std::string& geometrySrc)
        : m_Name(name)
        {
        std::unordered_map<GLenum, std::string> sources;
        std::string vertex = ReadFile(vertexSrc);
        std::string fragment = ReadFile(fragmentSrc);
        sources[GL_VERTEX_SHADER] = vertex;
        sources[GL_FRAGMENT_SHADER] = fragment;
        if(!geometrySrc.empty()) {
            std::string geometry = ReadFile(geometrySrc);
            sources[GL_GEOMETRY_SHADER] = geometry;
        }
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader(){
        glDeleteProgram(m_ID);
    }
    std::string OpenGLShader::ReadFile(const std::string& filePath){
        std::string result;
        std::ifstream in(filePath, std::ios::in | std::ios::binary);
        if(in){
            in.seekg(0,std::ios::end);
            result.resize(in.tellg());
            in.seekg(0,std::ios::beg);
            in.read(&result[0],result.size());
            in.close();
        }
        else{
            SCIVIBE_CORE_ERROR("could not open the file '{0}'", filePath);
        }
        return result;
    }

    static GLenum ShaderTypeFromString(const std::string& type){
        if(type == "vertex") return GL_VERTEX_SHADER;
        if(type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
        if(type == "geometry") return GL_GEOMETRY_SHADER;
        SCIVIBE_CORE_ASSERT(false, "shader {0} doesnt exist", type);
        return 0;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum,std::string>& shaderSources){
        GLuint program = glCreateProgram();
        SCIVIBE_CORE_ASSERT(shaderSources.size() <= 3, "On supporte acutellement que 3 type de shaders");
        std::array<GLuint, 3> glShaderIds{};
        size_t glShaderIDindex = 0;
        for (auto& kv : shaderSources){
            GLenum type = kv.first;
            const std::string& source = kv.second;
            GLuint shader = glCreateShader(type);
            const GLchar* sourceCStr = source.c_str();
            glShaderSource(shader,1,&sourceCStr,0);
            glCompileShader(shader);
            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS,&isCompiled);
            if(isCompiled==GL_FALSE){
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
                std::vector<GLchar> infoLog(std::max(maxLength, 1));
                glGetShaderInfoLog(shader,maxLength,&maxLength,&infoLog[0]);
                SCIVIBE_CORE_ERROR("{0}",infoLog.data());
                glDeleteShader(shader);
                for (size_t i = 0; i < glShaderIDindex; ++i) {
                    glDeleteShader(glShaderIds[i]);
                }
                glDeleteProgram(program);
                return;
            }
            glAttachShader(program,shader);
            glShaderIds[glShaderIDindex++] = shader;
        }
        glLinkProgram(program);
        
        GLint isLinked = 0;
        glGetProgramiv(program,GL_LINK_STATUS,(int*)&isLinked);
        if(isLinked==GL_FALSE){
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(std::max(maxLength, 1));
            glGetProgramInfoLog(program,maxLength,&maxLength,&infoLog[0]);
            glDeleteProgram(program);
            for(size_t i = 0; i < glShaderIDindex; ++i){
                glDeleteShader(glShaderIds[i]);
            }

            SCIVIBE_CORE_ERROR("{0}",infoLog.data());
            return;
        }
        for(size_t i = 0; i < glShaderIDindex; ++i){
            glDeleteShader(glShaderIds[i]);
        }
        m_ID = program;
    }

    std::unordered_map<GLenum,std::string> OpenGLShader::PreProcess(const std::string& source){
        std::unordered_map<GLenum,std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken,0);
        while( pos != std::string::npos){
            size_t eol = source.find_first_of("\r\n",pos);
            if (eol == std::string::npos) {
                SCIVIBE_CORE_ERROR("Missing shader source after #type");
                return {};
            }
            size_t begin = pos + typeTokenLength +1;
            std::string type = source.substr(begin,eol-begin);
            type.erase(std::remove_if(type.begin(), type.end(),
                [](unsigned char c) { return std::isspace(c); }), type.end()
            );
            const GLenum shaderType = ShaderTypeFromString(type);
            if (!shaderType) {
                return {};
            }
            
            // Advance past the directive before looking for the next stage.
            size_t nextLinePos = source.find_first_not_of("\r\n",eol);
            if (nextLinePos == std::string::npos) {
                SCIVIBE_CORE_ERROR("Missing source for {0} shader", type);
                return {};
            }
            pos = source.find(typeToken, nextLinePos);
            shaderSources[shaderType] = source.substr(nextLinePos,
                pos == std::string::npos ? std::string::npos : pos - nextLinePos);
        }
        return shaderSources;
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
