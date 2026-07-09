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

mlc::String trait_dispatch_name(mlc::String trait_name, mlc::String method_name) noexcept;
mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;
mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> expression) noexcept;
mlc::String generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter, mlc::String generic_type_name) noexcept;
mlc::String non_generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> gen_parameter_proto_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> gen_parameter_def_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> non_self_parameter_types_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> parameter_call_argument_names(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::String concept_declval_arguments_for_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::String extend_method_return_type_from_declaration(context::CodegenContext extend_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String trait_concept_cpp_name_for_static_assert(mlc::String trait_name) noexcept;
mlc::String generic_trait_static_assert_line(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept;
mlc::String trait_struct_field_line_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::Array<mlc::String> trait_requires_requirement_parts(context::CodegenContext trait_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_wrapper_proto_line(mlc::String type_name, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_trait_wrapper_line(mlc::String type_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_trait_concept_adapter_line(mlc::String type_name, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::Array<mlc::String> extend_method_part_strings(mlc::String type_name, mlc::String trait_name, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String trait_method_field_line(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept;
mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
bool trait_skips_concept_dispatch_forward_protos(mlc::String trait_name) noexcept;
mlc::String trait_concept_dispatch_forward_proto_line(mlc::String trait_name, mlc::String mangled, mlc::String return_type_cpp) noexcept;
mlc::String trait_concept_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String gen_trait_decl_with_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;
mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;
mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_extend_trait_concept_adapters(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_decl_extend_trait_suffix(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_decl_extend_methods_only(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;
mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;

} // namespace decl_extend

#endif // DECL_EXTEND_HPP
