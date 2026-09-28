#version 330 core
layout (location = 0) in vec3 aPosition;

uniform mat4 uViewProjection;
uniform mat4 uTransform;

out vec3 vPos;

void main()
{
    vPos = aPosition;
    gl_Position = uViewProjection* uTransform* vec4(aPosition,1.0f);
}