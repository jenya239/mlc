#include "decl_cpp.hpp"

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

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace print;
using namespace emit_helpers;
using namespace context;
using namespace decl;
using namespace decl_extend;
using namespace decl_index;
using namespace expr;
using namespace type_gen;
using namespace derive_methods_cpp;
using namespace trait_struct_cpp;
using namespace return_body;
using namespace cpp_naming;
using namespace ast_tokens;

std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_string_output(mlc::String output) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> empty_cpp_declaration() noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept;

mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

context::CodegenContext prototype_context_for_function(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

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

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_segment_for_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext codegen_context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_cpp_forward_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context, int phase) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_native_decl_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;

decl_cpp::DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty_cpp_declarations() noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_native_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_fn_defs_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_string_output(mlc::String output) noexcept{return emit_helpers::make_fragment_cpp_declaration(output);}

std::shared_ptr<cpp_ast::CppDeclaration> empty_cpp_declaration() noexcept{return std::make_shared<cpp_ast::CppDeclaration>((cpp_ast::CppDeclarationEmpty{}));}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept{return parts.map([](mlc::String part) mutable { return cpp_decl_from_string_output(part); });}

mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return print::print_decl(declaration);}

mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{return declarations.map([](std::shared_ptr<cpp_ast::CppDeclaration> declaration) mutable { return print_cpp_declaration(declaration); }).join(mlc::String(""));}

context::CodegenContext prototype_context_for_function(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{return params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->type_value))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->type_value)); auto [type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type : type_name;
  return context::CodegenContext_for_type_body(context, resolved_type);
 }(); } return context; }() : context;}

mlc::Array<mlc::String> function_parameter_proto_items(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext prototype_context) noexcept{return name == mlc::String("main") && params.size() == 0 ? main_program_parameter_items_cpp() : decl_extend::gen_parameter_proto_items(prototype_context, params);}

mlc::Array<mlc::String> function_parameter_def_items(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext prototype_context) noexcept{return name == mlc::String("main") && params.size() == 0 ? main_program_parameter_items_cpp() : decl_extend::gen_parameter_def_items(prototype_context, params);}

mlc::Array<mlc::String> main_program_parameter_items_cpp() noexcept{return mlc::Array<mlc::String>{mlc::String("int argc"), mlc::String("char** argv")};}

std::shared_ptr<cpp_ast::CppDeclaration> variant_forward_struct_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::String variant_name = context::CodegenContext_resolve(context, type_gen::variant_ctor_name(variant));
mlc::Array<mlc::String> used = type_gen::union_string_lists(type_gen::variant_used_type_parameters(type_params, variant), phantom);
return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(cpp_naming::template_prefix(used), variant_name, {}, true));
}

std::shared_ptr<cpp_ast::CppVariantArm> variant_alias_arm_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::String variant_name = context::CodegenContext_resolve(context, type_gen::variant_ctor_name(variant));
mlc::Array<mlc::String> used = type_gen::union_string_lists(type_gen::variant_used_type_parameters(type_params, variant), phantom);
mlc::String type_argument_suffix = used.size() > 0 ? mlc::String("<") + used.join(mlc::String(", ")) + mlc::String(">") : mlc::String("");
return std::make_shared<cpp_ast::CppVariantArm>(cpp_ast::CppVariantArm{variant_name + type_argument_suffix, {}});
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_type_forward_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> phantom = type_gen::type_phantom_params_for_variants(type_params, variants);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = output;
int variant_index = 0;
while (variant_index < variants.size()){
{
result.push_back(variant_forward_struct_cpp(context, type_params, phantom, variants[variant_index]));
variant_index = variant_index + 1;
}
}
mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> alias_arms = {};
variant_index = 0;
while (variant_index < variants.size()){
{
alias_arms.push_back(variant_alias_arm_cpp(context, type_params, phantom, variants[variant_index]));
variant_index = variant_index + 1;
}
}
result.push_back(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppVariant(cpp_naming::template_prefix(type_params), context::CodegenContext_resolve(context, type_name), alias_arms)));
return result;
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_fwd_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{} : append_type_forward_declarations({}, context, type_name, type_params, variants);}

std::shared_ptr<cpp_ast::CppField> record_field_cpp(context::CodegenContext context, std::shared_ptr<ast::FieldDef> field_definition) noexcept{return std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{type_gen::type_to_cpp(context, field_definition->type_value), cpp_naming::cpp_safe(field_definition->name)});}

