#ifndef TEST_PIPE_AND_RECORD_UPDATE_HPP
#define TEST_PIPE_AND_RECORD_UPDATE_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "eval.hpp"
#include "context.hpp"
#include "exprs.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace test_pipe_and_record_update {

std::shared_ptr<semantic_ir::SExpr> sid(mlc::String name) noexcept;
std::shared_ptr<semantic_ir::SExpr> si(int v) noexcept;
mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept;

} // namespace test_pipe_and_record_update

#endif // TEST_PIPE_AND_RECORD_UPDATE_HPP
