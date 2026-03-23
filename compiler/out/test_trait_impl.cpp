#define main mlc_user_main
#include "test_trait_impl.hpp"

namespace test_trait_impl {

void i32_to_string(i32 self, void ) noexcept{
return /* unit */;
}
static_assert(Display<int>, "i32 does not implement Display");
void main(void ) noexcept{
return /* unit */;
}

} // namespace test_trait_impl
