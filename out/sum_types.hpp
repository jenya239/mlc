#ifndef SUM_TYPES_HPP
#define SUM_TYPES_HPP

#include "mlc.hpp"
#include <variant>


namespace sum_types {

struct Circle;
struct Rect;
using Shape = std::variant<Circle, Rect>;
struct Circle {int field0;};
struct Rect {int field0;int field1;};
int area(Shape s) noexcept;
mlc::String describe(Shape s) noexcept;
int main(int argc, char** argv) noexcept;

} // namespace sum_types

#endif // SUM_TYPES_HPP
