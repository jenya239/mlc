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
#include "return_body.hpp"
#include "expr.hpp"
#include "decl_extend.hpp"
#include "decl_index.hpp"
namespace decl {

struct DeclPartsBundle {
  mlc::Array<mlc::String> type_fwds;
  mlc::Array<mlc::String> type_defs;
  mlc::Array<mlc::String> fn_protos;
  mlc::Array<mlc::String> fn_defs;
};
mlc::String resolved_self_type_name(mlc::String type_name, context::CodegenContext context) noexcept;
context::CodegenContext prototype_context_for_self_parameter(context::CodegenContext context, std::shared_ptr<ast::Param> first_parameter) noexcept;
mlc::String body_self_type_for_parameter(context::CodegenContext context, std::shared_ptr<ast::Param> first_parameter) noexcept;
context::CodegenContext prototype_context_for_function(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;
mlc::String body_self_type_for_parameters(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
context::CodegenContext decl_prototype_context_for_body(mlc::Array<std::shared_ptr<ast::Param>> parameters, context::CodegenContext body_context, context::CodegenContext context) noexcept;
context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> parameters, context::CodegenContext context) noexcept;
mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;
mlc::String CodegenContext_gen_decl(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::String CodegenContext_gen_proto(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::String gen_decl(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;
mlc::String gen_proto(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;
bool inner_declaration_is_main(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;
mlc::String extend_cpp_forward_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept;
mlc::String decl_segment_for_type(mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits, context::CodegenContext codegen_context, int phase) noexcept;
mlc::String decl_segment_for_trait(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext codegen_context, int phase) noexcept;
mlc::String decl_segment_for_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept;
mlc::String decl_segment_for_extend(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept;
mlc::String decl_segment_for_import_or_assoc(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept;
bool extern_body_is_extern(std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;
mlc::String decl_segment_for_exported_function_phase_two(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext codegen_context) noexcept;
mlc::String decl_segment_for_exported_function_phase_three(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext codegen_context) noexcept;
mlc::String decl_segment_for_exported_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext codegen_context, int phase) noexcept;
mlc::String decl_segment_for_exported(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept;
mlc::String decl_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept;
mlc::Array<mlc::String> exported_type_name_from_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::Array<mlc::String> exported_type_names_from_decls(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;
mlc::Array<mlc::String> extend_method_prototypes(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;
mlc::Array<mlc::String> extend_helper_prototypes_for_declaration(mlc::Array<mlc::String> exported_types, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::Array<mlc::String> extend_forward_prototypes_for_declaration(context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::Array<mlc::String> extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
mlc::Array<mlc::String> extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
DeclPartsBundle collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
mlc::String extend_trait_suffix_header_for_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;
mlc::Array<mlc::String> collect_extend_trait_suffix_header(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;

} // namespace decl

#endif // DECL_HPP
