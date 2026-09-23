#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec3 vPos;
out vec4 vColor;

void main()
{
    vPos = aPos;
    vColor = aColor;
    gl_Position = vec4(aPos, 1.0);
}