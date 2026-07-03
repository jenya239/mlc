#ifndef DECL_CPP_HPP
#define DECL_CPP_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "decl_extend.hpp"
#include "decl_index.hpp"
#include "expr.hpp"
#include "type_gen.hpp"
#include "derive_methods_cpp.hpp"
#include "trait_struct_cpp.hpp"
#include "return_body.hpp"
#include "cpp_naming.hpp"
namespace decl_cpp {

struct DeclPartsBundleCpp {
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> type_fwds;
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> type_defs;
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> fn_protos;
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> fn_defs;
};
std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_string_output(mlc::String output) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> empty_cpp_declaration() noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept;
mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;
mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;
mlc::Array<mlc::String> function_parameter_proto_items(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext prototype_context) noexcept;
mlc::Array<mlc::String> function_parameter_def_items(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext prototype_context) noexcept;
mlc::Array<mlc::String> main_program_parameter_items_cpp() noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> variant_forward_struct_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;
std::shared_ptr<cpp_ast::CppVariantArm> variant_alias_arm_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_type_forward_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_fwd_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
std::shared_ptr<cpp_ast::CppField> record_field_cpp(context::CodegenContext context, std::shared_ptr<ast::FieldDef> field_definition) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppField>> record_fields_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppField>> tuple_fields_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> variant_body_struct_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> cpp_using_from_struct_using_entry(context::StructUsingEntry entry) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> struct_using_declarations_cpp(context::CodegenContext context, mlc::String type_name) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> single_variant_body_struct_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_type_body_struct_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_body_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;
mlc::String trait_template_header(mlc::Array<mlc::String> type_params) noexcept;
mlc::String trait_requires_expression_for_method(context::CodegenContext trait_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::Array<mlc::String> trait_requires_expressions_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_decl_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
std::shared_ptr<cpp_ast::CppExpression> main_set_args_vector_argument_cpp() noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> main_set_args_preamble_cpp() noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prepend_main_set_args_preamble(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;
mlc::String function_declaration_template_prefix(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> native_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> native_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context, int body_statement_depth) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> gen_decl_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> gen_proto_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_decl_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_proto_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_method_forward_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_helper_protos_for_exported_types_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String extend_forward_call_argument_list(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> extend_forward_call_argument_expressions(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> native_inline_noexcept_forwarding_fn_cpp(mlc::String return_type_cpp, mlc::String method_name_cpp, mlc::Array<mlc::String> parameter_items, mlc::String callee_code, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> call_arguments) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> native_noexcept_one_line_returning_fn_cpp(mlc::String return_type_cpp, mlc::String name_cpp, mlc::Array<mlc::String> parameter_items, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> extend_trait_wrapper_decl_cpp(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> extend_trait_concept_adapter_decl_cpp(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> extend_trait_static_assert_decl_cpp(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_trait_suffix_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_extend_extern_method_cpp(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_extend_wrapper_protos_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;
bool extern_body_is_extern(std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_proto_for_fn_method(std::shared_ptr<semantic_ir::SemanticDeclaration> method, context::CodegenContext extend_context, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_proto_for_method(std::shared_ptr<semantic_ir::SemanticDeclaration> method, context::CodegenContext extend_context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_segment_for_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext codegen_context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_cpp_forward_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_type_cpp(mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_trait_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_fn_phase_two_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_exported_fn_phase_two_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_exported_fn_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_for_extend_declaration_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_for_exported_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_native_fn_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_native_decl_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;
DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty_cpp_declarations() noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_native_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_exported_fn_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_extend_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_for_exported_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_native_fn_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_fn_defs_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

} // namespace decl_cpp

#endif // DECL_CPP_HPP
