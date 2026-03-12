#include "test_simple.hpp"

namespace test_simple {

int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
return 42;
}

} // namespace test_simple
