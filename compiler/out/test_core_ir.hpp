#ifndef TEST_CORE_IR_HPP
#define TEST_CORE_IR_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "core.hpp"
#include "verify_core.hpp"
#include "core_dump.hpp"

namespace test_core_ir {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> core_ir_tests() noexcept;

} // namespace test_core_ir

#endif // TEST_CORE_IR_HPP
