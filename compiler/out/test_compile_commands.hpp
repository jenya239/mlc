#ifndef TEST_COMPILE_COMMANDS_HPP
#define TEST_COMPILE_COMMANDS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "compile_commands.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "decl_index.hpp"
#include "pipeline.hpp"

namespace test_compile_commands {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> compile_commands_tests() noexcept;

} // namespace test_compile_commands

#endif // TEST_COMPILE_COMMANDS_HPP
