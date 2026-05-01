#define main mlc_user_main
#include "codegen_test_helpers.hpp"

namespace codegen_test_helpers {

context::CodegenContext empty_codegen_context() noexcept{
return context::create_codegen_context(ast_builders::empty_program());
}
test_runner::TestResult assert_expr_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<semantic_ir::SExpr> expr, mlc::String expected_code) noexcept{
return test_runner::assert_eq_str(test_name, eval::gen_expr(expr, context), expected_code);
}
test_runner::TestResult assert_type_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr, mlc::String expected_code) noexcept{
return test_runner::assert_eq_str(test_name, type_gen::type_to_cpp(context, type_expr), expected_code);
}
test_runner::TestResult assert_code_contains(mlc::String test_name, mlc::String actual_code, mlc::String expected_substring) noexcept{
return test_runner::assert_true((((test_name + mlc::String(" contains '", 11)) + expected_substring) + mlc::String("'", 1)), string_contains(actual_code, expected_substring));
}
bool chars_match_at(mlc::String haystack, int start_position, mlc::String needle, int needle_length) noexcept{
auto character_index = 0;
auto all_match = true;
while ((character_index < needle_length)) {
if ((haystack.char_at((start_position + character_index)) != needle.char_at(character_index))) {
all_match = false;
character_index = needle_length;
} else {
character_index = (character_index + 1);
}
}
return all_match;
}
bool string_contains(mlc::String haystack, mlc::String needle) noexcept{
auto needle_length = needle.length();
auto haystack_length = haystack.length();
if ((needle_length > haystack_length)) {
return false;
} else {
auto position = 0;
auto found = false;
while ((position <= (haystack_length - needle_length))) {
if (chars_match_at(haystack, position, needle, needle_length)) {
found = true;
position = haystack_length;
} else {
position = (position + 1);
}
}
return found;
}
}

} // namespace codegen_test_helpers
