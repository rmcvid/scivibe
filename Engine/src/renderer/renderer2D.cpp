#include "renderer/renderer2D.hpp"
#include "renderer/vertexArray.hpp"
#include "shader/shader.hpp"
#include "renderCommand.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/rendererCurve2D.hpp"
#include "core/application.hpp"

namespace scivibe{

    struct Vertex{
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        glm::vec4 TintColor;
        float Scale;
    };

    struct Renderer2DData{
        // per drawaquad
        static const uint32_t MaxQuads {1000};
        static const uint32_t MaxVertices {MaxQuads*4};
        static const uint32_t MaxIndices {MaxQuads*6};
        static const uint32_t MaxTextureSlots = 32; // render caps à utiliser ici
        Ref<VertexArray> objVertexArray;
        Ref<VertexBuffer> objVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Shader> TextShader;
        std::unique_ptr<Vertex[]> VertexBufferBase;
        Vertex* VertexBufferPtr = nullptr;
        Ref<Texture2D> WhiteTexture;
        std::unique_ptr<uint32_t[]> IndexBufferBase;
        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;
        std::array<Ref<Texture2D>,MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;
        Renderer2D::Statistics Stats;
        glm::vec4 VertexPositions[4];
        Renderer2D::PossibleState state = Renderer2D::PossibleState::Texture;
        //glm::mat4 ViewProjection{1.0f};
    };
    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        s_Data.objVertexArray = VertexArray::Create();
        s_Data.objVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(Vertex));
        s_Data.objVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "aPosition"},
            {ShaderDataType::Float4, "aColor"},
            {ShaderDataType::Float2, "aTexCoord"},
            {ShaderDataType::Float,  "aTexIndex"},
            {ShaderDataType::Float4,  "aTintColor"},
            {ShaderDataType::Float,  "aScale"},
        });
        s_Data.objVertexArray->AddVertexBuffer(s_Data.objVertexBuffer);
        s_Data.VertexBufferBase = std::make_unique<Vertex[]>(s_Data.MaxVertices);
        s_Data.IndexBufferBase = std::make_unique<uint32_t[]>(s_Data.MaxIndices);

        // Allocate GPU capacity once; each batch supplies its own indices.
        s_Data.objVertexArray->SetIndexBuffer(IndexBuffer::Create(nullptr, s_Data.MaxIndices));

        s_Data.WhiteTexture = Texture2D::Create(1,1);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData,sizeof(uint32_t));

        int samplers[s_Data.MaxTextureSlots];
        for(int i =0; i< s_Data.MaxTextureSlots; ++i){
            samplers[i] = i;
        }
        s_Data.TextShader = Shader::Create(SHADER_PATH "TextShader.glsl");
        s_Data.TextShader->Bind();
        s_Data.TextShader->SetIntArray("uSdfAtlases",samplers,s_Data.MaxTextureSlots);
        s_Data.TextureShader = Shader::Create( SHADER_PATH "TextureShader.glsl");
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetIntArray("uTexture",samplers ,s_Data.MaxTextureSlots);
        s_Data.TextureSlots[0] = s_Data.WhiteTexture;
        s_Data.VertexPositions[0] = {-0.5f,-0.5f,0.0f,1.0f};
        s_Data.VertexPositions[1] = { 0.5f,-0.5f,0.0f,1.0f};
        s_Data.VertexPositions[2] = { 0.5f, 0.5f,0.0f,1.0f};
        s_Data.VertexPositions[3] = {-0.5f, 0.5f,0.0f,1.0f};
        ResetData();
        ResetStats();
    }
    void Renderer2D::Shutdown(){
        // s_Data is static: never leave GPU objects for destruction after the window.
        s_Data.TextureSlots.fill(nullptr);
        s_Data.WhiteTexture.reset();
        s_Data.TextureShader.reset();
        s_Data.TextShader.reset();
        s_Data.objVertexArray.reset();
        s_Data.objVertexBuffer.reset();
        s_Data.IndexBufferBase.reset();
        s_Data.VertexBufferBase.reset();
        ResetData();
    }
    
    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        ResetData();
        const auto& spec = Application::Get().GetFrameBuffer()->GetSpecification();
        RendererCurve2D::SetView(camera.GetViewProjectionMatrix(),glm::uvec2{spec.Width, spec.Height});
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("uViewProjection",camera.GetViewProjectionMatrix());
        s_Data.TextShader->Bind();
        s_Data.TextShader->SetMat4("uViewProjection",camera.GetViewProjectionMatrix());
    }
    void Renderer2D::SwitchState(PossibleState state){
        switch (state)
        {
        case PossibleState::Texture:
            s_Data.TextureShader->Bind();
            break;
        case PossibleState::Text:
            s_Data.TextShader->Bind();
            break;
        }
        s_Data.state = state;
    }
    void Renderer2D::VerifyState(PossibleState state){
        /*Verifie que le renderer2D est dans le bonne état sinon il flush and reset*/
        if(state == s_Data.state) return;
        FlushAndReset();
        SwitchState(state);
    }
    void Renderer2D::Flush(){
        if (s_Data.IndexCount == 0)
            return;
        SwitchState(s_Data.state);
        s_Data.objVertexArray->Bind();
        s_Data.objVertexBuffer->SetData(
            s_Data.VertexBufferBase.get(), s_Data.VertexCount * sizeof(Vertex));
        // SetData takes a count of uint32_t indices, not a byte size.
        s_Data.objVertexArray->GetIndexBuffers()->SetData(
            s_Data.IndexBufferBase.get(), s_Data.IndexCount);
        for(uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i){
            s_Data.TextureSlots[i]->Bind(i);
        }
        RenderCommand::DrawIndexed(s_Data.objVertexArray, s_Data.IndexCount);
        ++ s_Data.Stats.DrawCalls;
        ResetData();
    }
    void Renderer2D::FlushAndReset(){
        Flush();
    }
    void Renderer2D::ResetData(){
        s_Data.VertexCount = 0;
        s_Data.IndexCount = 0;
        s_Data.VertexBufferPtr = s_Data.VertexBufferBase.get();
        s_Data.TextureSlotIndex = 1;
    }
    void Renderer2D::EndScene(){
        Flush();
    }
    float Renderer2D::PrepareTriangle(const Ref<Texture2D>& texture){
        if (s_Data.VertexCount + 3 > Renderer2DData::MaxVertices ||
            s_Data.IndexCount + 3 > Renderer2DData::MaxIndices)
        {
            FlushAndReset();
        }
        if(!texture) return 0.0f;
        for(uint32_t i=1; i<s_Data.TextureSlotIndex;++i){
            if(*s_Data.TextureSlots[i] == *texture) return static_cast<float>(i);
        }
        if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots) FlushAndReset();
        const uint32_t index = s_Data.TextureSlotIndex++;
        s_Data.TextureSlots[index] = texture;
        return static_cast<float>(index);
    }
    float Renderer2D::PrepareQuad(const Ref<Texture2D>& texture){
        if (s_Data.VertexCount + 4 > Renderer2DData::MaxVertices ||
            s_Data.IndexCount + 6 > Renderer2DData::MaxIndices)
        {
            FlushAndReset();
        }
        if(!texture) return 0.0f;
        for(uint32_t i=1; i<s_Data.TextureSlotIndex;++i){
            if(*s_Data.TextureSlots[i] == *texture) return static_cast<float>(i);
        }
        if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots) FlushAndReset();
        const uint32_t index = s_Data.TextureSlotIndex++;
        s_Data.TextureSlots[index] = texture;
        return static_cast<float>(index);
    }
    /* Permet de dessiner une texture avec des coordonnées de texture différent des 4 coins*/
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle,
        const Ref<Texture2D>& texture, const glm::vec2& uvMin, const glm::vec2& uvMax, const float textScale, const glm::vec4 tintColor){
        VerifyState(PossibleState::Texture);
        constexpr glm::vec4 color {1.0f,1.0f,1.0f,1.0f};
        const float textureIndex = PrepareQuad(texture);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f),position) * 
            glm::rotate(glm::mat4(1.0f),rotateAngle,{0.0f,0.0f,1.0f})* 
            glm::scale(glm::mat4(1.0f),{size.x,size.y,1.0f});
        Renderer2D::QuadInfo info{ color, textureIndex, tintColor, textScale, uvMin, uvMax};
        Renderer2D::RenderQuad(info, transform);
    }
     /* Permet de dessiner une texture avec des coordonnées de texture différent des 4 coins*/
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle,
        const Ref<SubTexture2D>& subTexture, const float textScale, const glm::vec4 tintColor){
        VerifyState(PossibleState::Texture);
        constexpr glm::vec4 color {1.0f,1.0f,1.0f,1.0f};
        const float textureIndex = PrepareQuad(subTexture->GetTexture());
        glm::mat4 transform = glm::translate(glm::mat4(1.0f),position) * 
            glm::rotate(glm::mat4(1.0f),rotateAngle,{0.0f,0.0f,1.0f})* 
            glm::scale(glm::mat4(1.0f),{size.x,size.y,1.0f});
        Renderer2D::QuadInfo info{color, textureIndex, tintColor, textScale, subTexture->GetUVMin(), subTexture->GetUVMax()};
        Renderer2D::RenderQuad(info, transform);
    }
    /*permet de dessiner un carré avec une texture, 
    la rotation est assez lourde, on pourrait envisager de faire un drawrotatedquad rather*/
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,  const float rotateAngle, const glm::vec4& color){
        VerifyState(PossibleState::Texture);
        const float textureIndex = PrepareQuad(nullptr);
        /* il faudrait faire un helper et modifier les rotations si on veut vraiment améliorer les performances*/
        glm::mat4 transform = glm::translate(glm::mat4(1.0f),position) * 
            glm::rotate(glm::mat4(1.0f),rotateAngle,{0.0f,0.0f,1.0f})* 
            glm::scale(glm::mat4(1.0f),{size.x,size.y,1.0f});
        Renderer2D::QuadInfo info{ color, textureIndex};
        Renderer2D::RenderQuad(info, transform);
    }
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,  const float rotateAngle,
            const Ref<Texture2D>& texture ,const  float textScale, const glm::vec4 tintColor){
                DrawQuad(position,size,rotateAngle,texture, {0.0f,0.0f},{1.0f,1.0f},textScale, tintColor);
    }
    /* permet de dessiner un carrée d'une certaine couleur*/
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,  const float rotateAngle, const Ref<Texture2D>& texture,const  float textScale, const glm::vec4 tintColor ){
        DrawQuad({position.x,position.y,0.0f},size,rotateAngle,texture,textScale, tintColor);
    } 
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color){
        DrawQuad(position, size, 0.0f, color);
    }
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotateAngle, const glm::vec4& color){
        DrawQuad({position.x,position.y,0.0f},size,rotateAngle,color);
    }
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color){
        DrawQuad({position.x,position.y,0.0f},size,0.0f,color);

    }
    
    void Renderer2D::DrawTriangle(const glm::vec3& positionA, const glm::vec3& positionB,const glm::vec3& positionC, 
        const Ref<Texture2D>& texture, const glm::vec2& uvA, const glm::vec2& uvB,  const glm::vec2& uvC, const float textScale, const glm::vec4 tintColor, const glm::vec2& scale, const float rotateAngle){
        VerifyState(PossibleState::Texture);
        const float textureIndex = PrepareTriangle(texture);
        /* il faudrait faire un helper et modifier les rotations si on veut vraiment améliorer les performances*/
        glm::mat4 transform = glm::rotate(glm::mat4(1.0f),rotateAngle,{0.0f,0.0f,1.0f})* 
            glm::scale(glm::mat4(1.0f),{scale.x,scale.y,1.0f});
        Renderer2D::TriangleInfo info = {positionA, positionB,positionC, glm::vec4{1.0f}, textureIndex, tintColor,textScale,uvA,uvB,uvC};
        Renderer2D::RenderTriangle(info, transform);
    }
    void Renderer2D::DrawTriangle(const glm::vec3& positionA, const glm::vec3& positionB,const glm::vec3& positionC,  const glm::vec4& color, const glm::vec2& scale,  const float rotateAngle){
        VerifyState(PossibleState::Texture);
        const float textureIndex = PrepareTriangle(nullptr);
        /* il faudrait faire un helper et modifier les rotations si on veut vraiment améliorer les performances*/
        glm::mat4 transform = glm::rotate(glm::mat4(1.0f),rotateAngle,{0.0f,0.0f,1.0f})* 
            glm::scale(glm::mat4(1.0f),{scale.x,scale.y,1.0f});
        Renderer2D::TriangleInfo info = {positionA, positionB,positionC, color, textureIndex};
        Renderer2D::RenderTriangle(info, transform);
    }
    void Renderer2D::RenderTriangle(const TriangleInfo& info, const glm::mat4& transform){
        const uint32_t base = s_Data.VertexCount;
        s_Data.VertexBufferPtr->Position =transform* glm::vec4{info.a,1.0f};
        s_Data.VertexBufferPtr->Color = info.color;
        s_Data.VertexBufferPtr->TexCoord = info.uvA;
        s_Data.VertexBufferPtr->TexIndex = info.textureIndex;
        s_Data.VertexBufferPtr->TintColor = info.tintColor;
        s_Data.VertexBufferPtr->Scale = info.scale;
        s_Data.VertexBufferPtr++;

        s_Data.VertexBufferPtr->Position = transform * glm::vec4{info.b,1.0f};
        s_Data.VertexBufferPtr->Color = info.color;
        s_Data.VertexBufferPtr->TexCoord = info.uvB;
        s_Data.VertexBufferPtr->TexIndex = info.textureIndex;
        s_Data.VertexBufferPtr->TintColor = info.tintColor;
        s_Data.VertexBufferPtr->Scale = info.scale;
        s_Data.VertexBufferPtr++;

        s_Data.VertexBufferPtr->Position = transform * glm::vec4{info.c,1.0f};
        s_Data.VertexBufferPtr->Color = info.color;
        s_Data.VertexBufferPtr->TexCoord = info.uvC;
        s_Data.VertexBufferPtr->TexIndex = info.textureIndex;
        s_Data.VertexBufferPtr->TintColor = info.tintColor;
        s_Data.VertexBufferPtr->Scale = info.scale;
        s_Data.VertexBufferPtr++;
        s_Data.IndexBufferBase[s_Data.IndexCount++] = base;
        s_Data.IndexBufferBase[s_Data.IndexCount++] = base + 1;
        s_Data.IndexBufferBase[s_Data.IndexCount++] = base + 2;

        s_Data.VertexCount += 3;

        ++ s_Data.Stats.TriangleCount;
    }
    void Renderer2D::RenderQuad(const QuadInfo &info, const glm::mat4 &transform){
        const uint32_t base = s_Data.VertexCount;
        s_Data.VertexBufferPtr->Position =transform* s_Data.VertexPositions[0];
        s_Data.VertexBufferPtr->Color = info.color;
        s_Data.VertexBufferPtr->TexCoord = {info.uvMin.x, info.uvMin.y};
        s_Data.VertexBufferPtr->TexIndex = info.textureIndex;
        s_Data.VertexBufferPtr->TintColor = info.tintColor;
        s_Data.VertexBufferPtr->Scale = info.scale;
        s_Data.VertexBufferPtr++;

        s_Data.VertexBufferPtr->Position = transform * s_Data.VertexPositions[1];
        s_Data.VertexBufferPtr->Color = info.color;
        s_Data.VertexBufferPtr->TexCoord = {info.uvMax.x, info.uvMin.y};
        s_Data.VertexBufferPtr->TexIndex = info.textureIndex;
        s_Data.VertexBufferPtr->TintColor = info.tintColor;
        s_Data.VertexBufferPtr->Scale = info.scale;
        s_Data.VertexBufferPtr++;

        s_Data.VertexBufferPtr->Position = transform * s_Data.VertexPositions[2];
        s_Data.VertexBufferPtr->Color = info.color;
        s_Data.VertexBufferPtr->TexCoord = {info.uvMax.x, info.uvMax.y};
        s_Data.VertexBufferPtr->TexIndex = info.textureIndex;
        s_Data.VertexBufferPtr->TintColor = info.tintColor;
        s_Data.VertexBufferPtr->Scale = info.scale;
        s_Data.VertexBufferPtr++;

        s_Data.VertexBufferPtr->Position = transform * s_Data.VertexPositions[3];
        s_Data.VertexBufferPtr->Color = info.color;
        s_Data.VertexBufferPtr->TexCoord = {info.uvMin.x, info.uvMax.y};
        s_Data.VertexBufferPtr->TexIndex = info.textureIndex;
        s_Data.VertexBufferPtr->TintColor = info.tintColor;
        s_Data.VertexBufferPtr->Scale = info.scale;
        s_Data.VertexBufferPtr++;
        constexpr uint32_t localIndices[] = {0, 1, 2, 2, 3, 0};
        for (uint32_t index : localIndices)
            s_Data.IndexBufferBase[s_Data.IndexCount++] = base + index;

        s_Data.VertexCount += 4;

        ++ s_Data.Stats.QuadCount;
    }
    void Renderer2D::DrawArrow(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color,const float width, const float widthRatio, float overHangRatio){
        const glm::vec3 dir = end-start;
        const float length = glm::length(glm::vec2(dir.x, dir.y));
        float widthUsed = width != 0.0f ? width : widthRatio * length;
        if(length <= 0.00001f || widthUsed <= 0.0f) return;
        const float angle = std::atan2(dir.y, dir.x);
        const glm::vec3 normal{-dir.y / length, dir.x / length, 0.0f};
        const float headHalfWidth = widthUsed * (0.5f + std::max(0.0f, overHangRatio));
        const float headLength = std::min(2.0f * widthUsed, length);
        const float shaftLength = length - headLength;
        const glm::vec3 headBase = end - dir * (headLength / length);
        const glm::vec3 shaftCenter = (start + headBase) * 0.5f;
        if(shaftLength > 0.00001f)
            DrawQuad(shaftCenter, {shaftLength, widthUsed},angle,color);
        DrawTriangle(headBase - normal * headHalfWidth,end,headBase + normal * headHalfWidth,color);
    }
    void Renderer2D::ResetStats(){
        s_Data.Stats = {};
    }
    Renderer2D::Statistics Renderer2D::GetStats(){
        return s_Data.Stats;
    }

    void Renderer2D::DrawGlyphSDF(const glm::vec3 &position, const glm::vec2 &size, float rotateAngle, 
        const Ref<Texture2D>& texture, const glm::vec2 &uvMin, const glm::vec2 &uvMax, float textScale, glm::vec4 tintColor ){
        VerifyState(PossibleState::Text);
        constexpr glm::vec4 color {1.0f,1.0f,1.0f,1.0f};
        const float textureIndex = PrepareQuad(texture);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f),position) * 
            glm::rotate(glm::mat4(1.0f),rotateAngle,{0.0f,0.0f,1.0f})* 
            glm::scale(glm::mat4(1.0f),{size.x,size.y,1.0f});
        Renderer2D::QuadInfo info{ color, textureIndex, tintColor, textScale, uvMin, uvMax};
        Renderer2D::RenderQuad(info, transform);
    }
    void Renderer2D::Write(std::u32string_view text, Font& font, const glm::vec3& position,const float scale, const glm::vec4& color){
        glm::vec2 pen{position.x, position.y};
        // ici passage à la ligne  mettre un helper et une condition
        VerifyState(PossibleState::Text);
        for (char32_t codepoint : text) {
            if (codepoint == U'\n') {
                pen.x = position.x;
                pen.y -= font.GetLineHeight() * scale;
                continue;
            }
            const Glyph& glyph = font.GetGlyph(codepoint);
            const glm::vec2 size = glm::vec2(glyph.Size) * scale;
            const glm::vec2 bottomLeft {
                pen.x + glyph.Bearing.x*scale,
                pen.y - (glyph.Size.y - glyph.Bearing.y) * scale
            };

            if(glyph.Size.x > 0 && glyph.Size.y > 0){
                const glm::vec2 center = bottomLeft + size/2.0f;
                DrawGlyphSDF(glm::vec3(center, position.z),size, 0.0f,font.GetAtlasTexture(),glyph.UVMin, glyph.UVMax,1.0f, color);
            }
            pen.x += (glyph.Advance / 64.0f) * scale;
        }
    }

}
