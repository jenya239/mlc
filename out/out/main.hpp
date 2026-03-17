#ifndef MAIN_HPP
#define MAIN_HPP

#include "mlc.hpp"
#include <variant>


namespace mlc_main {

struct Circle;
struct Rect;
using Shape = std::variant<Circle, Rect>;
struct Circle {int radius;};
struct Rect {int width;int height;};
std::shared_ptr<Shape> make_circle(int r) noexcept;
std::shared_ptr<Shape> make_rect(int w, int h) noexcept;
int area(std::shared_ptr<Shape> s) noexcept;
int main(int argc, char** argv) noexcept;

} // namespace mlc_main

#endif // MAIN_HPP