mlc::Array<std::shared_ptr<cpp_ast::CppField>> record_fields_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.map([context](std::shared_ptr<ast::FieldDef> field_definition) mutable { return record_field_cpp(context, field_definition); });}

mlc::Array<std::shared_ptr<cpp_ast::CppField>> tuple_fields_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields = {};
int field_index = 0;
while (field_index < field_types.size()){
{
fields.push_back(std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{type_gen::type_to_cpp(context, field_types[field_index]), mlc::String("field") + mlc::to_string(field_index)}));
field_index = field_index + 1;
}
}
return fields;
}

std::shared_ptr<cpp_ast::CppDeclaration> variant_body_struct_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::Array<mlc::String> used = type_gen::union_string_lists(type_gen::variant_used_type_parameters(type_params, variant), phantom);
mlc::String prefix = cpp_naming::template_prefix(used);
return std::visit(overloaded{
  [&](const VarUnit& varunit) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [name, _w0] = varunit; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(prefix, context::CodegenContext_resolve(context, name), {}, false)); },
  [&](const VarTuple& vartuple) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [name, field_types, _w0] = vartuple; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(prefix, context::CodegenContext_resolve(context, name), tuple_fields_cpp(context, field_types), false)); },
  [&](const VarRecord& varrecord) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [name, field_definitions, _w0] = varrecord; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(prefix, context::CodegenContext_resolve(context, name), record_fields_cpp(context, field_definitions), false)); }
}, (*variant));
}

std::shared_ptr<cpp_ast::CppDeclaration> cpp_using_from_struct_using_entry(context::StructUsingEntry entry) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppUsing(entry.alias, entry.type_cpp));}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> struct_using_declarations_cpp(context::CodegenContext context, mlc::String type_name) noexcept{
mlc::Array<context::StructUsingEntry> using_entries = context.struct_using_entries.get(type_name);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations = {};
int entry_index = 0;
while (entry_index < using_entries.size()){
{
declarations.push_back(cpp_using_from_struct_using_entry(using_entries[entry_index]));
entry_index = entry_index + 1;
}
}
return declarations;
}

std::shared_ptr<cpp_ast::CppDeclaration> single_variant_body_struct_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::String prefix = cpp_naming::template_prefix(type_params);
mlc::String resolved_name = context::CodegenContext_resolve(context, type_name);
return std::visit(overloaded{
  [&](const VarUnit& varunit) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1] = varunit; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(prefix, resolved_name, {}, false)); },
  [&](const VarTuple& vartuple) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, field_types, _w1] = vartuple; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(prefix, resolved_name, tuple_fields_cpp(context, field_types), false)); },
  [&](const VarRecord& varrecord) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, field_definitions, _w1] = varrecord; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(prefix, resolved_name, record_fields_cpp(context, field_definitions), false)); }
}, (*variant));
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_type_body_struct_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = output;
  result.push_back(single_variant_body_struct_cpp(context, type_name, type_params, variants[0]));
  if (context.struct_using_entries.has(type_name)){
{
result = append_cpp_declarations(result, struct_using_declarations_cpp(context, type_name));
}
}
  return result;
 }() : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { 
  mlc::Array<mlc::String> phantom = type_gen::type_phantom_params_for_variants(type_params, variants);
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = output;
  int variant_index = 0;
  while (variant_index < variants.size()){
{
result.push_back(variant_body_struct_cpp(context, type_params, phantom, variants[variant_index]));
variant_index = variant_index + 1;
}
}
  return result;
 }();}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_body_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = append_type_body_struct_declarations({}, context, type_name, type_params, variants);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> derive_declarations = derive_methods_cpp::gen_derive_methods_cpp(type_name, variants, derive_traits);
int derive_index = 0;
while (derive_index < derive_declarations.size()){
{
result.push_back(derive_declarations[derive_index]);
derive_index = derive_index + 1;
}
}
return result;
}

mlc::String trait_template_header(mlc::Array<mlc::String> type_params) noexcept{return type_params.size() > 0 ? mlc::String("template<typename ") + type_params[0] + mlc::String(", typename Self>\n") : mlc::String("template<typename Self>\n");}

