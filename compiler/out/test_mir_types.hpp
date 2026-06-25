#ifndef TEST_MIR_TYPES_HPP
#define TEST_MIR_TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "mir_types.hpp"
#include "verify_mir.hpp"
#include "mir_dump.hpp"
#include "registry.hpp"

namespace test_mir_types {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> mir_types_tests() noexcept;

} // namespace test_mir_types

#endif // TEST_MIR_TYPES_HPP
