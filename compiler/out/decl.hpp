#ifndef DECL_HPP
#define DECL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "eval.hpp"
#include "expr.hpp"

namespace decl {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String gen_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept;

mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept;

} // namespace decl

#endif // DECL_HPP
