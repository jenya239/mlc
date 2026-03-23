#define main mlc_user_main
#include "test_trait_e2e.hpp"

namespace test_trait_e2e {

mlc::String i32_to_string(int self) noexcept { return mlc::to_string(self); }
inline mlc::String to_string(int self) noexcept { return i32_to_string(self); }
static_assert(Display<int>, "i32 does not implement Display");
template<typename T>
requires Display<T>
T id(T x) noexcept{
return x;
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
return id(42);
}

} // namespace test_trait_e2e
