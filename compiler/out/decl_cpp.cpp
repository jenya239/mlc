#define main mlc_user_main
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

std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_decl_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_proto_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_string_output(mlc::String output) noexcept{
  return emit_helpers::make_fragment_cpp_declaration(output);
}
std::shared_ptr<cpp_ast::CppDeclaration> empty_cpp_declaration() noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDeclarationEmpty{});
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept{
  return parts.map([=](mlc::String part) mutable { return cpp_decl_from_string_output(part); });
}
mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
  return print::print_decl(declaration);
}
mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
  return declarations.map([=](std::shared_ptr<cpp_ast::CppDeclaration> declaration) mutable { return print_cpp_declaration(declaration); }).join(mlc::String("", 0));
}
mlc::Array<mlc::String> function_parameter_proto_items(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext prototype_context) noexcept{
  if (((name == mlc::String("main", 4)) && (params.length() == 0)))   {
    return main_program_parameter_items_cpp();
  } else   {
    return decl_extend::gen_parameter_proto_items(prototype_context, params);
  }
}
mlc::Array<mlc::String> function_parameter_def_items(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext prototype_context) noexcept{
  if (((name == mlc::String("main", 4)) && (params.length() == 0)))   {
    return main_program_parameter_items_cpp();
  } else   {
    return decl_extend::gen_parameter_def_items(prototype_context, params);
  }
}
mlc::Array<mlc::String> main_program_parameter_items_cpp() noexcept{
  return mlc::Array<mlc::String>{mlc::String("int argc", 8), mlc::String("char** argv", 11)};
}
std::shared_ptr<cpp_ast::CppDeclaration> variant_forward_struct_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto variant_name = context::CodegenContext_resolve(context, type_gen::variant_ctor_name(variant));
  auto used = type_gen::union_string_lists(type_gen::variant_used_type_parameters(type_params, variant), phantom);
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{cpp_naming::template_prefix(used), variant_name, {}, true});
}
std::shared_ptr<cpp_ast::CppVariantArm> variant_alias_arm_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto variant_name = context::CodegenContext_resolve(context, type_gen::variant_ctor_name(variant));
  auto used = type_gen::union_string_lists(type_gen::variant_used_type_parameters(type_params, variant), phantom);
  auto type_argument_suffix = ((used.length() > 0) ? (((mlc::String("<", 1) + mlc::to_string(used.join(mlc::String(", ", 2)))) + mlc::String(">", 1))) : (mlc::String("", 0)));
  return std::make_shared<cpp_ast::CppVariantArm>(cpp_ast::CppVariantArm{(variant_name + type_argument_suffix), {}});
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_type_forward_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto phantom = type_gen::type_phantom_params_for_variants(type_params, variants);
  auto result = output;
  auto variant_index = 0;
  while ((variant_index < variants.length()))   {
    result.push_back(variant_forward_struct_cpp(context, type_params, phantom, variants[variant_index]));
    (variant_index = (variant_index + 1));
  }
  auto alias_arms = mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>>{};
  (variant_index = 0);
  while ((variant_index < variants.length()))   {
    alias_arms.push_back(variant_alias_arm_cpp(context, type_params, phantom, variants[variant_index]));
    (variant_index = (variant_index + 1));
  }
  result.push_back(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppVariant{cpp_naming::template_prefix(type_params), context::CodegenContext_resolve(context, type_name), alias_arms}));
  return result;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_fwd_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if ((variants.length() == 1))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  } else   {
    return append_type_forward_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{}, context, type_name, type_params, variants);
  }
}
std::shared_ptr<cpp_ast::CppField> record_field_cpp(context::CodegenContext context, std::shared_ptr<ast::FieldDef> field_definition) noexcept{
  return std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{type_gen::type_to_cpp(context, field_definition->type_value), cpp_naming::cpp_safe(field_definition->name)});
}
mlc::Array<std::shared_ptr<cpp_ast::CppField>> record_fields_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  return field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return record_field_cpp(context, field_definition); });
}
mlc::Array<std::shared_ptr<cpp_ast::CppField>> tuple_fields_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept{
  auto fields = mlc::Array<std::shared_ptr<cpp_ast::CppField>>{};
  auto field_index = 0;
  while ((field_index < field_types.length()))   {
    fields.push_back(std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{type_gen::type_to_cpp(context, field_types[field_index]), (mlc::String("field", 5) + mlc::to_string(field_index))}));
    (field_index = (field_index + 1));
  }
  return fields;
}
std::shared_ptr<cpp_ast::CppDeclaration> variant_body_struct_cpp(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto used = type_gen::union_string_lists(type_gen::variant_used_type_parameters(type_params, variant), phantom);
  auto prefix = cpp_naming::template_prefix(used);
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{prefix, context::CodegenContext_resolve(context, name), {}, false}); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types, __2] = varTuple; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{prefix, context::CodegenContext_resolve(context, name), tuple_fields_cpp(context, field_types), false}); },
[&](const ast::VarRecord& varRecord) { auto [name, field_definitions, __2] = varRecord; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{prefix, context::CodegenContext_resolve(context, name), record_fields_cpp(context, field_definitions), false}); }
}, (*variant));
}
std::shared_ptr<cpp_ast::CppDeclaration> cpp_using_from_struct_using_entry(context::StructUsingEntry entry) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppUsing{entry.alias, entry.type_cpp});
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> struct_using_declarations_cpp(context::CodegenContext context, mlc::String type_name) noexcept{
  auto using_entries = context.struct_using_entries.get(type_name);
  auto declarations = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto entry_index = 0;
  while ((entry_index < using_entries.length()))   {
    declarations.push_back(cpp_using_from_struct_using_entry(using_entries[entry_index]));
    (entry_index = (entry_index + 1));
  }
  return declarations;
}
std::shared_ptr<cpp_ast::CppDeclaration> single_variant_body_struct_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto prefix = cpp_naming::template_prefix(type_params);
  auto resolved_name = context::CodegenContext_resolve(context, type_name);
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{prefix, resolved_name, {}, false}); },
[&](const ast::VarTuple& varTuple) { auto [__0, field_types, __2] = varTuple; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{prefix, resolved_name, tuple_fields_cpp(context, field_types), false}); },
[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions, __2] = varRecord; return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{prefix, resolved_name, record_fields_cpp(context, field_definitions), false}); }
}, (*variant));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_type_body_struct_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if ((variants.length() == 1))   {
    auto result = output;
    result.push_back(single_variant_body_struct_cpp(context, type_name, type_params, variants[0]));
    if (context.struct_using_entries.has(type_name))     {
      (result = append_cpp_declarations(result, struct_using_declarations_cpp(context, type_name)));
    }
    return result;
  } else   {
    auto phantom = type_gen::type_phantom_params_for_variants(type_params, variants);
    auto result = output;
    auto variant_index = 0;
    while ((variant_index < variants.length()))     {
      result.push_back(variant_body_struct_cpp(context, type_params, phantom, variants[variant_index]));
      (variant_index = (variant_index + 1));
    }
    return result;
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_body_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
  auto result = append_type_body_struct_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{}, context, type_name, type_params, variants);
  auto derive_declarations = derive_methods_cpp::gen_derive_methods_cpp(type_name, variants, derive_traits);
  auto derive_index = 0;
  while ((derive_index < derive_declarations.length()))   {
    result.push_back(derive_declarations[derive_index]);
    (derive_index = (derive_index + 1));
  }
  return result;
}
mlc::String trait_template_header(mlc::Array<mlc::String> type_params) noexcept{
  if ((type_params.length() > 0))   {
    return ((mlc::String("template<typename ", 18) + type_params[0]) + mlc::String(", typename Self>\n", 17));
  } else   {
    return mlc::String("template<typename Self>\n", 24);
  }
}
mlc::String trait_requires_expression_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, params, return_type, __5, __6, __7, __8] = semanticDeclarationFn; return expr::concept_requires_expression_method_returns_convertible(cpp_naming::cpp_safe(decl_extend::trait_dispatch_name(trait_name, decl_extend::extract_method_name(mangled, trait_name))), decl_extend::concept_declval_arguments_for_params(trait_context, params), type_gen::sem_type_to_cpp(trait_context, return_type));
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<mlc::String> trait_requires_expressions_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  auto trait_context = context::CodegenContext_for_type_body(context, trait_name);
  auto expressions = mlc::Array<mlc::String>{};
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    auto expression = trait_requires_expression_for_method(trait_context, trait_name, methods[method_index]);
    if ((expression.length() > 0))     {
      expressions.push_back(expression);
    }
    (method_index = (method_index + 1));
  }
  return expressions;
}
std::shared_ptr<cpp_ast::CppDeclaration> trait_concept_dispatch_forward_proto_cpp(mlc::String trait_name, mlc::String mangled, mlc::String return_type_cpp) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnProto{mlc::String("template<typename Self>\n", 24), return_type_cpp, cpp_naming::cpp_safe(decl_extend::trait_dispatch_name(trait_name, decl_extend::extract_method_name(mangled, trait_name))), mlc::Array<mlc::String>{mlc::String("const Self& self", 16)}});
}
bool trait_skips_concept_dispatch_forward_protos(mlc::String trait_name) noexcept{
  return ((trait_name == mlc::String("ExprVisitor", 11)) || (trait_name == mlc::String("CompilerPass", 12)));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> trait_concept_dispatch_forward_protos_body(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  auto trait_context = context::CodegenContext_for_type_body(context, trait_name);
  auto forward_protos = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    if (is_semantic_declaration_fn(methods[method_index]))     {
      [&]() {
auto __match_subject = methods[method_index];
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, __5, __6, __7, __8] = semanticDeclarationFn; [&]() -> void {
  if (((parameters.length() > 0) && (ast::param_name(parameters[0]) == mlc::String("self", 4))))   {
forward_protos.push_back(trait_concept_dispatch_forward_proto_cpp(trait_name, mangled, type_gen::sem_type_to_cpp(trait_context, return_type)));

  }
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
    }
    (method_index = (method_index + 1));
  }
  return forward_protos;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> trait_concept_dispatch_forward_protos_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  if (trait_skips_concept_dispatch_forward_protos(trait_name))   {
    return empty_cpp_declarations();
  } else   {
    return trait_concept_dispatch_forward_protos_body(context, trait_name, methods);
  }
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_decl_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  if (context::trait_has_associated_types(context, trait_name))   {
    return empty_cpp_declaration();
  } else   {
    return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppConceptRequires{trait_template_header(type_params), cpp_naming::cpp_safe(trait_name), mlc::String("Self", 4), trait_requires_expressions_cpp(context, trait_name, methods)});
  }
}
std::shared_ptr<cpp_ast::CppExpression> main_set_args_vector_argument_cpp() noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(mlc::String("std::vector<mlc::String>", 24)), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("+", 1), emit_helpers::make_identifier_cpp_expression(mlc::String("argv", 4)), emit_helpers::make_integer_cpp_expression(1)}), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("+", 1), emit_helpers::make_identifier_cpp_expression(mlc::String("argv", 4)), emit_helpers::make_identifier_cpp_expression(mlc::String("argc", 4))})}});
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> main_set_args_preamble_cpp() noexcept{
  return mlc::Array{emit_helpers::make_expression_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::io::set_args", 17)), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{main_set_args_vector_argument_cpp()}}))};
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prepend_main_set_args_preamble(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
  auto result = main_set_args_preamble_cpp();
  auto statement_index = 0;
  while ((statement_index < statements.length()))   {
    result.push_back(statements[statement_index]);
    (statement_index = (statement_index + 1));
  }
  return result;
}
mlc::String function_declaration_template_prefix(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept{
  return (cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds));
}
mlc::Array<mlc::String> merged_function_type_parameters_cpp(mlc::Array<mlc::String> type_params, semantic_ir::FnEscapeInfo escape_info) noexcept{
  return type_params.concat(escape_info.synthetic_type_params);
}
bool function_emits_template_cpp(mlc::Array<mlc::String> type_params, semantic_ir::FnEscapeInfo escape_info) noexcept{
  return (merged_function_type_parameters_cpp(type_params, escape_info).length() > 0);
}
context::CodegenContext context_with_fn_escape_cpp(context::CodegenContext context, semantic_ir::FnEscapeInfo escape_info) noexcept{
  return context::CodegenContext_with_param_template_type_names(context, escape_info.param_template_type_names);
}
std::shared_ptr<cpp_ast::CppDeclaration> native_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
  return native_fn_proto_cpp_with_escape(name, type_params, type_bounds, params, return_type, semantic_ir::empty_fn_escape_info(), context);
}
std::shared_ptr<cpp_ast::CppDeclaration> native_fn_proto_cpp_with_escape(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context) noexcept{
  auto escape_context = context_with_fn_escape_cpp(context, escape_info);
  auto prototype_context = decl::prototype_context_for_function(escape_context, params);
  auto safe_name = context::CodegenContext_resolve(escape_context, name);
  auto return_type_cpp = type_gen::sem_type_to_cpp(prototype_context, return_type);
  auto parameters = function_parameter_proto_items(name, params, prototype_context);
  auto all_type_params = merged_function_type_parameters_cpp(type_params, escape_info);
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnProto{(cpp_naming::template_prefix(all_type_params) + type_gen::requires_clause(type_params, type_bounds)), return_type_cpp, safe_name, parameters});
}
std::shared_ptr<cpp_ast::CppDeclaration> native_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context, int body_statement_depth) noexcept{
  return native_fn_decl_cpp_with_escape(name, type_params, type_bounds, params, return_type, body, semantic_ir::empty_fn_escape_info(), context, body_statement_depth);
}
std::shared_ptr<cpp_ast::CppDeclaration> native_fn_decl_cpp_with_escape(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context, int body_statement_depth) noexcept{
  auto escape_context = context_with_fn_escape_cpp(context, escape_info);
  auto body_context = context::CodegenContext_with_enclosing_function_return_type(decl::compute_fn_body_context(name, params, escape_context), return_type);
  auto prototype_context = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? (body_context) : (escape_context));
  auto safe_name = context::CodegenContext_resolve(escape_context, name);
  auto return_type_cpp = type_gen::sem_type_to_cpp(prototype_context, return_type);
  auto parameters = function_parameter_def_items(name, params, prototype_context);
  auto return_body_statements = return_body::gen_return_body_cpp(body, body_context);
  auto body_statements = (((name == mlc::String("main", 4)) && (params.length() == 0)) ? (prepend_main_set_args_preamble(return_body_statements)) : (return_body_statements));
  auto all_type_params = merged_function_type_parameters_cpp(type_params, escape_info);
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{(cpp_naming::template_prefix(all_type_params) + type_gen::requires_clause(type_params, type_bounds)), return_type_cpp, safe_name, parameters, body_statements, body_statement_depth});
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
  return native_fn_proto_cpp(name, type_params, type_bounds, params, return_type, context);
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept{
  return native_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, context, 1);
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_proto_cpp_with_escape(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context) noexcept{
  return native_fn_proto_cpp_with_escape(name, type_params, type_bounds, params, return_type, escape_info, context);
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_decl_cpp_with_escape(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context) noexcept{
  return native_fn_decl_cpp_with_escape(name, type_params, type_bounds, params, return_type, body, escape_info, context, 1);
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_decl_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return CodegenContext_gen_decl_cpp(context, declaration);
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_proto_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return CodegenContext_gen_proto_cpp(context, declaration);
}
std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_decl_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_proto_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_decl_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [name, type_params, type_bounds, params, return_type, body, __6, escape_info, __8] = semanticDeclarationFn; return (semantic_expression_is_bare_extern(body) ? (empty_cpp_declaration()) : ((semantic_expression_is_ffi_extern(body) ? (gen_ffi_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, escape_info, self)) : (gen_fn_decl_cpp_with_escape(name, type_params, type_bounds, params, return_type, body, escape_info, self))))); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [type_name, type_params, variants, derive_traits, __4] = semanticDeclarationType; return cpp_decl_from_native_declarations(append_cpp_declarations(gen_type_decl_fwd_cpp(self, type_name, type_params, variants), gen_type_decl_body_cpp(self, type_name, type_params, variants, derive_traits))); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [name, type_params, methods, __3] = semanticDeclarationTrait; return gen_trait_decl_cpp(self, name, type_params, methods); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return cpp_decl_from_native_declarations(gen_decl_extend_cpp(type_name, trait_name, methods, self)); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return CodegenContext_gen_decl_cpp(self, semantic_ir::sdecl_inner(inner_declaration)); }
}, (*declaration));
}
std::shared_ptr<cpp_ast::CppDeclaration> CodegenContext_gen_proto_cpp(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [name, type_params, type_bounds, params, return_type, body, __6, escape_info, __8] = semanticDeclarationFn; return (semantic_expression_is_bare_extern(body) ? (empty_cpp_declaration()) : ((semantic_expression_is_ffi_extern(body) ? (gen_ffi_fn_proto_cpp(name, type_params, type_bounds, params, return_type, escape_info, self)) : (gen_fn_proto_cpp_with_escape(name, type_params, type_bounds, params, return_type, escape_info, self))))); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [__0, __1, __2, __3, __4] = semanticDeclarationType; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [__0, __1, __2, __3] = semanticDeclarationTrait; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return [&]() -> std::shared_ptr<cpp_ast::CppDeclaration> {
  if (context::trait_has_associated_types(self, registry::trait_base_name(trait_name)))   {
    return empty_cpp_declaration();
  } else   {
    return [&]() {
auto extend_context = context::CodegenContext_for_type_body(self, type_name);
auto proto_declarations = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
auto method_index = 0;
while ((method_index < methods.length())) {
  if ((is_semantic_declaration_fn(methods[method_index]) && (!semantic_fn_body_is_extern(methods[method_index]))))   {
    proto_declarations.push_back(CodegenContext_gen_proto_cpp(extend_context, methods[method_index]));
  }
  (method_index = (method_index + 1));
}
return cpp_decl_from_native_declarations(proto_declarations);
}();
  }
}(); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declaration(); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return CodegenContext_gen_proto_cpp(self, semantic_ir::sdecl_inner(inner_declaration)); }
}, (*declaration));
}
bool is_semantic_declaration_fn(std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> bool {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [__0, __1, __2, __3, __4, __5, __6, __7, __8] = semanticDeclarationFn; return true;
}
return false;
std::abort();
}();
}
bool semantic_expression_is_extern(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionExtern; return true;
}
return false;
std::abort();
}();
}
mlc::String semantic_extern_ffi_c_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [__0, extern_c_name, __2, __3] = semanticExpressionExtern; return extern_c_name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String semantic_extern_ffi_header(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [__0, __1, extern_header, __3] = semanticExpressionExtern; return extern_header;
}
return mlc::String("", 0);
std::abort();
}();
}
bool semantic_expression_is_ffi_extern(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return (semantic_extern_ffi_c_name(expression).length() > 0);
}
bool semantic_expression_is_bare_extern(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return (semantic_expression_is_extern(expression) && (!semantic_expression_is_ffi_extern(expression)));
}
mlc::String ffi_header_include_line(mlc::String extern_header) noexcept{
  if ((((extern_header.length() >= 2) && (extern_header.char_at(0) == mlc::String("<", 1))) && (extern_header.char_at((extern_header.length() - 1)) == mlc::String(">", 1))))   {
    return ((mlc::String("#include <", 10) + extern_header.substring(1, (extern_header.length() - 2))) + mlc::String(">\n", 2));
  } else if ((extern_header.length() > 0))   {
    return ((mlc::String("#include \"", 10) + extern_header) + mlc::String("\"\n", 2));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String collect_ffi_include_lines_from_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [__0, __1, __2, __3, __4, body, __6, __7, __8] = semanticDeclarationFn; return (semantic_expression_is_ffi_extern(body) ? (ffi_header_include_line(semantic_extern_ffi_header(body))) : (mlc::String("", 0)));
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner_declaration] = semanticDeclarationExported; return collect_ffi_include_lines_from_declaration(semantic_ir::sdecl_inner(inner_declaration));
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String collect_ffi_include_lines(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
  auto include_text = mlc::String("", 0);
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    (include_text = (include_text + collect_ffi_include_lines_from_declaration(declarations[declaration_index])));
    (declaration_index = (declaration_index + 1));
  }
  return include_text;
}
mlc::Array<mlc::String> ffi_parameter_type_items(mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept{
  auto type_items = mlc::Array<mlc::String>{};
  auto parameter_index = 0;
  while ((parameter_index < params.length()))   {
    type_items.push_back(type_gen::type_to_cpp(context, params[parameter_index]->type_value));
    (parameter_index = (parameter_index + 1));
  }
  return type_items;
}
mlc::Array<mlc::String> ffi_parameter_name_items(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
  auto name_items = mlc::Array<mlc::String>{};
  auto parameter_index = 0;
  while ((parameter_index < params.length()))   {
    name_items.push_back(cpp_naming::cpp_safe(params[parameter_index]->name));
    (parameter_index = (parameter_index + 1));
  }
  return name_items;
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_ffi_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context) noexcept{
  return gen_fn_proto_cpp_with_escape(name, type_params, type_bounds, params, return_type, escape_info, context);
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_ffi_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context) noexcept{
  auto escape_context = context_with_fn_escape_cpp(context, escape_info);
  auto body_context = context::CodegenContext_with_enclosing_function_return_type(decl::compute_fn_body_context(name, params, escape_context), return_type);
  auto prototype_context = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? (body_context) : (escape_context));
  auto safe_name = context::CodegenContext_resolve(escape_context, name);
  auto return_type_cpp = type_gen::sem_type_to_cpp(prototype_context, return_type);
  auto parameters = function_parameter_def_items(name, params, prototype_context);
  auto extern_c_name = semantic_extern_ffi_c_name(body);
  auto binder_name = (mlc::String("mlc_ffi_bind_", 13) + cpp_naming::cpp_safe(name));
  auto signature_params = ffi_parameter_type_items(params, prototype_context).join(mlc::String(", ", 2));
  auto call_arguments = ffi_parameter_name_items(params).join(mlc::String(", ", 2));
  auto binder_statement = emit_helpers::make_fragment_cpp_statement(((((((((mlc::String("static auto const ", 18) + binder_name) + mlc::String(" = static_cast<", 15)) + return_type_cpp) + mlc::String("(*)(", 4)) + signature_params) + mlc::String(")>(&::", 6)) + extern_c_name) + mlc::String(");", 2)));
  auto call_expression = ((call_arguments.length() == 0) ? ((binder_name + mlc::String("()", 2))) : ((((binder_name + mlc::String("(", 1)) + call_arguments) + mlc::String(")", 1))));
  auto return_statement = emit_helpers::make_fragment_cpp_statement(((mlc::String("return ", 7) + call_expression) + mlc::String(";", 1)));
  auto all_type_params = merged_function_type_parameters_cpp(type_params, escape_info);
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{(cpp_naming::template_prefix(all_type_params) + type_gen::requires_clause(type_params, type_bounds)), return_type_cpp, safe_name, parameters, mlc::Array{binder_statement, return_statement}, 1});
}
bool semantic_fn_body_is_extern(std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> bool {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [__0, __1, __2, __3, __4, body, __6, __7, __8] = semanticDeclarationFn; return semantic_expression_is_extern(body);
}
return false;
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_protos_for_declaration_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [__0, __1, methods, __3] = semanticDeclarationExtend; return [&]() {
auto result = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
auto method_index = 0;
while ((method_index < methods.length())) {
  if ((!semantic_fn_body_is_extern(methods[method_index])))   {
    result.push_back(gen_proto_cpp(methods[method_index], context));
  }
  (method_index = (method_index + 1));
}
return result;
}();
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner] = semanticDeclarationExported; return extend_forward_protos_for_declaration_cpp(inner, context);
}
return [&]() {
auto empty = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
return empty;
}();
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_method_forward_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  return declarations.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_forward_protos_for_declaration_cpp(declaration, context); });
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_helper_protos_for_declaration_cpp(mlc::Array<mlc::String> exported_types, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [type_name, __1, methods, __3] = semanticDeclarationExtend; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
  if (decl_index::list_contains(exported_types, type_name))   {
    return [&]() {
auto result = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
auto method_index = 0;
while ((method_index < methods.length())) {
  if ((!semantic_fn_body_is_extern(methods[method_index])))   {
    result.push_back(gen_proto_cpp(methods[method_index], context));
  }
  (method_index = (method_index + 1));
}
return result;
}();
  } else   {
    return [&]() {
auto empty = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
return empty;
}();
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner] = semanticDeclarationExported; return extend_helper_protos_for_declaration_cpp(exported_types, inner, context);
}
return [&]() {
auto empty = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
return empty;
}();
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_helper_protos_for_exported_types_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  auto exported_types = decl::exported_type_names_from_decls(declarations);
  return declarations.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_helper_protos_for_declaration_cpp(exported_types, declaration, context); });
}
mlc::String semantic_declaration_fn_return_type_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> method, context::CodegenContext extend_context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [__0, __1, __2, __3, return_type, __5, __6, __7, __8] = semanticDeclarationFn; return type_gen::sem_type_to_cpp(extend_context, return_type);
}
return mlc::String("void", 4);
std::abort();
}();
}
mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    if (is_semantic_declaration_fn(methods[method_index]))     {
      auto return_type_cpp = semantic_declaration_fn_return_type_cpp(methods[method_index], extend_context);
      if ((return_type_cpp != mlc::String("void", 4)))       {
        return return_type_cpp;
      }
    }
    (method_index = (method_index + 1));
  }
  return mlc::String("void", 4);
}
mlc::String extend_forward_call_argument_list(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
  auto argument_names = mlc::Array<mlc::String>{};
  auto parameter_index = 0;
  while ((parameter_index < params.length()))   {
    argument_names.push_back(cpp_naming::cpp_safe(params[parameter_index]->name));
    (parameter_index = (parameter_index + 1));
  }
  return argument_names.join(mlc::String(", ", 2));
}
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> extend_forward_call_argument_expressions(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
  auto expressions = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{};
  auto parameter_index = 0;
  while ((parameter_index < params.length()))   {
    expressions.push_back(emit_helpers::make_identifier_cpp_expression(cpp_naming::cpp_safe(params[parameter_index]->name)));
    (parameter_index = (parameter_index + 1));
  }
  return expressions;
}
std::shared_ptr<cpp_ast::CppDeclaration> native_inline_noexcept_forwarding_fn_cpp(mlc::String return_type_cpp, mlc::String method_name_cpp, mlc::Array<mlc::String> parameter_items, mlc::String callee_code, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> call_arguments) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{mlc::String("inline ", 7), return_type_cpp, method_name_cpp, parameter_items, mlc::Array{emit_helpers::make_return_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(callee_code), call_arguments}))}, (-1)});
}
std::shared_ptr<cpp_ast::CppDeclaration> native_noexcept_one_line_returning_fn_cpp(mlc::String return_type_cpp, mlc::String name_cpp, mlc::Array<mlc::String> parameter_items, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{mlc::String("", 0), return_type_cpp, name_cpp, parameter_items, mlc::Array{emit_helpers::make_return_cpp_statement(return_expression)}, (-1)});
}
mlc::String trait_concept_cpp_name_for_static_assert(mlc::String trait_name) noexcept{
  if ((trait_name == mlc::String("ExprVisitor", 11)))   {
    return mlc::String("expr_visitor::ExprVisitor", 25);
  } else if ((trait_name == mlc::String("CompilerPass", 12)))   {
    return mlc::String("pass::CompilerPass", 18);
  } else   {
    return cpp_naming::cpp_safe(trait_name);
  }
}
std::shared_ptr<cpp_ast::CppDeclaration> extend_trait_static_assert_decl_cpp(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept{
  auto trait_safe = trait_concept_cpp_name_for_static_assert(trait_name);
  auto message = ((type_name + mlc::String(" does not implement ", 20)) + trait_name);
  if ((trait_name == mlc::String("ExprVisitor", 11)))   {
    auto result_type_cpp = extend_method_return_type_cpp(extend_context, methods);
    return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStaticAssert{trait_safe, mlc::Array<mlc::String>{result_type_cpp, implementor_type_cpp}, message});
  } else   {
    return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStaticAssert{trait_safe, mlc::Array<mlc::String>{implementor_type_cpp}, message});
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_trait_wrapper_for_method_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> method, mlc::String type_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
  if ((!is_semantic_declaration_fn(method)))   {
    return empty_cpp_declarations();
  } else   {
    return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, params, return_type, __5, __6, __7, __8] = semanticDeclarationFn; return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{extend_trait_wrapper_decl_cpp(type_name, mangled, params, return_type, context, context::context_resolve)};
}
return empty_cpp_declarations();
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_trait_wrappers_only_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
  auto wrapper_only = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    (wrapper_only = append_cpp_declarations(wrapper_only, extend_trait_wrapper_for_method_cpp(methods[method_index], type_name, context, context::context_resolve)));
    (method_index = (method_index + 1));
  }
  return wrapper_only;
}
bool semantic_declaration_is_assoc_bind(std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> bool {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject))) {
const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind = std::get<semantic_ir::SemanticDeclarationAssocBind>((*__match_subject));
auto [__0, __1, __2] = semanticDeclarationAssocBind; return true;
}
return false;
std::abort();
}();
}
bool extend_methods_have_assoc_bind(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    if (semantic_declaration_is_assoc_bind(methods[method_index]))     {
      return true;
    }
    (method_index = (method_index + 1));
  }
  return false;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_trait_concept_method_suffix_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> method, mlc::String type_name, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
  if ((!is_semantic_declaration_fn(method)))   {
    return empty_cpp_declarations();
  } else   {
    return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, type_params, type_bounds, params, return_type, body, __6, __7, __8] = semanticDeclarationFn; return [&]() {
auto extend_context = context::CodegenContext_for_type_body(context, type_name);
auto method_suffix = empty_cpp_declarations();
if (semantic_expression_is_extern(body)) {
  method_suffix.push_back(gen_fn_proto_cpp(mangled, type_params, type_bounds, params, return_type, extend_context));
}
return append_cpp_declarations(append_cpp_declarations(method_suffix, mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{extend_trait_wrapper_decl_cpp(type_name, mangled, params, return_type, context, context::context_resolve)}), mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{extend_trait_concept_adapter_decl_cpp(type_name, trait_name, mangled, params, return_type, context, context::context_resolve)});
}();
}
return empty_cpp_declarations();
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_trait_concept_suffix_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
  auto extend_context = context::CodegenContext_for_type_body(context, type_name);
  auto implementor_type_cpp = type_gen::type_name_to_cpp(context, type_name);
  auto result = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    (result = append_cpp_declarations(result, extend_trait_concept_method_suffix_cpp(methods[method_index], type_name, trait_name, context, context::context_resolve)));
    (method_index = (method_index + 1));
  }
  result.push_back(extend_trait_static_assert_decl_cpp(trait_name, type_name, methods, extend_context, implementor_type_cpp));
  return result;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_trait_suffix_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
  auto bare_trait_name = registry::trait_base_name(trait_name);
  if (context::trait_has_associated_types(context, bare_trait_name))   {
    return gen_decl_extend_trait_wrappers_only_cpp(type_name, methods, context, context::context_resolve);
  } else   {
    return gen_decl_extend_trait_concept_suffix_cpp(type_name, bare_trait_name, methods, context, context::context_resolve);
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_method_definition_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> method, mlc::String type_name, mlc::String trait_name, context::CodegenContext context, context::CodegenContext extend_context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept{
  if ((!is_semantic_declaration_fn(method)))   {
    return empty_cpp_declarations();
  } else   {
    return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, type_params, type_bounds, params, return_type, body, __6, __7, __8] = semanticDeclarationFn; return (semantic_expression_is_extern(body) ? (gen_extend_extern_method_cpp(mangled, type_name, params, return_type, trait_name, context, context::context_resolve)) : (mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{native_fn_decl_cpp(mangled, type_params, type_bounds, params, return_type, body, extend_context, 0)}));
}
return empty_cpp_declarations();
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_methods_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
  auto extend_context = context::CodegenContext_for_type_body(context, type_name);
  auto context_resolve = [=](auto codegen_context, auto name) mutable { return codegen_context.resolve(name); };
  auto result = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto method_index = 0;
  while ((method_index < methods.length()))   {
    (result = append_cpp_declarations(result, extend_method_definition_cpp(methods[method_index], type_name, trait_name, context, extend_context, context::context_resolve)));
    (method_index = (method_index + 1));
  }
  return result;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
  auto bare_trait_name = registry::trait_base_name(trait_name);
  auto context_resolve = [=](auto codegen_context, auto name) mutable { return codegen_context.resolve(name); };
  auto result = gen_decl_extend_methods_cpp(type_name, bare_trait_name, methods, context);
  if ((bare_trait_name.length() > 0))   {
    (result = append_cpp_declarations(result, gen_decl_extend_trait_suffix_cpp(type_name, bare_trait_name, methods, context, context::context_resolve)));
  }
  return result;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_wrapper_proto_for_method_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> method, mlc::String type_name, context::CodegenContext context) noexcept{
  if ((!is_semantic_declaration_fn(method)))   {
    return empty_cpp_declarations();
  } else   {
    return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, params, return_type, __5, __6, __7, __8] = semanticDeclarationFn; return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnProto{mlc::String("", 0), type_gen::sem_type_to_cpp(context::CodegenContext_for_type_body(context, type_name), return_type), cpp_naming::cpp_safe(decl_extend::extract_method_name(mangled, type_name)), decl_extend::gen_parameter_proto_items(context::CodegenContext_for_type_body(context, type_name), params)})};
}
return empty_cpp_declarations();
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_extend_wrapper_protos_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
  if ((trait_name.length() == 0))   {
    return empty_cpp_declarations();
  } else   {
    auto wrappers = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
    auto method_index = 0;
    while ((method_index < methods.length()))     {
      (wrappers = append_cpp_declarations(wrappers, extend_wrapper_proto_for_method_cpp(methods[method_index], type_name, context)));
      (method_index = (method_index + 1));
    }
    return wrappers;
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_proto_for_fn_method(std::shared_ptr<semantic_ir::SemanticDeclaration> method, context::CodegenContext extend_context, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept{
  if (semantic_expression_is_bare_extern(body))   {
    return empty_cpp_declarations();
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(method, extend_context)};
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_proto_for_method(std::shared_ptr<semantic_ir::SemanticDeclaration> method, context::CodegenContext extend_context) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, __1, __2, __3, __4, body, __6, __7, __8] = semanticDeclarationFn; return extend_forward_proto_for_fn_method(method, extend_context, body); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [__0, __1, __2, __3, __4] = semanticDeclarationType; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [__0, __1, __2, __3] = semanticDeclarationTrait; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [__0, __1, __2, __3] = semanticDeclarationExtend; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return extend_forward_proto_for_method(semantic_ir::sdecl_inner(inner_declaration), extend_context); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declarations(); }
}, (*method));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_forward_segment_for_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext codegen_context) noexcept{
  auto extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
  auto result = gen_extend_wrapper_protos_cpp(type_name, trait_name, methods, codegen_context);
  if ((!context::trait_has_associated_types(codegen_context, trait_name)))   {
    auto method_index = 0;
    [&]() {
while ((method_index < methods.length())) {
(result = append_cpp_declarations(result, extend_forward_proto_for_method(methods[method_index], extend_context)));
(method_index = (method_index + 1));
}
}();
  }
  return result;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_cpp_forward_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return extend_forward_segment_for_extend(type_name, trait_name, methods, codegen_context); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return extend_forward_segment_for_extend(type_name, trait_name, methods, codegen_context); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [__0, __1, __2, __3, __4] = semanticDeclarationType; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [__0, __1, __2, __3] = semanticDeclarationTrait; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, __1, __2, __3, __4, __5, __6, __7, __8] = semanticDeclarationFn; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [__0] = semanticDeclarationExported; return empty_cpp_declarations(); }
}, (*semantic_ir::sdecl_inner(inner_declaration))); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [__0, __1, __2, __3, __4] = semanticDeclarationType; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [__0, __1, __2, __3] = semanticDeclarationTrait; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, __1, __2, __3, __4, __5, __6, __7, __8] = semanticDeclarationFn; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declarations(); }
}, (*declaration));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_type_cpp(mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits, context::CodegenContext context, int phase) noexcept{
  if ((phase == 0))   {
    return gen_type_decl_fwd_cpp(context, type_name, type_params, variants);
  } else if ((phase == 1))   {
    return gen_type_decl_body_cpp(context, type_name, type_params, variants, derive_traits);
  } else   {
    return empty_cpp_declarations();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_trait_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, int phase) noexcept{
  if (context::trait_has_associated_types(context, name))   {
    return empty_cpp_declarations();
  } else if ((phase == 0))   {
    return append_cpp_declarations(trait_concept_dispatch_forward_protos_cpp(context, name, methods), mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_trait_decl_cpp(context, name, type_params, methods)});
  } else if ((phase == 4))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{trait_struct_cpp::gen_trait_struct_cpp(context, name, type_params, methods)};
  } else   {
    return empty_cpp_declarations();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_fn_phase_two_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept{
  if (semantic_expression_is_bare_extern(body))   {
    return empty_cpp_declarations();
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(declaration, context)};
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_exported_fn_phase_two_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept{
  if (semantic_expression_is_extern(body))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(inner_declaration, context)};
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_proto_cpp(inner_declaration, context)};
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_exported_fn_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context, int phase) noexcept{
  if ((phase == 2))   {
    return decl_segment_exported_fn_phase_two_cpp(inner_declaration, body, context);
  } else   {
    return empty_cpp_declarations();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_for_extend_declaration_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context, int phase) noexcept{
  if ((phase == 2))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{CodegenContext_gen_proto_cpp(context, declaration)};
  } else   {
    return empty_cpp_declarations();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_for_exported_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, context::CodegenContext context, int phase) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [__0, __1, __2, __3] = semanticDeclarationExtend; return decl_segment_for_extend_declaration_cpp(inner_declaration, context, phase); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [type_name, type_params, variants, derive_traits, __4] = semanticDeclarationType; return decl_segment_type_cpp(type_name, type_params, variants, derive_traits, context, phase); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [name, type_params, methods, __3] = semanticDeclarationTrait; return decl_segment_trait_cpp(name, type_params, methods, context, phase); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [nested_inner] = semanticDeclarationExported; return decl_segment_for_exported_cpp(nested_inner, context, phase); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, __1, __2, __3, __4, body, __6, __7, __8] = semanticDeclarationFn; return decl_segment_exported_fn_segment_cpp(inner_declaration, body, context, phase); }
}, (*semantic_ir::sdecl_inner(inner_declaration)));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_native_fn_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context, int phase) noexcept{
  if ((phase == 2))   {
    return decl_segment_fn_phase_two_cpp(declaration, body, context);
  } else   {
    return empty_cpp_declarations();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context, int phase) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [__0, __1, __2, __3] = semanticDeclarationExtend; return decl_segment_for_extend_declaration_cpp(declaration, context, phase); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [type_name, type_params, variants, derive_traits, __4] = semanticDeclarationType; return decl_segment_type_cpp(type_name, type_params, variants, derive_traits, context, phase); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [name, type_params, methods, __3] = semanticDeclarationTrait; return decl_segment_trait_cpp(name, type_params, methods, context, phase); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return decl_segment_for_exported_cpp(inner_declaration, context, phase); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, __1, __2, __3, __4, body, __6, __7, __8] = semanticDeclarationFn; return decl_segment_native_fn_segment_cpp(declaration, body, context, phase); }
}, (*declaration));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_native_decl_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept{
  auto output = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    (output = append_cpp_declarations(output, decl_segment_cpp(declarations[declaration_index], context, phase)));
    (declaration_index = (declaration_index + 1));
  }
  return output;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_trait_suffix_header_for_declaration_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return [&]() {
auto bare_trait_name = registry::trait_base_name(trait_name);
return (((bare_trait_name.length() > 0) && (!context::trait_has_associated_types(context, bare_trait_name))) ? (gen_decl_extend_trait_suffix_cpp(type_name, bare_trait_name, methods, context, [=](context::CodegenContext codegen_context, mlc::String name) mutable { return context::CodegenContext_resolve(codegen_context, name); })) : (empty_cpp_declarations()));
}();
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner_declaration] = semanticDeclarationExported; return extend_trait_suffix_header_for_declaration_cpp(semantic_ir::sdecl_inner(inner_declaration), context);
}
return empty_cpp_declarations();
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_extend_trait_suffix_header_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  return declarations.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_trait_suffix_header_for_declaration_cpp(declaration, context); });
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> template_fn_body_for_declaration_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [__0, type_params, __2, __3, __4, body, __6, escape_info, __8] = semanticDeclarationFn; return (semantic_expression_is_bare_extern(body) ? (empty_cpp_declarations()) : ((function_emits_template_cpp(type_params, escape_info) ? (mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_decl_cpp(declaration, context)}) : (empty_cpp_declarations()))));
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner_declaration] = semanticDeclarationExported; return template_fn_body_for_declaration_cpp(semantic_ir::sdecl_inner(inner_declaration), context);
}
return empty_cpp_declarations();
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_template_fn_bodies_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  return declarations.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return template_fn_body_for_declaration_cpp(declaration, context); });
}
DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  auto fn_protos = collect_decl_parts_cpp(declarations, context, 2);
  (fn_protos = append_cpp_declarations(fn_protos, extend_helper_protos_for_exported_types_cpp(declarations, context)));
  (fn_protos = append_cpp_declarations(fn_protos, collect_extend_trait_suffix_header_cpp(declarations, context)));
  (fn_protos = append_cpp_declarations(fn_protos, collect_template_fn_bodies_cpp(declarations, context)));
  return DeclPartsBundleCpp{collect_decl_parts_cpp(declarations, context, 0), collect_decl_parts_cpp(declarations, context, 1), fn_protos, collect_fn_defs_cpp(declarations, context)};
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept{
  if (((((phase == 0) || (phase == 1)) || (phase == 2)) || (phase == 4)))   {
    return collect_native_decl_segments_cpp(declarations, context, phase);
  } else if ((phase == 3))   {
    return collect_fn_defs_cpp(declarations, context);
  } else   {
    return empty_cpp_declarations();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty_cpp_declarations() noexcept{
  auto empty = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  return empty;
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> append_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> output, mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
  auto index = 0;
  while ((index < declarations.length()))   {
    output.push_back(declarations[index]);
    (index = (index + 1));
  }
  return output;
}
std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_native_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
  if ((declarations.length() == 0))   {
    return empty_cpp_declaration();
  } else if ((declarations.length() == 1))   {
    return declarations[0];
  } else   {
    return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDeclarationSequence{declarations});
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_exported_fn_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context) noexcept{
  if (function_emits_template_cpp(type_parameters, escape_info))   {
    return empty_cpp_declarations();
  } else if (semantic_expression_is_bare_extern(body))   {
    return empty_cpp_declarations();
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_decl_cpp(inner_declaration, context)};
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_extend_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
  auto bare_trait_name = registry::trait_base_name(trait_name);
  if (((bare_trait_name.length() > 0) && context::trait_has_associated_types(context, bare_trait_name)))   {
    return gen_decl_extend_cpp(type_name, bare_trait_name, methods, context);
  } else   {
    return gen_decl_extend_methods_cpp(type_name, bare_trait_name, methods, context);
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_for_exported_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, context::CodegenContext context) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return decl_fn_def_extend_cpp(type_name, trait_name, methods, context); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [__0, __1, __2, __3, __4] = semanticDeclarationType; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [__0, __1, __2, __3] = semanticDeclarationTrait; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, type_parameters, __2, __3, __4, body, __6, escape_info, __8] = semanticDeclarationFn; return decl_fn_def_exported_fn_cpp(inner_declaration, type_parameters, body, escape_info, context); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [nested_inner] = semanticDeclarationExported; return decl_fn_def_for_exported_cpp(nested_inner, context); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declarations(); }
}, (*semantic_ir::sdecl_inner(inner_declaration)));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_native_fn_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept{
  return decl_fn_def_native_fn_cpp_with_escape(name, type_params, type_bounds, params, return_type, body, semantic_ir::empty_fn_escape_info(), context);
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_native_fn_cpp_with_escape(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, semantic_ir::FnEscapeInfo escape_info, context::CodegenContext context) noexcept{
  if (semantic_expression_is_bare_extern(body))   {
    return empty_cpp_declarations();
  } else if (function_emits_template_cpp(type_params, escape_info))   {
    return empty_cpp_declarations();
  } else if (semantic_expression_is_ffi_extern(body))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_ffi_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, escape_info, context)};
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{gen_fn_decl_cpp_with_escape(name, type_params, type_bounds, params, return_type, body, escape_info, context)};
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_fn_def_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return decl_fn_def_extend_cpp(type_name, trait_name, methods, context); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return decl_fn_def_for_exported_cpp(inner_declaration, context); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [__0, __1, __2, __3, __4] = semanticDeclarationType; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [__0, __1, __2, __3] = semanticDeclarationTrait; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationExternLib& semanticDeclarationExternLib) { auto [__0, __1] = semanticDeclarationExternLib; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return empty_cpp_declarations(); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [name, type_params, type_bounds, params, return_type, body, __6, escape_info, __8] = semanticDeclarationFn; return decl_fn_def_native_fn_cpp_with_escape(name, type_params, type_bounds, params, return_type, body, escape_info, context); }
}, (*declaration));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_fn_defs_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  auto output = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  (output = append_cpp_declarations(output, extend_method_forward_segments_cpp(declarations, context)));
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    auto declaration = declarations[declaration_index];
    (output = append_cpp_declarations(output, extend_cpp_forward_segment_cpp(declaration, context)));
    auto definition_segments = decl_fn_def_cpp(declaration, context);
    auto segment_index = 0;
    while ((segment_index < definition_segments.length()))     {
      output.push_back(definition_segments[segment_index]);
      (segment_index = (segment_index + 1));
    }
    (output = append_cpp_declarations(output, collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{declaration}, context, 4)));
    (declaration_index = (declaration_index + 1));
  }
  return output;
}

} // namespace decl_cpp
