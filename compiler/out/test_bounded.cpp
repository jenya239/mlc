#define main mlc_user_main
#include "test_bounded.hpp"

namespace test_bounded {

void i32_to_string(i32 self, void ) noexcept{
return /* unit */;
}
x i32_print_display(i32 self, Display T) noexcept{
return T{};
}
int i32_main(i32 self) noexcept{
print_display(42);
return 0;
}
static_assert(Display<int>, "i32 does not implement Display");

} // namespace test_bounded
