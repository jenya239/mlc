#ifndef TEST_CPP_TO_SOURCE_HPP
#define TEST_CPP_TO_SOURCE_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "to_source.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_to_source {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_to_source_tests() noexcept;

} // namespace test_cpp_to_source

#endif // TEST_CPP_TO_SOURCE_HPP
