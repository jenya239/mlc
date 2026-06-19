#include "derive_methods_cpp.hpp"

#include "ast.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "hash_stmt_helpers.hpp"

namespace derive_methods_cpp {

using namespace ast;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace hash_stmt_helpers;
using namespace ast_tokens;

std::shared_ptr<cpp_ast::CppExpression> make_string_constructor(mlc::String text) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_mlc_to_string_call(std::shared_ptr<cpp_ast::CppExpression> argument) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_self_field_expression(mlc::String field_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_self_discriminant_expression() noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_std_get_expression(mlc::String variant_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_holds_alternative_expression(mlc::String variant_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_field_display_expression(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_variant_field_display_expression(mlc::String variant_name, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> fold_plus_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_display_field_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_display_record_return_expression(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_variant_record_display_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_record_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_display_tuple_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_return_expression(std::shared_ptr<ast::TypeVariant> variant) noexcept;

std::shared_ptr<cpp_ast::CppStatement> display_to_string_if_statement(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_display_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> display_to_string_function_definition(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_display_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_display_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_definitions(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_display_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_parameter_field_expression(mlc::String parameter_name, mlc::String field_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_parameter_discriminant_expression(mlc::String parameter_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> fold_and_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> build_eq_field_comparisons(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_eq_sum_return_expression() noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> eq_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_sum_cpp(mlc::String type_name) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_eq_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_field_less_than_expression(mlc::String field_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_field_equal_expression(mlc::String field_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> build_prefix_equal_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int up_to_index) noexcept;

std::shared_ptr<cpp_ast::CppExpression> build_ord_record_return_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppExpression> derive_ord_sum_return_expression() noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> ord_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_sum_cpp(mlc::String type_name) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_ord_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_hash_tuple_field_access_expression(mlc::String variant_name, int field_index) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_hash_variant_field_access_expression(mlc::String variant_name, mlc::String field_name) noexcept;

std::shared_ptr<cpp_ast::CppStatement> make_hash_combine_statement(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept;

std::shared_ptr<cpp_ast::CppStatement> make_hash_seed_declaration_statement() noexcept;

std::shared_ptr<cpp_ast::CppStatement> make_hash_empty_return_statement() noexcept;

std::shared_ptr<cpp_ast::CppStatement> make_hash_return_h_statement() noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_record_body_statements(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

std::shared_ptr<cpp_ast::CppStatement> derive_hash_discriminant_combine_statement(int discriminant_index) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_tuple_field_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_record_variant_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int discriminant_index) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_variant_body_statements(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;

std::shared_ptr<cpp_ast::CppStatement> derive_hash_sum_branch_statement(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> hash_specialization_declaration(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_hash_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_trait_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_methods_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_string_constructor(mlc::String text) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::String")), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{emit_helpers::make_string_cpp_expression(text)}));}

std::shared_ptr<cpp_ast::CppExpression> make_mlc_to_string_call(std::shared_ptr<cpp_ast::CppExpression> argument) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::to_string")), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{argument}));}

std::shared_ptr<cpp_ast::CppExpression> make_self_field_expression(mlc::String field_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(emit_helpers::make_identifier_cpp_expression(mlc::String("self")), field_name, false));}

std::shared_ptr<cpp_ast::CppExpression> make_self_discriminant_expression() noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(emit_helpers::make_identifier_cpp_expression(mlc::String("self")), mlc::String("_"), false));}

std::shared_ptr<cpp_ast::CppExpression> make_std_get_expression(mlc::String variant_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("std::get<") + variant_name + mlc::String(">")), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{make_self_discriminant_expression()}));}

std::shared_ptr<cpp_ast::CppExpression> make_holds_alternative_expression(mlc::String variant_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("std::holds_alternative<") + variant_name + mlc::String(">")), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{make_self_discriminant_expression()}));}

std::shared_ptr<cpp_ast::CppExpression> derive_field_display_expression(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<ast::TyString>((*field_type))) {  return make_self_field_expression(field_name); } return make_mlc_to_string_call(make_self_field_expression(field_name)); }();}

std::shared_ptr<cpp_ast::CppExpression> derive_variant_field_display_expression(mlc::String variant_name, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<ast::TyString>((*field_type))) {  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(make_std_get_expression(variant_name), field_name, false)); } return make_mlc_to_string_call(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(make_std_get_expression(variant_name), field_name, false))); }();}

std::shared_ptr<cpp_ast::CppExpression> fold_plus_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{
return expressions.size() == 0 ? emit_helpers::make_string_cpp_expression(mlc::String("")) : expressions.size() == 1 ? expressions[0] : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  std::shared_ptr<cpp_ast::CppExpression> result = expressions[0];
  int index = 1;
  while (index < expressions.size()){
{
result = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("+"), result, expressions[index]));
index = index + 1;
}
}
  return result;
 }();
}

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_display_field_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{
parts.push_back(make_string_constructor(mlc::String(", ") + field_name + mlc::String(": ")));
parts.push_back(derive_field_display_expression(field_name, field_type));
return parts;
}

