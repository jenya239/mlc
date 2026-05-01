#define main mlc_user_main
#include "literals.hpp"

namespace literals {

mlc::String gen_integer_literal(int integer_value) noexcept{
return mlc::to_string(integer_value);
}
mlc::String gen_string_literal(mlc::String string_value) noexcept{
return ((((mlc::String("mlc::String(\"", 13) + cpp_naming::escape_str(string_value)) + mlc::String("\", ", 3)) + mlc::to_string(string_value.length())) + mlc::String(")", 1));
}
mlc::String gen_boolean_literal(bool boolean_value) noexcept{
if (boolean_value) {
return mlc::String("true", 4);
} else {
return mlc::String("false", 5);
}
}
mlc::String gen_unit_literal() noexcept{
return mlc::String("/* unit */", 10);
}
mlc::String gen_extern_placeholder() noexcept{
return mlc::String("", 0);
}
mlc::String gen_float_literal(mlc::String v) noexcept{
return v;
}
mlc::String gen_i64_literal(mlc::String v) noexcept{
return v;
}
mlc::String gen_u8_literal(mlc::String v) noexcept{
return ((mlc::String("static_cast<uint8_t>(", 21) + v) + mlc::String(")", 1));
}
mlc::String gen_usize_literal(mlc::String v) noexcept{
return ((mlc::String("static_cast<size_t>(", 20) + v) + mlc::String(")", 1));
}
mlc::String gen_char_literal(mlc::String v) noexcept{
return ((mlc::String("static_cast<char32_t>(", 22) + v) + mlc::String(")", 1));
}

} // namespace literals
