#ifndef TEST_RECORD_FIELD_DEFAULT_VALIDATE_HPP
#define TEST_RECORD_FIELD_DEFAULT_VALIDATE_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "record_field_default_validate.hpp"
#include "ast.hpp"

namespace test_record_field_default_validate {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> record_field_default_validate_tests() noexcept;

} // namespace test_record_field_default_validate

#endif // TEST_RECORD_FIELD_DEFAULT_VALIDATE_HPP