std::shared_ptr<cpp_ast::CppExpression> derive_display_record_return_expression(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.size() == 0 ? make_string_constructor(type_name + mlc::String(" {}")) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts = {};
  parts.push_back(make_string_constructor(type_name + mlc::String(" { ") + field_definitions[0]->name + mlc::String(": ")));
  parts.push_back(derive_field_display_expression(field_definitions[0]->name, field_definitions[0]->type_value));
  int index = 1;
  while (index < field_definitions.size()){
{
parts = append_display_field_parts(parts, field_definitions[index]->name, field_definitions[index]->type_value);
index = index + 1;
}
}
  parts.push_back(make_string_constructor(mlc::String(" }")));
  return fold_plus_expressions(parts);
 }();}

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_variant_record_display_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
int index = 1;
while (index < field_definitions.size()){
{
parts.push_back(make_string_constructor(mlc::String(", ")));
parts.push_back(derive_variant_field_display_expression(variant_name, field_definitions[index]->name, field_definitions[index]->type_value));
index = index + 1;
}
}
return parts;
}

std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_record_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.size() == 0 ? make_string_constructor(variant_name) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts = {};
  parts.push_back(make_string_constructor(variant_name + mlc::String("(")));
  parts.push_back(derive_variant_field_display_expression(variant_name, field_definitions[0]->name, field_definitions[0]->type_value));
  parts = append_variant_record_display_parts(parts, variant_name, field_definitions);
  parts.push_back(make_string_constructor(mlc::String(")")));
  return fold_plus_expressions(parts);
 }();}

std::shared_ptr<cpp_ast::CppExpression> derive_display_tuple_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept{return field_types.size() == 0 ? make_string_constructor(variant_name) : fold_plus_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{make_string_constructor(variant_name + mlc::String("(")), make_mlc_to_string_call(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(make_std_get_expression(variant_name), mlc::String("_0"), false))), make_string_constructor(mlc::String(")"))});}

std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_return_expression(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, _w0] = varunit; return make_string_constructor(name); },
  [&](const VarTuple& vartuple) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, field_types, _w0] = vartuple; return derive_display_tuple_return_expression(name, field_types); },
  [&](const VarRecord& varrecord) -> std::shared_ptr<cpp_ast::CppExpression> { auto [name, field_definitions, _w0] = varrecord; return derive_display_variant_record_return_expression(name, field_definitions); }
}, (*variant));}

std::shared_ptr<cpp_ast::CppStatement> display_to_string_if_statement(std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::String variant_name = std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; }
}, (*variant));
return emit_helpers::make_if_cpp_statement(make_holds_alternative_expression(variant_name), emit_helpers::make_return_cpp_statement(derive_display_variant_return_expression(variant)), emit_helpers::make_block_cpp_statement({}));
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_display_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
int index = 0;
while (index < variants.size()){
{
statements.push_back(display_to_string_if_statement(variants[index]));
index = index + 1;
}
}
statements.push_back(emit_helpers::make_return_cpp_statement(make_string_constructor(mlc::String(""))));
return statements;
}

