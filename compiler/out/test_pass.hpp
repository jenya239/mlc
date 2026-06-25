#ifndef TEST_PASS_HPP
#define TEST_PASS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "compile_options.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "load_item.hpp"
#include "pipeline.hpp"

namespace test_pass {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> pass_type_tests() noexcept;

} // namespace test_pass

#endif // TEST_PASS_HPP
