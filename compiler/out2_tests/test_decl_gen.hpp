#ifndef TEST_DECL_GEN_HPP
#define TEST_DECL_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "ast_builders.hpp"
#include "codegen_test_helpers.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"

namespace test_decl_gen {

std::shared_ptr<registry::Type> ti32() noexcept;
std::shared_ptr<registry::Type> tstr() noexcept;
std::shared_ptr<registry::Type> tunit() noexcept;
std::shared_ptr<registry::Type> unk() noexcept;
std::shared_ptr<semantic_ir::SExpr> make_ident(mlc::String name) noexcept;
std::shared_ptr<semantic_ir::SExpr> make_bin(mlc::String op, std::shared_ptr<semantic_ir::SExpr> left, std::shared_ptr<semantic_ir::SExpr> right) noexcept;
std::shared_ptr<semantic_ir::SExpr> make_unit_expr() noexcept;
std::shared_ptr<ast::Param> iparam(mlc::String name) noexcept;
std::shared_ptr<semantic_ir::SDecl> fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body) noexcept;
std::shared_ptr<semantic_ir::SExpr> make_str_expr(mlc::String text) noexcept;
mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept;

} // namespace test_decl_gen

#endif // TEST_DECL_GEN_HPP