std::shared_ptr<cpp_ast::CppDeclaration> display_to_string_function_definition(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef(mlc::String(""), mlc::String("mlc::String"), type_name + mlc::String("_to_string"), mlc::Array<mlc::String>{mlc::String("const ") + type_name + mlc::String("& self")}, body_statements, 1));}

std::shared_ptr<cpp_ast::CppDeclaration> derive_display_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return display_to_string_function_definition(type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(derive_display_record_return_expression(type_name, field_definitions))});}

std::shared_ptr<cpp_ast::CppDeclaration> derive_display_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return display_to_string_function_definition(type_name, derive_display_sum_body_statements(variants));}

bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() != 1 ? false : [&]() { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, _w1, _w2] = _v_varrecord; return true; } return false; }();}

mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_definitions(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> empty = {};
return [&]() -> mlc::Array<std::shared_ptr<ast::FieldDef>> { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, field_definitions, _w1] = _v_varrecord; return field_definitions; } return empty; }();
}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_display_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants_is_single_record(variants) ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_display_record_cpp(type_name, derive_record_field_definitions(variants))} : mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_display_sum_cpp(type_name, variants)};}

std::shared_ptr<cpp_ast::CppExpression> make_parameter_field_expression(mlc::String parameter_name, mlc::String field_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(emit_helpers::make_identifier_cpp_expression(parameter_name), field_name, false));}

std::shared_ptr<cpp_ast::CppExpression> make_parameter_discriminant_expression(mlc::String parameter_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(emit_helpers::make_identifier_cpp_expression(parameter_name), mlc::String("_"), false));}

std::shared_ptr<cpp_ast::CppExpression> fold_and_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{
return expressions.size() == 0 ? emit_helpers::make_boolean_cpp_expression(true) : expressions.size() == 1 ? expressions[0] : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  std::shared_ptr<cpp_ast::CppExpression> result = expressions[0];
  int index = 1;
  while (index < expressions.size()){
{
result = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("&&"), result, expressions[index]));
index = index + 1;
}
}
  return result;
 }();
}

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> build_eq_field_comparisons(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> comparisons = {};
int index = 0;
while (index < field_definitions.size()){
{
comparisons.push_back(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("=="), make_parameter_field_expression(mlc::String("a"), field_definitions[index]->name), make_parameter_field_expression(mlc::String("b"), field_definitions[index]->name))));
index = index + 1;
}
}
return comparisons;
}

std::shared_ptr<cpp_ast::CppExpression> derive_eq_sum_return_expression() noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("=="), make_parameter_discriminant_expression(mlc::String("a")), make_parameter_discriminant_expression(mlc::String("b"))));}

std::shared_ptr<cpp_ast::CppDeclaration> eq_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef(mlc::String(""), mlc::String("bool"), mlc::String("operator=="), mlc::Array<mlc::String>{mlc::String("const ") + type_name + mlc::String("& a"), mlc::String("const ") + type_name + mlc::String("& b")}, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(return_expression)}, 1));}

std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return eq_operator_function_definition(type_name, fold_and_expressions(build_eq_field_comparisons(field_definitions)));}

std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_sum_cpp(mlc::String type_name) noexcept{return eq_operator_function_definition(type_name, derive_eq_sum_return_expression());}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_eq_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants_is_single_record(variants) ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_eq_record_cpp(type_name, derive_record_field_definitions(variants))} : mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_eq_sum_cpp(type_name)};}

std::shared_ptr<cpp_ast::CppExpression> make_field_less_than_expression(mlc::String field_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("<"), make_parameter_field_expression(mlc::String("a"), field_name), make_parameter_field_expression(mlc::String("b"), field_name)));}

std::shared_ptr<cpp_ast::CppExpression> make_field_equal_expression(mlc::String field_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("=="), make_parameter_field_expression(mlc::String("a"), field_name), make_parameter_field_expression(mlc::String("b"), field_name)));}