mlc::Array<mlc::String> trait_requires_expressions_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
context::CodegenContext trait_context = context::CodegenContext_for_type_body(context, trait_name);
mlc::Array<mlc::String> expressions = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return [&]() { 
  mlc::String prefixed_callee = cpp_naming::cpp_safe(mangled);
  mlc::String return_cpp = type_gen::sem_type_to_cpp(trait_context, return_type);
  mlc::String declval_arguments = decl_extend::concept_declval_arguments_for_params(trait_context, params);
  return expressions.push_back(expr::concept_requires_expression_method_returns_convertible(prefixed_callee, declval_arguments, return_cpp));
 }(); } return; }();
method_index = method_index + 1;
}
}
return expressions;
}

std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_decl_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppConceptRequires(trait_template_header(type_params), cpp_naming::cpp_safe(trait_name), mlc::String("Self"), trait_requires_expressions_cpp(context, trait_name, methods)));}

std::shared_ptr<cpp_ast::CppExpression> main_set_args_vector_argument_cpp() noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("std::vector<mlc::String>")), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("+"), emit_helpers::make_identifier_cpp_expression(mlc::String("argv")), emit_helpers::make_integer_cpp_expression(1))), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("+"), emit_helpers::make_identifier_cpp_expression(mlc::String("argv")), emit_helpers::make_identifier_cpp_expression(mlc::String("argc"))))}));}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> main_set_args_preamble_cpp() noexcept{return mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_expression_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::io::set_args")), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{main_set_args_vector_argument_cpp()})))};}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prepend_main_set_args_preamble(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> result = main_set_args_preamble_cpp();
int statement_index = 0;
while (statement_index < statements.size()){
{
result.push_back(statements[statement_index]);
statement_index = statement_index + 1;
}
}
return result;
}

mlc::String function_declaration_template_prefix(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept{return cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds);}

std::shared_ptr<cpp_ast::CppDeclaration> native_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
context::CodegenContext prototype_context = prototype_context_for_function(context, params);
mlc::String safe_name = context::CodegenContext_resolve(context, name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(prototype_context, return_type);
mlc::Array<mlc::String> parameters = function_parameter_proto_items(name, params, prototype_context);
return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnProto(function_declaration_template_prefix(type_params, type_bounds), return_type_cpp, safe_name, parameters));
}

std::shared_ptr<cpp_ast::CppDeclaration> native_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context, int body_statement_depth) noexcept{
context::CodegenContext body_context = decl::compute_fn_body_context(name, params, context);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? body_context : context;
mlc::String safe_name = context::CodegenContext_resolve(context, name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(prototype_context, return_type);
mlc::Array<mlc::String> parameters = function_parameter_def_items(name, params, prototype_context);
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> return_body_statements = return_body::gen_return_body_cpp(body, body_context);
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements = name == mlc::String("main") && params.size() == 0 ? prepend_main_set_args_preamble(return_body_statements) : return_body_statements;
return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef(function_declaration_template_prefix(type_params, type_bounds), return_type_cpp, safe_name, parameters, body_statements, body_statement_depth));
}

std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{return native_fn_proto_cpp(name, type_params, type_bounds, params, return_type, context);}

std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept{return native_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, context, 1);}

std::shared_ptr<cpp_ast::CppDeclaration> gen_decl_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{return CodegenContext_gen_decl_cpp(context, declaration);}

std::shared_ptr<cpp_ast::CppDeclaration> gen_proto_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{return CodegenContext_gen_proto_cpp(context, declaration);}

