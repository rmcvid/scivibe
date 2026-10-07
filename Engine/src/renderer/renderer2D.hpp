#pragma once

#include "renderer/camera.hpp"
#include "renderer/texture.hpp"
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
            static void Flush();


            // primitive 
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle, const Ref<Texture2D>& texture,  const float textScale = 1.00f, const glm::vec4 tincColor = glm::vec4(1.0f));
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotateAngle, const Ref<Texture2D>& texture,  const float textScale = 1.00f, const glm::vec4 tincColor = glm::vec4(1.0f));
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotateAngle, const glm::vec4& color);
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotateAngle, const glm::vec4& color);
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);



            // stats
            struct Statistics{
                uint32_t DrawCalls = 0;
                uint32_t QuadCount = 0;
                uint32_t GetTotalVertexCount() { return QuadCount * 4; }
                uint32_t GetTotalIndexCount() { return QuadCount * 6; }
            };
            static void ResetStats();
            static Statistics GetStats();
        private:
            static void FlushAndReset();
            struct QuadInfo {
                glm::vec4 color{1.0f};
                float textureIndex = 0.0f;
                glm::vec4 tintColor{1.0f};
                float scale = 1.0f;
            };
            static void RenderQuad(const QuadInfo& info,const glm::mat4& transform);
        };
}