#define main mlc_user_main
#include "main.hpp"

namespace mlc_main {

std::shared_ptr<Shape> make_circle(int r) noexcept{
return std::make_shared<Shape>(Circle{r});
}
std::shared_ptr<Shape> make_rect(int w, int h) noexcept{
return std::make_shared<Shape>(Rect{w, h});
}
int area(std::shared_ptr<Shape> s) noexcept{
return std::visit(overloaded{[&](const Circle& circle) { const auto& radius = circle.radius; return (radius * radius); },
[&](const Rect& rect) { const auto& width = rect.width; const auto& height = rect.height; return (width * height); }
}, (*s));
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
auto c = make_circle(5);
auto r = make_rect(3, 4);
mlc::io::println(area(c));
mlc::io::println(area(r));
return 0;
}

} // namespace mlc_main

#undef main

static void mlc_cli_set_args(int argc, char** argv) {
  std::vector<mlc::String> arguments;
  arguments.reserve(argc > 0 ? argc - 1 : 0);
  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::mlc_main::mlc_user_main(argc, argv);
}
