#include "test_record_field_default_validate.hpp"

#include "test_runner.hpp"
#include "record_field_default_validate.hpp"
#include "ast.hpp"

namespace test_record_field_default_validate {

using namespace test_runner;
using namespace record_field_default_validate;
using namespace ast;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> record_field_default_validate_tests() noexcept;

mlc::Array<test_runner::TestResult> record_field_default_validate_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("static initializer accepts literal"), record_field_default_validate::record_field_default_expression_is_static_initializer(std::make_shared<ast::Expr>(ast::ExprInt(1, ast::span_unknown())))));
results.push_back(test_runner::assert_true(mlc::String("static initializer rejects call"), !record_field_default_validate::record_field_default_expression_is_static_initializer(std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("foo"), ast::span_unknown())), {}, ast::span_unknown())))));
return results;
}

} // namespace test_record_field_default_validate
