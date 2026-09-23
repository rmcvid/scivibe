#version 330 core
out vec4 FragColor;

///layout (location = 0) out vec4 color;

in vec3 vPos;
in vec4 vColor;

void main()
{
    //FragColor = vec4(0.2,0.2,0.2, 1.0f);
    FragColor = vColor;

}