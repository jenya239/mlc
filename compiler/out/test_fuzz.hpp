#ifndef TEST_FUZZ_HPP
#define TEST_FUZZ_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "random_program.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "decl_index.hpp"
#include "pipeline.hpp"

namespace test_fuzz {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept;

} // namespace test_fuzz

#endif // TEST_FUZZ_HPP
