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
#include "semantic_ir.hpp"
#include "registry.hpp"

namespace test_codegen {

std::shared_ptr<registry::Type> i32_t() noexcept;
std::shared_ptr<registry::Type> str_t() noexcept;
std::shared_ptr<registry::Type> bool_t() noexcept;
std::shared_ptr<registry::Type> unit_t() noexcept;
std::shared_ptr<registry::Type> unk_t() noexcept;
std::shared_ptr<semantic_ir::SExpr> si(int v) noexcept;
std::shared_ptr<semantic_ir::SExpr> ss(mlc::String v) noexcept;
std::shared_ptr<semantic_ir::SExpr> sb(bool v) noexcept;
std::shared_ptr<semantic_ir::SExpr> sid(mlc::String name) noexcept;
std::shared_ptr<semantic_ir::SExpr> su() noexcept;
mlc::Array<test_runner::TestResult> codegen_tests() noexcept;

} // namespace test_codegen

#endif // TEST_CODEGEN_HPP
