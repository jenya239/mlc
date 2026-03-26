#include "codegen_test_helpers.hpp"

#include "test_runner.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "eval.hpp"
#include "type_gen.hpp"
#include "ast_builders.hpp"

namespace codegen_test_helpers {

using namespace test_runner;
using namespace ast;
using namespace context;
using namespace eval;
using namespace type_gen;
using namespace ast_builders;
using namespace ast_tokens;

context::CodegenContext empty_codegen_context() noexcept;

test_runner::TestResult assert_expr_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<ast::Expr> expr, mlc::String expected_code) noexcept;

test_runner::TestResult assert_type_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr, mlc::String expected_code) noexcept;

test_runner::TestResult assert_code_contains(mlc::String test_name, mlc::String actual_code, mlc::String expected_substring) noexcept;

bool chars_match_at(mlc::String haystack, int start_position, mlc::String needle, int needle_length) noexcept;

bool string_contains(mlc::String haystack, mlc::String needle) noexcept;

context::CodegenContext empty_codegen_context() noexcept{return context::create_codegen_context(ast_builders::empty_program());}

test_runner::TestResult assert_expr_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<ast::Expr> expr, mlc::String expected_code) noexcept{return test_runner::assert_eq_str(test_name, eval::gen_expr(expr, context), expected_code);}

test_runner::TestResult assert_type_generates(mlc::String test_name, context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr, mlc::String expected_code) noexcept{return test_runner::assert_eq_str(test_name, type_gen::type_to_cpp(context, type_expr), expected_code);}

test_runner::TestResult assert_code_contains(mlc::String test_name, mlc::String actual_code, mlc::String expected_substring) noexcept{return test_runner::assert_true(test_name + mlc::String(" contains '") + expected_substring + mlc::String("'"), string_contains(actual_code, expected_substring));}

bool chars_match_at(mlc::String haystack, int start_position, mlc::String needle, int needle_length) noexcept{
int character_index = 0;
bool all_match = true;
while (character_index < needle_length){
{
if (haystack.char_at(start_position + character_index) != needle.char_at(character_index)){
{
all_match = false;
character_index = needle_length;
}
} else {
{
character_index = character_index + 1;
}
}
}
}
return all_match;
}

bool string_contains(mlc::String haystack, mlc::String needle) noexcept{
int needle_length = needle.length();
int haystack_length = haystack.length();
return needle_length > haystack_length ? false : [&]() -> bool { 
  int position = 0;
  bool found = false;
  while (position <= haystack_length - needle_length){
{
if (chars_match_at(haystack, position, needle, needle_length)){
{
found = true;
position = haystack_length;
}
} else {
{
position = position + 1;
}
}
}
}
  return found;
 }();
}

} // namespace codegen_test_helpers
