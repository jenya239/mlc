#ifndef DECL_HPP
#define DECL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "eval.hpp"

namespace decl {

struct Expr;
struct Stmt;

mlc::String gen_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, context::CodegenContext context) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, context::CodegenContext context) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, context::CodegenContext context, int phase) noexcept;

} // namespace decl

#endif // DECL_HPP
