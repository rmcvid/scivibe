#type vertex
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexIndex;
layout (location = 4) in vec4 aTintColor;
layout (location = 5) in float aScale;
uniform mat4 uViewProjection;

out vec3 vPos;
out vec4 vColor;
out vec2 vTexCoord;
out float vTexIndex;
out vec4 vTintColor;
out float vScale;

void main(){
    vPos = aPosition;
    vColor = aColor;
    vTexCoord = aTexCoord;
    vTexIndex = aTexIndex;
    vTintColor = aTintColor;
    vScale = aScale;
    gl_Position = uViewProjection * vec4(aPosition, 1.0f);
}


#type fragment
#version 330 core
layout (location = 0) out vec4 color;

in vec3 vPos;
in vec4 vColor;
in vec2 vTexCoord;
in float vTexIndex;
in vec4 vTintColor;
in float vScale;

uniform sampler2D uTexture[32];

void main()
{
   color = texture(uTexture[int(vTexIndex)],vTexCoord*vScale)*vColor*vTintColor;
}
