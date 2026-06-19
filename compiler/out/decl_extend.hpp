#ifndef DECL_EXTEND_HPP
#define DECL_EXTEND_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "ast.hpp"
#include "literals.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "expr.hpp"

namespace decl_extend {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> gen_parameter_proto_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> gen_parameter_def_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> non_self_parameter_types_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> parameter_call_argument_names(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::String concept_declval_arguments_for_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_decl_extend_trait_suffix(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;

} // namespace decl_extend

#endif // DECL_EXTEND_HPP
