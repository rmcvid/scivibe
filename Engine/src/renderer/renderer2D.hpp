#pragma once

#include "core/core.hpp"
#include "renderer/camera.hpp"
#include "renderer/texture.hpp"
#include "pseudoLatex/font.hpp"
#include <cstdint>
#include <string_view>
namespace scivibe{
    class SCIVIBE_API Renderer2D
    {
        Renderer2D();
        ~Renderer2D();

        public: 
            static void Init();
            static void Shutdown();
            static void BeginScene(const OrthographicCamera& camera);
            static void EndScene();
            // Upload and draw pending geometry, then reset the batch.
            static void Flush();
            // primitive 
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle, const Ref<Texture2D>& texture, const glm::vec2& uvMin, const glm::vec2& uvMax, const float textScale = 1.00f, const glm::vec4 tintColor = glm::vec4(1.0f));
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle, const Ref<SubTexture2D>& subTexture, const float textScale = 1.00f, const glm::vec4 tincColor = glm::vec4(1.0f));
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle, const Ref<Texture2D>& texture, const float textScale = 1.00f, const glm::vec4 tincColor = glm::vec4(1.0f));
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotateAngle, const Ref<Texture2D>& texture, const float textScale = 1.00f, const glm::vec4 tincColor = glm::vec4(1.0f));
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle, const glm::vec4& color);
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotateAngle, const glm::vec4& color);
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
            //
            static void DrawTriangle(const glm::vec3& positionA,const glm::vec3& positionB,const glm::vec3& positionC, const glm::vec4& color, const glm::vec2& scale = {1.0f,1.0f},  const float rotateAngle = 0.0f);
            static void DrawTriangle(const glm::vec3& positionA, const glm::vec3& positionB,const glm::vec3& positionC, const Ref<Texture2D>& texture, const glm::vec2& uvA = {0.0f,0.0f}, const glm::vec2& uvB = {1.0f,0.0f},  const glm::vec2& uvC = {0.5f,1.0f}, const float textScale = 1.0f, const glm::vec4 tintColor = glm::vec4 {1.0f}, 
                        const glm::vec2& scale = {1.0f,1.0f}, const float rotateAngle = 0.0f);
            
            static void DrawArrow(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width = 0.0f, const float widthRatio = 0.2 , const float overHangRatio = 4.0f/20.0f);
            static void DrawGlyphSDF(const glm::vec3 &position, const glm::vec2 &size, float rotateAngle, const Ref<Texture2D>& texture, const glm::vec2 &uvMin, const glm::vec2 &uvMax, float textScale = 1.0f, glm::vec4 tintColor = glm::vec4(1.0f));
            static void Write(std::u32string_view text,Font& font,const glm::vec3& position, const float scale = 1.0f,const glm::vec4& color = glm::vec4(1.0f));

            // stats
            struct Statistics{
                uint32_t DrawCalls = 0;
                uint32_t QuadCount = 0;
                uint32_t TriangleCount = 0;
                uint32_t GetTotalVertexCount() { return QuadCount * 4 + TriangleCount * 3 ; }
                uint32_t GetTotalIndexCount() { return QuadCount * 6 + TriangleCount * 3 ; }
            };
            static void ResetStats();
            static Statistics GetStats();
            enum class PossibleState { // reprend le shader actif
                Texture,
                Text
            };
        private:
            static void FlushAndReset();
            static float PrepareQuad(const Ref<Texture2D>& texture);
            static float PrepareTriangle(const Ref<Texture2D>& texture);
            static void ResetData();
            
            static void VerifyState(PossibleState state);
            static void SwitchState(PossibleState state);
            struct QuadInfo {
                glm::vec4 color{1.0f};
                float textureIndex = 0.0f;
                glm::vec4 tintColor{1.0f};
                float scale = 1.0f;
                glm::vec2 uvMin{0.0f, 0.0f};
                glm::vec2 uvMax{1.0f, 1.0f};
            };
            struct TriangleInfo{
                glm::vec3 a{0.0f, 0.0f,0.0f};
                glm::vec3 b{1.0f, 0.0f,0.0f};
                glm::vec3 c{0.5f,0.5f,0.0f};
                glm::vec4 color{1.0f};
                float textureIndex = 0.0f;
                glm::vec4 tintColor{1.0f};
                float scale = 1.0f;
                glm::vec2 uvA{0.0f,0.0f};
                glm::vec2 uvB{1.0f,0.0f};
                glm::vec2 uvC{0.5f,0.5f};
            };
            
            
            static void RenderQuad(const QuadInfo& info,const glm::mat4& transform);
            static void RenderTriangle( const TriangleInfo& info, const glm::mat4& transform);
        };
}
