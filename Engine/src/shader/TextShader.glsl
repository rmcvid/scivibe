#type vertex
#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexIndex;
layout (location = 4) in vec4 aTintColor;

uniform mat4 uViewProjection;

flat out int vTexIndex;
out vec2 vTexCoord;
out vec4 vColor;

void main()
{
    vTexIndex = int(aTexIndex);
    vTexCoord = aTexCoord;
    vColor = aColor * aTintColor;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}

#type fragment
#version 330 core

in vec2 vTexCoord;
in vec4 vColor;
flat in int vTexIndex;

layout (location = 0) out vec4 color;

uniform sampler2D uSdfAtlases[32];
float SampleSdfAtlas(int index, vec2 uv)
{
    // Calculer les dérivées avant de choisir l'atlas.
    vec2 uvDx = dFdx(uv);
    vec2 uvDy = dFdy(uv);

    // Chaque cas utilise un index constant, compatible avec GLSL 330.
    #define SAMPLE_ATLAS(slot) \
        case slot: return textureGrad(uSdfAtlases[slot], uv, uvDx, uvDy).a;

    switch (index)
    {
        SAMPLE_ATLAS(0)
        SAMPLE_ATLAS(1)
        SAMPLE_ATLAS(2)
        SAMPLE_ATLAS(3)
        SAMPLE_ATLAS(4)
        SAMPLE_ATLAS(5)
        SAMPLE_ATLAS(6)
        SAMPLE_ATLAS(7)
        SAMPLE_ATLAS(8)
        SAMPLE_ATLAS(9)
        SAMPLE_ATLAS(10)
        SAMPLE_ATLAS(11)
        SAMPLE_ATLAS(12)
        SAMPLE_ATLAS(13)
        SAMPLE_ATLAS(14)
        SAMPLE_ATLAS(15)
        SAMPLE_ATLAS(16)
        SAMPLE_ATLAS(17)
        SAMPLE_ATLAS(18)
        SAMPLE_ATLAS(19)
        SAMPLE_ATLAS(20)
        SAMPLE_ATLAS(21)
        SAMPLE_ATLAS(22)
        SAMPLE_ATLAS(23)
        SAMPLE_ATLAS(24)
        SAMPLE_ATLAS(25)
        SAMPLE_ATLAS(26)
        SAMPLE_ATLAS(27)
        SAMPLE_ATLAS(28)
        SAMPLE_ATLAS(29)
        SAMPLE_ATLAS(30)
        SAMPLE_ATLAS(31)
    }

    #undef SAMPLE_ATLAS

    return 0.0;
}

void main()
{
    float distanceValue = SampleSdfAtlas(vTexIndex, vTexCoord);
    const float contour = 128.0 / 255.0;
    float transition = max(fwidth(distanceValue), 0.00001);
    float coverage = clamp((distanceValue - contour) / transition + 0.5,0.0,1.0);
    float alpha = vColor.a * coverage;
    if (alpha <= 0.0)
        discard;
    color = vec4(vColor.rgb, alpha);
}
