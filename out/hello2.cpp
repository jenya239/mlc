#define main mlc_user_main
#include "hello2.hpp"

namespace hello2 {

int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::io::println(mlc::String("hello from mlcc"));
return 0;
}

} // namespace hello2
