#define main mlc_user_main
#include "test_bounds_only.hpp"

namespace test_bounds_only {

template<typename T>
requires Display<T>
T id(T x) noexcept{
return x;
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
return 0;
}

} // namespace test_bounds_only