std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_decl_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const SemanticDeclarationFn& semanticdeclarationfn) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [name, type_params, type_bounds, params, return_type, body, _w0, _w1] = semanticdeclarationfn; return [&]() -> std::shared_ptr<cpp_ast::CppDeclaration> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return empty_cpp_declaration(); } return gen_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, self); }(); },
  [&](const SemanticDeclarationType& semanticdeclarationtype) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [type_name, type_params, variants, derive_traits, _w0] = semanticdeclarationtype; return cpp_decl_from_native_declarations(append_cpp_declarations(gen_type_decl_fwd_cpp(self, type_name, type_params, variants), gen_type_decl_body_cpp(self, type_name, type_params, variants, derive_traits))); },
  [&](const SemanticDeclarationTypeAlias& semanticdeclarationtypealias) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1, _w2, _w3] = semanticdeclarationtypealias; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationTrait& semanticdeclarationtrait) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [name, type_params, methods, _w0] = semanticdeclarationtrait; return gen_trait_decl_cpp(self, name, type_params, methods); },
  [&](const SemanticDeclarationExtend& semanticdeclarationextend) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [type_name, trait_name, methods, _w0] = semanticdeclarationextend; return cpp_decl_from_native_declarations(gen_decl_extend_cpp(type_name, trait_name, methods, self)); },
  [&](const SemanticDeclarationImport& semanticdeclarationimport) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1] = semanticdeclarationimport; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationAssocBind& semanticdeclarationassocbind) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1, _w2] = semanticdeclarationassocbind; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationExported& semanticdeclarationexported) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [inner_declaration] = semanticdeclarationexported; return CodegenContext_gen_decl_cpp(self, semantic_ir::sdecl_inner(inner_declaration)); }
}, (*declaration));}

std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_proto_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const SemanticDeclarationFn& semanticdeclarationfn) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [name, type_params, type_bounds, params, return_type, body, _w0, _w1] = semanticdeclarationfn; return [&]() -> std::shared_ptr<cpp_ast::CppDeclaration> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return empty_cpp_declaration(); } return gen_fn_proto_cpp(name, type_params, type_bounds, params, return_type, self); }(); },
  [&](const SemanticDeclarationType& semanticdeclarationtype) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1, _w2, _w3, _w4] = semanticdeclarationtype; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationTypeAlias& semanticdeclarationtypealias) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1, _w2, _w3] = semanticdeclarationtypealias; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationTrait& semanticdeclarationtrait) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1, _w2, _w3] = semanticdeclarationtrait; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationExtend& semanticdeclarationextend) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [type_name, _w0, methods, _w1] = semanticdeclarationextend; return [&]() -> std::shared_ptr<cpp_ast::CppDeclaration> { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(self, type_name);
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> proto_declarations = {};
  int method_index = 0;
  while (method_index < methods.size()){
{
std::shared_ptr<semantic_ir::SemanticDeclaration> method = methods[method_index];
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [_w0, _w1, _w2, _w3, _w4, body, _w5, _w6] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return std::make_tuple(); } return [&]() -> std::tuple<> { 
  proto_declarations.push_back(CodegenContext_gen_proto_cpp(extend_context, method));
  return std::make_tuple();
 }(); }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
  return cpp_decl_from_native_declarations(proto_declarations);
 }(); },
  [&](const SemanticDeclarationImport& semanticdeclarationimport) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1] = semanticdeclarationimport; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationAssocBind& semanticdeclarationassocbind) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [_w0, _w1, _w2] = semanticdeclarationassocbind; return empty_cpp_declaration(); },
  [&](const SemanticDeclarationExported& semanticdeclarationexported) -> std::shared_ptr<cpp_ast::CppDeclaration> { auto [inner_declaration] = semanticdeclarationexported; return CodegenContext_gen_proto_cpp(self, semantic_ir::sdecl_inner(inner_declaration)); }
}, (*declaration));}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_method_forward_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = {};
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declarations[declaration_index]))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declarations[declaration_index])); auto [_w0, _w1, methods, _w2] = _v_semanticdeclarationextend; return [&]() -> std::tuple<> { 
  int method_index = 0;
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [_w0, _w1, _w2, _w3, _w4, body, _w5, _w6] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return std::make_tuple(); } return [&]() -> std::tuple<> { 
  result.push_back(gen_proto_cpp(methods[method_index], context));
  return std::make_tuple();
 }(); }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
declaration_index = declaration_index + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_helper_protos_for_exported_types_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> exported_types = decl::exported_type_names_from_decls(declarations);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = {};
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declarations[declaration_index]))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declarations[declaration_index])); auto [type_name, _w0, methods, _w1] = _v_semanticdeclarationextend; return [&]() -> std::tuple<> { 
  if (decl_index::list_contains(exported_types, type_name)){
{
int method_index = 0;
[&]() { 
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [_w0, _w1, _w2, _w3, _w4, body, _w5, _w6] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return std::make_tuple(); } return [&]() -> std::tuple<> { 
  result.push_back(gen_proto_cpp(methods[method_index], context));
  return std::make_tuple();
 }(); }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
 }();
}
}
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
declaration_index = declaration_index + 1;
}
}
return result;
}

mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
mlc::String result = mlc::String("void");
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [_w0, _w1, _w2, _w3, return_type, _w4, _w5, _w6] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { 
  result = type_gen::sem_type_to_cpp(extend_context, return_type);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
if (result != mlc::String("void")){
{
method_index = methods.size();
}
} else {
{
method_index = method_index + 1;
}
}
}
}
return result;
}

mlc::String extend_forward_call_argument_list(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> argument_names = {};
int parameter_index = 0;
while (parameter_index < params.size()){
{
argument_names.push_back(cpp_naming::cpp_safe(params[parameter_index]->name));
parameter_index = parameter_index + 1;
}
}
return argument_names.join(mlc::String(", "));
}

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> extend_forward_call_argument_expressions(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions = {};
int parameter_index = 0;
while (parameter_index < params.size()){
{
expressions.push_back(emit_helpers::make_identifier_cpp_expression(cpp_naming::cpp_safe(params[parameter_index]->name)));
parameter_index = parameter_index + 1;
}
}
return expressions;
}

std::shared_ptr<cpp_ast::CppDeclaration> native_inline_noexcept_forwarding_fn_cpp(mlc::String return_type_cpp, mlc::String method_name_cpp, mlc::Array<mlc::String> parameter_items, mlc::String callee_code, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> call_arguments) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef(mlc::String("inline "), return_type_cpp, method_name_cpp, parameter_items, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(callee_code), call_arguments)))}, -1));}

std::shared_ptr<cpp_ast::CppDeclaration> native_noexcept_one_line_returning_fn_cpp(mlc::String return_type_cpp, mlc::String name_cpp, mlc::Array<mlc::String> parameter_items, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef(mlc::String(""), return_type_cpp, name_cpp, parameter_items, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(return_expression)}, -1));}

