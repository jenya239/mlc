#define main mlc_user_main
#include "sum_types.hpp"

namespace sum_types {

int area(Shape s) noexcept{
return std::visit(overloaded{[&](const Circle& circle) { auto [r] = circle; return ((r * r) * 3); },
[&](const Rect& rect) { auto [w, h] = rect; return (w * h); }
}, s);
}
mlc::String describe(Shape s) noexcept{
return std::visit(overloaded{[&](const Circle& circle) { auto [__0] = circle; return mlc::String("circle", 6); },
[&](const Rect& rect) { auto [__0, __1] = rect; return mlc::String("rect", 4); }
}, s);
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
auto c = Circle{5};
auto r = Rect{3, 4};
mlc::io::println(((describe(c) + mlc::String(": ", 2)) + mlc::to_string(area(c))));
mlc::io::println(((describe(r) + mlc::String(": ", 2)) + mlc::to_string(area(r))));
return 0;
}

} // namespace sum_types

#undef main

static void mlc_cli_set_args(int argc, char** argv) {
  std::vector<mlc::String> arguments;
  arguments.reserve(argc > 0 ? argc - 1 : 0);
  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::sum_types::mlc_user_main(argc, argv);
}