std::shared_ptr<cpp_ast::CppExpression> build_prefix_equal_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int up_to_index) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> comparisons = {};
int index = 0;
while (index < up_to_index){
{
comparisons.push_back(make_field_equal_expression(field_definitions[index]->name));
index = index + 1;
}
}
return fold_and_expressions(comparisons);
}

std::shared_ptr<cpp_ast::CppExpression> build_ord_record_return_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.size() == 0 ? emit_helpers::make_boolean_cpp_expression(false) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  std::shared_ptr<cpp_ast::CppExpression> result = make_field_less_than_expression(field_definitions[0]->name);
  int index = 1;
  while (index < field_definitions.size()){
{
result = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("||"), result, std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("&&"), build_prefix_equal_expression(field_definitions, index), make_field_less_than_expression(field_definitions[index]->name)))));
index = index + 1;
}
}
  return result;
 }();}

std::shared_ptr<cpp_ast::CppExpression> derive_ord_sum_return_expression() noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("<"), make_parameter_discriminant_expression(mlc::String("a")), make_parameter_discriminant_expression(mlc::String("b"))));}

std::shared_ptr<cpp_ast::CppDeclaration> ord_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef(mlc::String(""), mlc::String("bool"), mlc::String("operator<"), mlc::Array<mlc::String>{mlc::String("const ") + type_name + mlc::String("& a"), mlc::String("const ") + type_name + mlc::String("& b")}, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(return_expression)}, 1));}

std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return ord_operator_function_definition(type_name, build_ord_record_return_expression(field_definitions));}

std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_sum_cpp(mlc::String type_name) noexcept{return ord_operator_function_definition(type_name, derive_ord_sum_return_expression());}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_ord_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants_is_single_record(variants) ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_ord_record_cpp(type_name, derive_record_field_definitions(variants))} : mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_ord_sum_cpp(type_name)};}

mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyString>((*field_type))) {  return mlc::String("mlc::String"); } if (std::holds_alternative<ast::TyI32>((*field_type))) {  return mlc::String("int"); } if (std::holds_alternative<ast::TyBool>((*field_type))) {  return mlc::String("bool"); } return mlc::String(""); }();}

std::shared_ptr<cpp_ast::CppExpression> make_hash_tuple_field_access_expression(mlc::String variant_name, int field_index) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(make_std_get_expression(variant_name), mlc::String("field") + mlc::to_string(field_index), false));}

std::shared_ptr<cpp_ast::CppExpression> make_hash_variant_field_access_expression(mlc::String variant_name, mlc::String field_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(make_std_get_expression(variant_name), field_name, false));}

std::shared_ptr<cpp_ast::CppStatement> make_hash_combine_statement(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept{return hash_stmt_helpers::make_hash_combine_cpp_statement(cpp_type, access);}

std::shared_ptr<cpp_ast::CppStatement> make_hash_seed_declaration_statement() noexcept{return hash_stmt_helpers::make_hash_seed_cpp_statement();}

std::shared_ptr<cpp_ast::CppStatement> make_hash_empty_return_statement() noexcept{return hash_stmt_helpers::make_hash_empty_return_cpp_statement();}

std::shared_ptr<cpp_ast::CppStatement> make_hash_return_h_statement() noexcept{return emit_helpers::make_return_cpp_statement(emit_helpers::make_identifier_cpp_expression(mlc::String("h")));}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_record_body_statements(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.size() == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{make_hash_empty_return_statement()} : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  statements.push_back(make_hash_seed_declaration_statement());
  int index = 0;
  while (index < field_definitions.size()){
{
statements.push_back(make_hash_combine_statement(derive_hash_std_cpp_type(field_definitions[index]->type_value), make_self_field_expression(field_definitions[index]->name)));
index = index + 1;
}
}
  statements.push_back(make_hash_return_h_statement());
  return statements;
 }();}

mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; }
}, (*variant));}

