#ifndef DECL_EXTEND_HPP
#define DECL_EXTEND_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "literals.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "expr.hpp"

namespace decl_extend {

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;
mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> e) noexcept;
mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept;
mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept;
mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept;
mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept;
mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept;
mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept;

} // namespace decl_extend

#endif // DECL_EXTEND_HPP
