#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "log/log.hpp"

// from LearnOpenGL
namespace scivibe{
    class SCIVIBE_API Shader{
    public:
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        virtual ~Shader() = default;
        virtual void use() = 0;
        virtual void Bind() const = 0 ;
        virtual void UnBind() const = 0 ;

        //virtual void UploadVSRendererUniformBuffer();

        static Shader* Create(const std::string& filePath);
        static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc = "");

    
    };
}
#endif
