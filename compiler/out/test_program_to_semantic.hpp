#ifndef TEST_PROGRAM_TO_SEMANTIC_HPP
#define TEST_PROGRAM_TO_SEMANTIC_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "program_to_semantic.hpp"
#include "decls.hpp"
#include "lexer.hpp"

namespace test_program_to_semantic {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> program_to_semantic_tests() noexcept;

} // namespace test_program_to_semantic

#endif // TEST_PROGRAM_TO_SEMANTIC_HPP
