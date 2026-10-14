#type vertex
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}  

#type fragment
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampledColor =
        texture(uTexture[int(vTexIndex)], vTexCoord * vScale)
        * vColor
        * vTintColor;

    if (sampledColor.a <= 0.0)
        discard;

    color = sampledColor;
}