std::shared_ptr<cpp_ast::CppDeclaration> extend_trait_wrapper_decl_cpp(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
context::CodegenContext wrapper_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String method_name = decl_extend::extract_method_name(mangled, type_name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(wrapper_context, return_type);
return native_inline_noexcept_forwarding_fn_cpp(return_type_cpp, cpp_naming::cpp_safe(method_name), decl_extend::gen_parameter_proto_items(wrapper_context, params), context::context_resolve(context, mangled), extend_forward_call_argument_expressions(params));
}

std::shared_ptr<cpp_ast::CppDeclaration> extend_trait_concept_adapter_decl_cpp(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String method_name = decl_extend::extract_method_name(mangled, type_name);
mlc::String trait_callee = cpp_naming::cpp_safe(trait_name + mlc::String("_") + method_name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(extend_context, return_type);
return native_inline_noexcept_forwarding_fn_cpp(return_type_cpp, trait_callee, decl_extend::gen_parameter_proto_items(extend_context, params), context::context_resolve(context, mangled), extend_forward_call_argument_expressions(params));
}

std::shared_ptr<cpp_ast::CppDeclaration> extend_trait_static_assert_decl_cpp(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept{
mlc::String trait_safe = cpp_naming::cpp_safe(trait_name);
mlc::String message = type_name + mlc::String(" does not implement ") + trait_name;
return trait_name == mlc::String("ExprVisitor") ? [&]() -> std::shared_ptr<cpp_ast::CppDeclaration> { 
  mlc::String result_type_cpp = extend_method_return_type_cpp(extend_context, methods);
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStaticAssert(trait_safe, mlc::Array<mlc::String>{result_type_cpp, implementor_type_cpp}, message));
 }() : std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStaticAssert(trait_safe, mlc::Array<mlc::String>{implementor_type_cpp}, message));
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_trait_suffix_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String implementor_type_cpp = type_gen::type_name_to_cpp(context, type_name);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { 
  result.push_back(extend_trait_wrapper_decl_cpp(type_name, mangled, params, return_type, context, context_resolve));
  result.push_back(extend_trait_concept_adapter_decl_cpp(type_name, trait_name, mangled, params, return_type, context, context_resolve));
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
result.push_back(extend_trait_static_assert_decl_cpp(trait_name, type_name, methods, extend_context, implementor_type_cpp));
return result;
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_extend_extern_method_cpp(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
mlc::String method_name = trait_name.length() > 0 ? decl_extend::extract_method_name(mangled, type_name) : mlc::String("");
return type_name == mlc::String("i32") && method_name == mlc::String("to_string") && params.size() > 0 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { 
  mlc::String return_type_cpp = type_gen::sem_type_to_cpp(context, return_type);
  mlc::String parameter_name = cpp_naming::cpp_safe(params[0]->name);
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = {};
  result.push_back(native_noexcept_one_line_returning_fn_cpp(return_type_cpp, context::context_resolve(context, mangled), decl_extend::gen_parameter_def_items(context, params), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::to_string")), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{emit_helpers::make_identifier_cpp_expression(parameter_name)}))));
  return result;
 }() : empty_cpp_declarations();
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
auto context_resolve = [](context::CodegenContext codegen_context, mlc::String name) mutable { return context::CodegenContext_resolve(codegen_context, name); };
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [mangled, type_params, type_bounds, params, return_type, body, _w0, _w1] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return [&]() -> std::tuple<> { 
  result = append_cpp_declarations(result, gen_extend_extern_method_cpp(mangled, type_name, params, return_type, trait_name, context, context_resolve));
  return std::make_tuple();
 }(); } return [&]() -> std::tuple<> { 
  result.push_back(native_fn_decl_cpp(mangled, type_params, type_bounds, params, return_type, body, extend_context, 0));
  return std::make_tuple();
 }(); }();
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
if (trait_name.length() > 0){
{
result = append_cpp_declarations(result, gen_decl_extend_trait_suffix_cpp(type_name, trait_name, methods, context, context_resolve));
}
}
return result;
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_extend_wrapper_protos_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
return trait_name.length() == 0 ? empty_cpp_declarations() : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> wrappers = {};
  int method_index = 0;
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { 
  context::CodegenContext wrapper_context = context::CodegenContext_for_type_body(context, type_name);
  mlc::String method_name = decl_extend::extract_method_name(mangled, type_name);
  wrappers.push_back(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnProto(mlc::String(""), type_gen::sem_type_to_cpp(wrapper_context, return_type), cpp_naming::cpp_safe(method_name), decl_extend::gen_parameter_proto_items(wrapper_context, params))));
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
  return wrappers;
 }();
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_segment_for_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext codegen_context) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> result = gen_extend_wrapper_protos_cpp(type_name, trait_name, methods, codegen_context);
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [_w0, _w1, _w2, _w3, _w4, body, _w5, _w6] = _v_semanticdeclarationfn; return [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return std::make_tuple(); } return [&]() -> std::tuple<> { 
  result.push_back(gen_proto_cpp(methods[method_index], extend_context));
  return std::make_tuple();
 }(); }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_cpp_forward_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declaration))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declaration)); auto [type_name, trait_name, methods, _w0] = _v_semanticdeclarationextend; return extend_forward_segment_for_extend(type_name, trait_name, methods, codegen_context); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declaration))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declaration)); auto [inner_declaration] = _v_semanticdeclarationexported; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, trait_name, methods, _w0] = _v_semanticdeclarationextend; return extend_forward_segment_for_extend(type_name, trait_name, methods, codegen_context); } return empty_cpp_declarations(); }(); } return empty_cpp_declarations(); }();}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context, int phase) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*declaration))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*declaration)); auto [type_name, type_params, variants, derive_traits, _w0] = _v_semanticdeclarationtype; return phase == 0 ? gen_type_decl_fwd_cpp(context, type_name, type_params, variants) : phase == 1 ? gen_type_decl_body_cpp(context, type_name, type_params, variants, derive_traits) : empty_cpp_declarations(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*declaration))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*declaration)); auto [name, type_params, methods, _w0] = _v_semanticdeclarationtrait; return phase == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_trait_decl_cpp(context, name, type_params, methods)} : phase == 4 ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{trait_struct_cpp::gen_trait_struct_cpp(context, name, type_params, methods)} : empty_cpp_declarations(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*declaration))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*declaration)); auto [name, type_params, type_bounds, params, return_type, body, _w0, _w1] = _v_semanticdeclarationfn; return phase == 2 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return empty_cpp_declarations(); } return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(declaration, context)}; }() : empty_cpp_declarations(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declaration))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declaration)); auto [inner_declaration] = _v_semanticdeclarationexported; return phase == 0 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, type_params, variants, _w0, _w1] = _v_semanticdeclarationtype; return gen_type_decl_fwd_cpp(context, type_name, type_params, variants); } if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [name, type_params, methods, _w0] = _v_semanticdeclarationtrait; return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_trait_decl_cpp(context, name, type_params, methods)}; } return empty_cpp_declarations(); }() : phase == 1 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, type_params, variants, derive_traits, _w0] = _v_semanticdeclarationtype; return gen_type_decl_body_cpp(context, type_name, type_params, variants, derive_traits); } return empty_cpp_declarations(); }() : phase == 2 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, type_parameters, _w1, _w2, _w3, body, _w4, _w5] = _v_semanticdeclarationfn; return type_parameters.size() > 0 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(inner_declaration, context)}; } return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_decl_cpp(inner_declaration, context)}; }() : mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(inner_declaration, context)}; } return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(inner_declaration, context)}; }() : phase == 4 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [name, type_params, methods, _w0] = _v_semanticdeclarationtrait; return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{trait_struct_cpp::gen_trait_struct_cpp(context, name, type_params, methods)}; } return empty_cpp_declarations(); }() : empty_cpp_declarations(); } return empty_cpp_declarations(); }();}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_native_decl_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output = {};
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
output = append_cpp_declarations(output, decl_segment_cpp(declarations[declaration_index], context, phase));
declaration_index = declaration_index + 1;
}
}
return output;
}