std::shared_ptr<cpp_ast::CppStatement> derive_hash_discriminant_combine_statement(int discriminant_index) noexcept{return make_hash_combine_statement(mlc::String("size_t"), emit_helpers::make_integer_cpp_expression(discriminant_index));}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_tuple_field_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept{
statements.push_back(derive_hash_discriminant_combine_statement(discriminant_index));
int field_index = 0;
while (field_index < field_types.size()){
{
statements.push_back(make_hash_combine_statement(derive_hash_std_cpp_type(field_types[field_index]), make_hash_tuple_field_access_expression(variant_name, field_index)));
field_index = field_index + 1;
}
}
return statements;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_record_variant_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int discriminant_index) noexcept{
statements.push_back(derive_hash_discriminant_combine_statement(discriminant_index));
int index = 0;
while (index < field_definitions.size()){
{
statements.push_back(make_hash_combine_statement(derive_hash_std_cpp_type(field_definitions[index]->type_value), make_hash_variant_field_access_expression(variant_name, field_definitions[index]->name)));
index = index + 1;
}
}
return statements;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_variant_body_statements(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  statements.push_back(derive_hash_discriminant_combine_statement(discriminant_index));
  statements.push_back(make_hash_return_h_statement());
  return statements;
 }(); },
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { auto [variant_name, field_types, _w0] = vartuple; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  append_hash_tuple_field_combine_statements(statements, variant_name, field_types, discriminant_index);
  statements.push_back(make_hash_return_h_statement());
  return statements;
 }(); },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { auto [variant_name, field_definitions, _w0] = varrecord; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  append_hash_record_variant_combine_statements(statements, variant_name, field_definitions, discriminant_index);
  statements.push_back(make_hash_return_h_statement());
  return statements;
 }(); }
}, (*variant));}

std::shared_ptr<cpp_ast::CppStatement> derive_hash_sum_branch_statement(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{
mlc::String variant_name = type_variant_constructor_name(variant);
return emit_helpers::make_if_cpp_statement(make_holds_alternative_expression(variant_name), emit_helpers::make_block_cpp_statement(derive_hash_variant_body_statements(variant, discriminant_index)), emit_helpers::make_block_cpp_statement({}));
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
statements.push_back(make_hash_seed_declaration_statement());
int discriminant_index = 0;
int variant_index = 0;
while (variant_index < variants.size()){
{
statements.push_back(derive_hash_sum_branch_statement(variants[variant_index], discriminant_index));
discriminant_index = discriminant_index + 1;
variant_index = variant_index + 1;
}
}
statements.push_back(make_hash_return_h_statement());
return statements;
}

std::shared_ptr<cpp_ast::CppDeclaration> hash_specialization_declaration(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStdHashSpecialization(type_name, body_statements));}

std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return hash_specialization_declaration(type_name, derive_hash_record_body_statements(field_definitions));}

std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return hash_specialization_declaration(type_name, derive_hash_sum_body_statements(variants));}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_hash_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants_is_single_record(variants) ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_hash_record_cpp(type_name, derive_record_field_definitions(variants))} : mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_hash_sum_cpp(type_name, variants)};}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_trait_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (trait_name == mlc::String("Display")) { return gen_derive_display_cpp(type_name, variants); } if (trait_name == mlc::String("Eq")) { return gen_derive_eq_cpp(type_name, variants); } if (trait_name == mlc::String("Ord")) { return gen_derive_ord_cpp(type_name, variants); } if (trait_name == mlc::String("Hash")) { return gen_derive_hash_cpp(type_name, variants); } return {}; }();}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_methods_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
return derive_traits.size() == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{} : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations = {};
  int trait_index = 0;
  while (trait_index < derive_traits.size()){
{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> trait_declarations = gen_derive_trait_cpp(type_name, variants, derive_traits[trait_index]);
int declaration_index = 0;
while (declaration_index < trait_declarations.size()){
{
declarations.push_back(trait_declarations[declaration_index]);
declaration_index = declaration_index + 1;
}
}
trait_index = trait_index + 1;
}
}
  return declarations;
 }();
}

} // namespace derive_methods_cpp
