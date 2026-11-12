#include "math/functionCurve.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

namespace scivibe::Math {
    namespace {
        class Parser {
        public:
            explicit Parser(std::string_view source) : m_Source(source) {}
            ScalarFunction Parse() {
                if (m_Source.size() > 1024) Fail("Expression trop longue");
                auto function = Sum();
                SkipSpaces();
                if (m_Position != m_Source.size()) Fail("Symbole inattendu (utiliser * pour multiplier)");
                return function;
            }
        private:
            std::string m_Source;
            size_t m_Position = 0;
            int m_Depth = 0;
            [[noreturn]] void Fail(const char* message) const {
                throw std::invalid_argument(std::string(message) + " (position " + std::to_string(m_Position + 1) + ")");
            }
            void SkipSpaces() {
                while (m_Position < m_Source.size() && std::isspace(static_cast<unsigned char>(m_Source[m_Position]))) ++m_Position;
            }
            bool Take(char token) {
                SkipSpaces();
                if (m_Position == m_Source.size() || m_Source[m_Position] != token) return false;
                ++m_Position;
                return true;
            }
            ScalarFunction Sum() {
                auto left = Product();
                while (true) {
                    if (Take('+')) { auto right = Product(); left = [left, right](double x) { return left(x) + right(x); }; }
                    else if (Take('-')) { auto right = Product(); left = [left, right](double x) { return left(x) - right(x); }; }
                    else return left;
                }
            }
            ScalarFunction Product() {
                auto left = Unary();
                while (true) {
                    if (Take('*')) { auto right = Unary(); left = [left, right](double x) { return left(x) * right(x); }; }
                    else if (Take('/')) { auto right = Unary(); left = [left, right](double x) { return left(x) / right(x); }; }
                    else return left;
                }
            }
            ScalarFunction Unary() {
                if (++m_Depth > 64) Fail("Expression trop imbriquee");
                ScalarFunction result;
                if (Take('+')) result = Unary();
                else if (Take('-')) { auto value = Unary(); result = [value](double x) { return -value(x); }; }
                else result = Power();
                --m_Depth;
                return result;
            }
            ScalarFunction Power() {
                auto base = Primary();
                if (Take('^')) {
                    auto exponent = Unary(); // Right associative; -x^2 means -(x^2).
                    return [base, exponent](double x) { return std::pow(base(x), exponent(x)); };
                }
                return base;
            }
            ScalarFunction Primary() {
                if (Take('(')) {
                    auto value = Sum();
                    if (!Take(')')) Fail("Parenthese fermante manquante");
                    return value;
                }
                SkipSpaces();
                if (m_Position == m_Source.size()) Fail("Valeur attendue");
                const unsigned char c = static_cast<unsigned char>(m_Source[m_Position]);
                if (std::isdigit(c) || c == '.') {
                    const char* start = m_Source.c_str() + m_Position;
                    char* end = nullptr;
                    const double value = std::strtod(start, &end);
                    if (end == start || !std::isfinite(value)) Fail("Nombre invalide");
                    m_Position += static_cast<size_t>(end - start);
                    return [value](double) { return value; };
                }
                const size_t begin = m_Position;
                while (m_Position < m_Source.size() && std::isalnum(static_cast<unsigned char>(m_Source[m_Position]))) ++m_Position;
                const std::string name = m_Source.substr(begin, m_Position - begin);
                if (name == "x") return [](double x) { return x; };
                if (name == "pi") return [](double) { return 3.14159265358979323846; };
                if (name == "e") return [](double) { return 2.71828182845904523536; };
                using UnaryMath = double (*)(double);
                UnaryMath operation = nullptr;
                if (name == "sin") operation = static_cast<UnaryMath>(std::sin);
                else if (name == "cos") operation = static_cast<UnaryMath>(std::cos);
                else if (name == "tan") operation = static_cast<UnaryMath>(std::tan);
                else if (name == "sqrt") operation = static_cast<UnaryMath>(std::sqrt);
                else if (name == "abs") operation = static_cast<UnaryMath>(std::fabs);
                else if (name == "exp") operation = static_cast<UnaryMath>(std::exp);
                else if (name == "log" || name == "ln") operation = static_cast<UnaryMath>(std::log);
                else if (name == "log10") operation = static_cast<UnaryMath>(std::log10);
                else if (name == "asin") operation = static_cast<UnaryMath>(std::asin);
                else if (name == "acos") operation = static_cast<UnaryMath>(std::acos);
                else if (name == "atan") operation = static_cast<UnaryMath>(std::atan);
                if (!operation) Fail("Nom ou symbole inconnu");
                if (!Take('(')) Fail("Parenthese attendue apres la fonction");
                auto argument = Sum();
                if (!Take(')')) Fail("Parenthese fermante manquante");
                return [operation, argument](double x) { return operation(argument(x)); };
            }
        };
    }

    ScalarFunction CompileFunction(std::string_view expression, std::string& error) {
        error.clear();
        try { return Parser(expression).Parse(); }
        catch (const std::invalid_argument& exception) { error = exception.what(); return {}; }
    }

    CurvePath SampleFunction(const ScalarFunction& function, double minX, double maxX, int segments) {
        CurvePath path;
        if (!function || !std::isfinite(minX) || !std::isfinite(maxX) || minX >= maxX || segments < 2 || segments > 16384)
            return path;
        CurveContour contour;
        auto finish = [&]() {
            if (contour.Points.size() >= 2) path.Contours.push_back(std::move(contour));
            contour = CurveContour{};
        };
        // Probe and refine between samples. Non-finite values and intervals that
        // remain unresolved are separated, rather than drawing across an asymptote.
        auto appendSegment = [&](auto&& self, const glm::dvec2& a, const glm::dvec2& b, int depth) -> void {
            const double x = a.x * 0.5 + b.x * 0.5;
            const glm::dvec2 middle{x, function(x)};
            const bool finite = std::isfinite(a.y) && std::isfinite(b.y) && std::isfinite(middle.y);
            if (!finite || std::abs(middle.y - (a.y * 0.5 + b.y * 0.5)) >
                0.01 * (1.0 + std::min(std::abs(a.y), std::abs(b.y)))) {
                if (depth < 6 && x > a.x && x < b.x &&
                    (std::isfinite(a.y) || std::isfinite(b.y) || std::isfinite(middle.y))) {
                    self(self, a, middle, depth + 1);
                    self(self, middle, b, depth + 1);
                } else finish();
                return;
            }
            if (contour.Points.empty()) contour.Points.push_back(a);
            contour.Points.push_back(b);
        };
        glm::dvec2 previous{minX, function(minX)};
        for (int i = 1; i <= segments; ++i) {
            const double t = static_cast<double>(i) / segments;
            const double x = minX * (1.0 - t) + maxX * t;
            const glm::dvec2 next{x, function(x)};
            appendSegment(appendSegment, previous, next, 0);
            previous = next;
        }
        finish();
        return path;
    }
}
