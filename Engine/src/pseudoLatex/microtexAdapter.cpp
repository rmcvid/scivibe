#include "pseudoLatex/microtexAdapter.hpp"
#include "pseudoLatex/font.hpp"
#include "renderer/renderer2D.hpp"

#include <latex.h>
#include <memory>
#include <stdexcept>
#include <string>
#include "core/log.hpp"
#include "core/core.h"

namespace scivibe{
    namespace {
        uint32_t s_RasterSize = 48;
    }
    [[noreturn]] void Unsupported(const char* operation){
        throw std::runtime_error(std::string("Adaptateur Microtex : ") + operation + "non implemente");
    }

    void ConfigureMicroTeXAdapter(uint32_t rasterSize){
        s_RasterSize = rasterSize;
    }

    class MicroTeXFont final : public tex::Font{
        public:
            MicroTeXFont(const std::string& path, float logicalSize) :
                m_Path(path),
                m_LogicalSize(logicalSize),
                m_RasterSize(s_RasterSize)
            {}

            float getSize() const override{return m_LogicalSize;}

            tex::sptr<tex::Font> deriveFont(int style) const override{
                if(style != tex::PLAIN){
                    Unsupported("deriveFont avec changement de style");
                }
                return CreateRef<MicroTeXFont>(m_Path, m_LogicalSize);
            }

            bool operator==(const tex::Font& other) const override{
                const auto* font = dynamic_cast<const MicroTeXFont*>(&other);
                return font && m_Path == font->m_Path && m_LogicalSize == font->m_LogicalSize && m_RasterSize == font->m_RasterSize;
            }
            
            bool operator!=(const tex::Font& other) const override{
                return !(*this == other);
            }
            const scivibe::Font& GetRasterFont() const {
                if (!m_RasterFont) m_RasterFont = CreateRef<scivibe::Font>(m_Path,m_RasterSize);
                return *m_RasterFont;
            }
            float GetPixelToLogicalScale() const {
                return m_LogicalSize / static_cast<float>(m_RasterSize);
            }
        private:
            std::string m_Path;
            float m_LogicalSize;
            uint32_t m_RasterSize;
            mutable Ref<scivibe::Font> m_RasterFont;
    };

    class MicroTeXGraphics final : public tex::Graphics2D {
        public:
            MicroTeXGraphics(const glm::vec3& position, float unitsPerPixel) :
                m_Position(position),  m_UnitsPerPixel(unitsPerPixel)
            {}

            void setColor(tex::color color) override{ m_Color = color; }
            tex::color getColor() const override { return m_Color; }
            
            void setStroke(const tex::Stroke& stroke) override { m_Stroke = stroke;}
            const tex::Stroke& getStroke() const override { return m_Stroke;}
            void setStrokeWidth(float width) override { m_Stroke.lineWidth = width; }
            
            void setFont(const tex::Font* font) override {m_Font = font;} 
            const tex::Font* getFont() const override {return m_Font;}

            void translate(float x, float y) override {  m_Translation += m_Scale * glm::vec2(x, y); }
            void scale(float x, float y) override { 
                if (x <= 0.0f || y <= 0.0f) Unsupported("echelle nulle ou negative");
                m_Scale *= glm::vec2(x, y);
            }

            float sx() const override { return m_Scale.x; }
            float sy() const override { return m_Scale.y; }
            void reset() override {
                m_Translation = glm::vec2(0.0f);
                m_Scale = glm::vec2(1.0f);
            }

            void drawChar(wchar_t character, float x, float y) override {
                const auto* font = dynamic_cast<const MicroTeXFont*>(m_Font);

                if (!font) throw std::logic_error( "Aucune police MicroTeX compatible selectionnee" );
                const auto& rasterFont = font->GetRasterFont();
                const Glyph& glyph = rasterFont.GetGlyph(static_cast<char32_t>(character));
                
                if (glyph.Size.x == 0 || glyph.Size.y == 0) return;
                const float ratio = font->GetPixelToLogicalScale();
                const glm::vec2 localCenter{
                    x + (glyph.Bearing.x + glyph.Size.x * 0.5f) * ratio,
                    y + (-glyph.Bearing.y + glyph.Size.y * 0.5f) * ratio
                };

                const glm::vec2 transformedCenter =m_Translation + m_Scale * localCenter;

                const glm::vec3 center{
                    m_Position.x + transformedCenter.x * m_UnitsPerPixel,
                    m_Position.y - transformedCenter.y * m_UnitsPerPixel,
                    m_Position.z
                };
                const glm::vec2 size = glm::vec2(glyph.Size) * ratio * m_Scale * m_UnitsPerPixel;
                const glm::vec4 color{
                    tex::color_r(m_Color) / 255.0f,
                    tex::color_g(m_Color) / 255.0f,
                    tex::color_b(m_Color) / 255.0f,
                    tex::color_a(m_Color) / 255.0f
                };

                Renderer2D::DrawQuad(center,size,0.0f,rasterFont.GetAtlasTexture(),glyph.UVMin,glyph.UVMax,1.0f,color);
            }

            void rotate(float) override {
                Unsupported("rotation");
            }

            void rotate(float, float, float) override {
                Unsupported("rotation");
            }
            // ici on peut le définir non ?
            void drawText(const std::wstring&, float, float) override {
                Unsupported("drawText");
            }

            void drawLine(float, float, float, float) override {
                Unsupported("drawLine");
            }

            void drawRect(float, float, float, float) override {
                Unsupported("drawRect");
            }

            void fillRect(float, float, float, float) override {
                Unsupported("fillRect");
            }

            void drawRoundRect(float, float, float, float, float, float) override {
                Unsupported("drawRoundRect");
            }

            void fillRoundRect(float, float, float, float, float, float) override {
                Unsupported("fillRoundRect");
            }

        private:
            glm::vec3 m_Position;
            float m_UnitsPerPixel;
            glm::vec2 m_Translation{0.0f};
            glm::vec2 m_Scale{1.0f};
            tex::color m_Color = tex::white;
            tex::Stroke m_Stroke;
            const tex::Font* m_Font = nullptr;
    };

    void DrawMicroTeX(tex::TeXRender& formula,const glm::vec3& position,float unitsPerPixel) {
        MicroTeXGraphics graphics(position, unitsPerPixel);
        formula.draw(graphics, 0, 0);
    }
}

namespace tex {

    Font* Font::create(const std::string& file, float size) {
        return new scivibe::MicroTeXFont(file, size);
    }

    sptr<Font> Font::_create( const std::string& name, int style, float size) {
        if (name != "Serif" || style != PLAIN) {
            scivibe::Unsupported("famille ou style de police de texte" );
        }

        const std::string file = LaTeX::getResRootPath() + "/fonts/latin/cmr10.ttf";
        return std::make_shared<scivibe::MicroTeXFont>(file,size);
    }

    sptr<TextLayout> TextLayout::create(const std::wstring&,const sptr<Font>&) {
        scivibe::Unsupported("TextLayout, notamment pour \\text{...}" );
    }

}
