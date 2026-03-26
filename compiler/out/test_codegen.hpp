#ifndef TEST_CODEGEN_HPP
#define TEST_CODEGEN_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "eval.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "module.hpp"
#include "ast.hpp"

namespace test_codegen {

struct Expr;
struct Stmt;

mlc::Array<test_runner::TestResult> codegen_tests() noexcept;

} // namespace test_codegen

#endif // TEST_CODEGEN_HPP
