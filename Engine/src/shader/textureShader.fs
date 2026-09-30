#version 330 core
layout (location = 0) out vec4 color;

//in vec3 vPos;
in vec2 vTexCoord;
uniform sampler2D uTexture;
void main()
{
    color = texture(uTexture,vTexCoord);
}