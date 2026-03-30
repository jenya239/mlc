#include "literals.hpp"

#include "context.hpp"

namespace literals {

using namespace context;

mlc::String gen_integer_literal(int integer_value) noexcept;

mlc::String gen_string_literal(mlc::String string_value) noexcept;

mlc::String gen_boolean_literal(bool boolean_value) noexcept;

mlc::String gen_unit_literal() noexcept;

mlc::String gen_extern_placeholder() noexcept;

mlc::String gen_integer_literal(int integer_value) noexcept{return mlc::to_string(integer_value);}

mlc::String gen_string_literal(mlc::String string_value) noexcept{return mlc::String("mlc::String(\"") + context::escape_str(string_value) + mlc::String("\", ") + mlc::to_string(string_value.length()) + mlc::String(")");}

mlc::String gen_boolean_literal(bool boolean_value) noexcept{return boolean_value ? mlc::String("true") : mlc::String("false");}

mlc::String gen_unit_literal() noexcept{return mlc::String("/* unit */");}

mlc::String gen_extern_placeholder() noexcept{return mlc::String("");}

} // namespace literals