decl_cpp::DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> fn_protos = collect_decl_parts_cpp(declarations, context, 2);
fn_protos = append_cpp_declarations(fn_protos, extend_helper_protos_for_exported_types_cpp(declarations, context));
return decl_cpp::DeclPartsBundleCpp{collect_decl_parts_cpp(declarations, context, 0), collect_decl_parts_cpp(declarations, context, 1), fn_protos, collect_fn_defs_cpp(declarations, context)};
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept{return phase == 0 || phase == 1 || phase == 2 || phase == 4 ? collect_native_decl_segments_cpp(declarations, context, phase) : phase == 3 ? collect_fn_defs_cpp(declarations, context) : empty_cpp_declarations();}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty_cpp_declarations() noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty = {};
return empty;
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
int index = 0;
while (index < declarations.size()){
{
output.push_back(declarations[index]);
index = index + 1;
}
}
return output;
}

std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_native_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{return declarations.size() == 0 ? empty_cpp_declaration() : declarations.size() == 1 ? declarations[0] : std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDeclarationSequence(declarations));}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*declaration))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*declaration)); auto [name, type_params, type_bounds, params, return_type, body, _w0, _w1] = _v_semanticdeclarationfn; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return empty_cpp_declarations(); } return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, context)}; }(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declaration))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declaration)); auto [type_name, trait_name, methods, _w0] = _v_semanticdeclarationextend; return gen_decl_extend_cpp(type_name, trait_name, methods, context); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declaration))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declaration)); auto [inner_declaration] = _v_semanticdeclarationexported; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticdeclarationtype; return empty_cpp_declarations(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2, _w3] = _v_semanticdeclarationtrait; return empty_cpp_declarations(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, type_parameters, _w1, _w2, _w3, body, _w4, _w5] = _v_semanticdeclarationfn; return type_parameters.size() > 0 ? [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_decl_cpp(inner_declaration, context)}; } return empty_cpp_declarations(); }() : mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_decl_cpp(inner_declaration, context)}; } if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, trait_name, methods, _w0] = _v_semanticdeclarationextend; return gen_decl_extend_cpp(type_name, trait_name, methods, context); } return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_decl_cpp(inner_declaration, context)}; }(); } return empty_cpp_declarations(); }();}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_fn_defs_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output = {};
output = append_cpp_declarations(output, extend_method_forward_segments_cpp(declarations, context));
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
std::shared_ptr<semantic_ir::SemanticDeclaration> declaration = declarations[declaration_index];
output = append_cpp_declarations(output, extend_cpp_forward_segment_cpp(declaration, context));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> definition_segments = decl_fn_def_cpp(declaration, context);
int segment_index = 0;
while (segment_index < definition_segments.size()){
{
output.push_back(definition_segments[segment_index]);
segment_index = segment_index + 1;
}
}
output = append_cpp_declarations(output, collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{declaration}, context, 4));
declaration_index = declaration_index + 1;
}
}
return output;
}

} // namespace decl_cpp
