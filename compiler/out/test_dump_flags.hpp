#ifndef TEST_DUMP_FLAGS_HPP
#define TEST_DUMP_FLAGS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "compile_options.hpp"
#include "dump_flags.hpp"
#include "semantic_ir_dump.hpp"
#include "program_to_semantic.hpp"
#include "decls.hpp"
#include "lexer.hpp"

namespace test_dump_flags {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> dump_flags_tests() noexcept;

} // namespace test_dump_flags

#endif // TEST_DUMP_FLAGS_HPP
