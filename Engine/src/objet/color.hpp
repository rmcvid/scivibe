#ifndef COLOR_HPP
#define COLOR_HPP
namespace scivibe {
    class Color
    {
    private:
        float r;
        float g;
        float b;
        float a;

    public:
        Color(float r, float g, float b, float a = 1.0f)
            : r(r), g(g), b(b), a(a)
        {
        }
    };

}
#endif
