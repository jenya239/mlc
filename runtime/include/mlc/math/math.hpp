#ifndef MLC_MATH_HPP
#define MLC_MATH_HPP

#include <cmath>
#include <cstdint>
#include <limits>
#include <algorithm>

namespace mlc::math {

// Basic arithmetic functions
inline int32_t abs(int32_t x) {
    return std::abs(x);
}

inline float abs_f(float x) {
    return std::fabs(x);
}

inline int32_t min(int32_t a, int32_t b) {
    return std::min(a, b);
}

inline int32_t max(int32_t a, int32_t b) {
    return std::max(a, b);
}

inline float min_f(float a, float b) {
    return std::fmin(a, b);
}

inline float max_f(float a, float b) {
    return std::fmax(a, b);
}

// Power and root functions
inline float pow_f(float base, float exp) {
    return std::pow(base, exp);
}

inline float sqrt_f(float x) {
    return std::sqrt(x);
}

// Cast/helpers for MLC numeric ports (f32 ops emit broken f32_mul; use f64).
inline double i32_to_f64(int32_t value) {
    return static_cast<double>(value);
}

inline double f64_infinity() {
    return std::numeric_limits<double>::infinity();
}

inline int32_t f64_to_i32(double value) {
    return static_cast<int32_t>(value);
}

// Trigonometric functions
inline float sin_f(float x) {
    return std::sin(x);
}

inline float cos_f(float x) {
    return std::cos(x);
}

inline float tan_f(float x) {
    return std::tan(x);
}

// Mathematical constants
constexpr float PI = 3.14159265359f;
constexpr float E = 2.71828182846f;

} // namespace mlc::math

#endif // MLC_MATH_HPP
