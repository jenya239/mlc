#define main mlc_user_main
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

std::shared_ptr<cpp_ast::CppExpression> make_string_constructor(mlc::String text) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::String", 11)), mlc::Array{emit_helpers::make_string_cpp_expression(text)}});
}
std::shared_ptr<cpp_ast::CppExpression> make_mlc_to_string_call(std::shared_ptr<cpp_ast::CppExpression> argument) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::to_string", 14)), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{argument}});
}
std::shared_ptr<cpp_ast::CppExpression> make_self_field_expression(mlc::String field_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{emit_helpers::make_identifier_cpp_expression(mlc::String("self", 4)), field_name, false});
}
std::shared_ptr<cpp_ast::CppExpression> make_self_discriminant_expression() noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{emit_helpers::make_identifier_cpp_expression(mlc::String("self", 4)), mlc::String("_", 1), false});
}
std::shared_ptr<cpp_ast::CppExpression> make_std_get_expression(mlc::String variant_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(((mlc::String("std::get<", 9) + variant_name) + mlc::String(">", 1))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{make_self_discriminant_expression()}});
}
std::shared_ptr<cpp_ast::CppExpression> make_holds_alternative_expression(mlc::String variant_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(((mlc::String("std::holds_alternative<", 23) + variant_name) + mlc::String(">", 1))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{make_self_discriminant_expression()}});
}
std::shared_ptr<cpp_ast::CppExpression> derive_field_display_expression(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = field_type;
if (std::holds_alternative<ast::TyString>((*__match_subject))) {
const ast::TyString& tyString = std::get<ast::TyString>((*__match_subject));
return make_self_field_expression(field_name);
}
return make_mlc_to_string_call(make_self_field_expression(field_name));
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> derive_variant_field_display_expression(mlc::String variant_name, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = field_type;
if (std::holds_alternative<ast::TyString>((*__match_subject))) {
const ast::TyString& tyString = std::get<ast::TyString>((*__match_subject));
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{make_std_get_expression(variant_name), field_name, false});
}
return make_mlc_to_string_call(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{make_std_get_expression(variant_name), field_name, false}));
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> fold_plus_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{
  if ((expressions.length() == 0))   {
    return emit_helpers::make_string_cpp_expression(mlc::String("", 0));
  } else if ((expressions.length() == 1))   {
    return expressions[0];
  } else   {
    auto result = expressions[0];
    auto index = 1;
    while ((index < expressions.length()))     {
      (result = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("+", 1), result, expressions[index]}));
      (index = (index + 1));
    }
    return result;
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_display_field_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{
  parts.push_back(make_string_constructor(((mlc::String(", ", 2) + field_name) + mlc::String(": ", 2))));
  parts.push_back(derive_field_display_expression(field_name, field_type));
  return parts;
}
std::shared_ptr<cpp_ast::CppExpression> derive_display_record_return_expression(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  if ((field_definitions.length() == 0))   {
    return make_string_constructor((type_name + mlc::String(" {}", 3)));
  } else   {
    auto parts = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{};
    parts.push_back(make_string_constructor((((type_name + mlc::String(" { ", 3)) + field_definitions[0]->name) + mlc::String(": ", 2))));
    parts.push_back(derive_field_display_expression(field_definitions[0]->name, field_definitions[0]->type_value));
    auto index = 1;
    while ((index < field_definitions.length()))     {
      (parts = append_display_field_parts(parts, field_definitions[index]->name, field_definitions[index]->type_value));
      (index = (index + 1));
    }
    parts.push_back(make_string_constructor(mlc::String(" }", 2)));
    return fold_plus_expressions(parts);
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_variant_record_display_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  auto index = 1;
  while ((index < field_definitions.length()))   {
    parts.push_back(make_string_constructor(mlc::String(", ", 2)));
    parts.push_back(derive_variant_field_display_expression(variant_name, field_definitions[index]->name, field_definitions[index]->type_value));
    (index = (index + 1));
  }
  return parts;
}
std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_record_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  if ((field_definitions.length() == 0))   {
    return make_string_constructor(variant_name);
  } else   {
    auto parts = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{};
    parts.push_back(make_string_constructor((variant_name + mlc::String("(", 1))));
    parts.push_back(derive_variant_field_display_expression(variant_name, field_definitions[0]->name, field_definitions[0]->type_value));
    (parts = append_variant_record_display_parts(parts, variant_name, field_definitions));
    parts.push_back(make_string_constructor(mlc::String(")", 1)));
    return fold_plus_expressions(parts);
  }
}
std::shared_ptr<cpp_ast::CppExpression> derive_display_tuple_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept{
  if ((field_types.length() == 0))   {
    return make_string_constructor(variant_name);
  } else   {
    return fold_plus_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{make_string_constructor((variant_name + mlc::String("(", 1))), make_mlc_to_string_call(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{make_std_get_expression(variant_name), mlc::String("_0", 2), false})), make_string_constructor(mlc::String(")", 1))});
  }
}
std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_return_expression(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return make_string_constructor(name); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types, __2] = varTuple; return derive_display_tuple_return_expression(name, field_types); },
[&](const ast::VarRecord& varRecord) { auto [name, field_definitions, __2] = varRecord; return derive_display_variant_record_return_expression(name, field_definitions); }
}, (*variant));
}
mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return name; },
[&](const ast::VarTuple& varTuple) { auto [name, __1, __2] = varTuple; return name; },
[&](const ast::VarRecord& varRecord) { auto [name, __1, __2] = varRecord; return name; }
}, (*variant));
}
std::shared_ptr<cpp_ast::CppStatement> display_to_string_if_statement(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return emit_helpers::make_if_cpp_statement(make_holds_alternative_expression(type_variant_constructor_name(variant)), emit_helpers::make_return_cpp_statement(derive_display_variant_return_expression(variant)), emit_helpers::make_block_cpp_statement({}));
}
mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_definitions(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions, __2] = varRecord; return field_definitions; },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return empty_derive_field_definitions(); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1, __2] = varTuple; return empty_derive_field_definitions(); }
}, (*variants[0]));
}
mlc::Array<std::shared_ptr<ast::FieldDef>> empty_derive_field_definitions() noexcept{
  auto empty = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
  return empty;
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_display_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  auto index = 0;
  while ((index < variants.length()))   {
    statements.push_back(display_to_string_if_statement(variants[index]));
    (index = (index + 1));
  }
  statements.push_back(emit_helpers::make_return_cpp_statement(make_string_constructor(mlc::String("", 0))));
  return statements;
}
std::shared_ptr<cpp_ast::CppDeclaration> display_to_string_function_definition(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{mlc::String("", 0), mlc::String("mlc::String", 11), (type_name + mlc::String("_to_string", 10)), mlc::Array<mlc::String>{((mlc::String("const ", 6) + type_name) + mlc::String("& self", 6))}, body_statements, 1});
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_display_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  return display_to_string_function_definition(type_name, mlc::Array{emit_helpers::make_return_cpp_statement(derive_display_record_return_expression(type_name, field_definitions))});
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_display_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return display_to_string_function_definition(type_name, derive_display_sum_body_statements(variants));
}
bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if ((variants.length() != 1))   {
    return false;
  } else   {
    return [&]() -> bool {
auto __match_subject = variants[0];
if (std::holds_alternative<ast::VarRecord>((*__match_subject))) {
const ast::VarRecord& varRecord = std::get<ast::VarRecord>((*__match_subject));
auto [__0, __1, __2] = varRecord; return true;
}
return false;
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_display_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if (variants_is_single_record(variants))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_display_record_cpp(type_name, derive_record_field_definitions(variants))};
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_display_sum_cpp(type_name, variants)};
  }
}
std::shared_ptr<cpp_ast::CppExpression> make_parameter_field_expression(mlc::String parameter_name, mlc::String field_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{emit_helpers::make_identifier_cpp_expression(parameter_name), field_name, false});
}
std::shared_ptr<cpp_ast::CppExpression> make_parameter_discriminant_expression(mlc::String parameter_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{emit_helpers::make_identifier_cpp_expression(parameter_name), mlc::String("_", 1), false});
}
std::shared_ptr<cpp_ast::CppExpression> fold_and_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{
  if ((expressions.length() == 0))   {
    return emit_helpers::make_boolean_cpp_expression(true);
  } else if ((expressions.length() == 1))   {
    return expressions[0];
  } else   {
    auto result = expressions[0];
    auto index = 1;
    while ((index < expressions.length()))     {
      (result = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("&&", 2), result, expressions[index]}));
      (index = (index + 1));
    }
    return result;
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> build_eq_field_comparisons(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  auto comparisons = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{};
  auto index = 0;
  while ((index < field_definitions.length()))   {
    comparisons.push_back(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("==", 2), make_parameter_field_expression(mlc::String("a", 1), field_definitions[index]->name), make_parameter_field_expression(mlc::String("b", 1), field_definitions[index]->name)}));
    (index = (index + 1));
  }
  return comparisons;
}
std::shared_ptr<cpp_ast::CppExpression> derive_eq_sum_return_expression() noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("==", 2), make_parameter_discriminant_expression(mlc::String("a", 1)), make_parameter_discriminant_expression(mlc::String("b", 1))});
}
std::shared_ptr<cpp_ast::CppDeclaration> eq_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{mlc::String("", 0), mlc::String("bool", 4), mlc::String("operator==", 10), mlc::Array<mlc::String>{((mlc::String("const ", 6) + type_name) + mlc::String("& a", 3)), ((mlc::String("const ", 6) + type_name) + mlc::String("& b", 3))}, mlc::Array{emit_helpers::make_return_cpp_statement(return_expression)}, 1});
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  return eq_operator_function_definition(type_name, fold_and_expressions(build_eq_field_comparisons(field_definitions)));
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_sum_cpp(mlc::String type_name) noexcept{
  return eq_operator_function_definition(type_name, derive_eq_sum_return_expression());
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_eq_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if (variants_is_single_record(variants))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_eq_record_cpp(type_name, derive_record_field_definitions(variants))};
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_eq_sum_cpp(type_name)};
  }
}
std::shared_ptr<cpp_ast::CppExpression> make_field_less_than_expression(mlc::String field_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("<", 1), make_parameter_field_expression(mlc::String("a", 1), field_name), make_parameter_field_expression(mlc::String("b", 1), field_name)});
}
std::shared_ptr<cpp_ast::CppExpression> make_field_equal_expression(mlc::String field_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("==", 2), make_parameter_field_expression(mlc::String("a", 1), field_name), make_parameter_field_expression(mlc::String("b", 1), field_name)});
}
std::shared_ptr<cpp_ast::CppExpression> build_prefix_equal_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int up_to_index) noexcept{
  auto comparisons = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{};
  auto index = 0;
  while ((index < up_to_index))   {
    comparisons.push_back(make_field_equal_expression(field_definitions[index]->name));
    (index = (index + 1));
  }
  return fold_and_expressions(comparisons);
}
std::shared_ptr<cpp_ast::CppExpression> build_ord_record_return_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  if ((field_definitions.length() == 0))   {
    return emit_helpers::make_boolean_cpp_expression(false);
  } else   {
    auto result = make_field_less_than_expression(field_definitions[0]->name);
    auto index = 1;
    while ((index < field_definitions.length()))     {
      (result = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("||", 2), result, std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("&&", 2), build_prefix_equal_expression(field_definitions, index), make_field_less_than_expression(field_definitions[index]->name)})}));
      (index = (index + 1));
    }
    return result;
  }
}
std::shared_ptr<cpp_ast::CppExpression> derive_ord_sum_return_expression() noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("<", 1), make_parameter_discriminant_expression(mlc::String("a", 1)), make_parameter_discriminant_expression(mlc::String("b", 1))});
}
std::shared_ptr<cpp_ast::CppDeclaration> ord_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{mlc::String("", 0), mlc::String("bool", 4), mlc::String("operator<", 9), mlc::Array<mlc::String>{((mlc::String("const ", 6) + type_name) + mlc::String("& a", 3)), ((mlc::String("const ", 6) + type_name) + mlc::String("& b", 3))}, mlc::Array{emit_helpers::make_return_cpp_statement(return_expression)}, 1});
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  return ord_operator_function_definition(type_name, build_ord_record_return_expression(field_definitions));
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_sum_cpp(mlc::String type_name) noexcept{
  return ord_operator_function_definition(type_name, derive_ord_sum_return_expression());
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_ord_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if (variants_is_single_record(variants))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_ord_record_cpp(type_name, derive_record_field_definitions(variants))};
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_ord_sum_cpp(type_name)};
  }
}
mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = field_type;
if (std::holds_alternative<ast::TyString>((*__match_subject))) {
const ast::TyString& tyString = std::get<ast::TyString>((*__match_subject));
return mlc::String("mlc::String", 11);
}
if (std::holds_alternative<ast::TyI32>((*__match_subject))) {
const ast::TyI32& tyI32 = std::get<ast::TyI32>((*__match_subject));
return mlc::String("int", 3);
}
if (std::holds_alternative<ast::TyBool>((*__match_subject))) {
const ast::TyBool& tyBool = std::get<ast::TyBool>((*__match_subject));
return mlc::String("bool", 4);
}
return mlc::String("", 0);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> make_hash_tuple_field_access_expression(mlc::String variant_name, int field_index) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{make_std_get_expression(variant_name), (mlc::String("field", 5) + mlc::to_string(field_index)), false});
}
std::shared_ptr<cpp_ast::CppExpression> make_hash_variant_field_access_expression(mlc::String variant_name, mlc::String field_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{make_std_get_expression(variant_name), field_name, false});
}
std::shared_ptr<cpp_ast::CppStatement> make_hash_combine_statement(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept{
  return hash_stmt_helpers::make_hash_combine_cpp_statement(cpp_type, access);
}
std::shared_ptr<cpp_ast::CppStatement> make_hash_seed_declaration_statement() noexcept{
  return hash_stmt_helpers::make_hash_seed_cpp_statement();
}
std::shared_ptr<cpp_ast::CppStatement> make_hash_empty_return_statement() noexcept{
  return hash_stmt_helpers::make_hash_empty_return_cpp_statement();
}
std::shared_ptr<cpp_ast::CppStatement> make_hash_return_h_statement() noexcept{
  return emit_helpers::make_return_cpp_statement(emit_helpers::make_identifier_cpp_expression(mlc::String("h", 1)));
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_record_body_statements(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  if ((field_definitions.length() == 0))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{make_hash_empty_return_statement()};
  } else   {
    auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
    statements.push_back(make_hash_seed_declaration_statement());
    auto index = 0;
    while ((index < field_definitions.length()))     {
      statements.push_back(make_hash_combine_statement(derive_hash_std_cpp_type(field_definitions[index]->type_value), make_self_field_expression(field_definitions[index]->name)));
      (index = (index + 1));
    }
    statements.push_back(make_hash_return_h_statement());
    return statements;
  }
}
std::shared_ptr<cpp_ast::CppStatement> derive_hash_discriminant_combine_statement(int discriminant_index) noexcept{
  return make_hash_combine_statement(mlc::String("size_t", 6), emit_helpers::make_integer_cpp_expression(discriminant_index));
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_tuple_field_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept{
  statements.push_back(derive_hash_discriminant_combine_statement(discriminant_index));
  auto field_index = 0;
  while ((field_index < field_types.length()))   {
    statements.push_back(make_hash_combine_statement(derive_hash_std_cpp_type(field_types[field_index]), make_hash_tuple_field_access_expression(variant_name, field_index)));
    (field_index = (field_index + 1));
  }
  return statements;
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_record_variant_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int discriminant_index) noexcept{
  statements.push_back(derive_hash_discriminant_combine_statement(discriminant_index));
  auto index = 0;
  while ((index < field_definitions.length()))   {
    statements.push_back(make_hash_combine_statement(derive_hash_std_cpp_type(field_definitions[index]->type_value), make_hash_variant_field_access_expression(variant_name, field_definitions[index]->name)));
    (index = (index + 1));
  }
  return statements;
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_variant_body_statements(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
statements.push_back(derive_hash_discriminant_combine_statement(discriminant_index));
statements.push_back(make_hash_return_h_statement());
return statements;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, field_types, __2] = varTuple; return [&]() {
auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
append_hash_tuple_field_combine_statements(statements, variant_name, field_types, discriminant_index);
statements.push_back(make_hash_return_h_statement());
return statements;
}(); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_definitions, __2] = varRecord; return [&]() {
auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
append_hash_record_variant_combine_statements(statements, variant_name, field_definitions, discriminant_index);
statements.push_back(make_hash_return_h_statement());
return statements;
}(); }
}, (*variant));
}
std::shared_ptr<cpp_ast::CppStatement> derive_hash_sum_branch_statement(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{
  auto variant_name = type_variant_constructor_name(variant);
  return emit_helpers::make_if_cpp_statement(make_holds_alternative_expression(variant_name), emit_helpers::make_block_cpp_statement(derive_hash_variant_body_statements(variant, discriminant_index)), emit_helpers::make_block_cpp_statement({}));
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  statements.push_back(make_hash_seed_declaration_statement());
  auto discriminant_index = 0;
  auto variant_index = 0;
  while ((variant_index < variants.length()))   {
    statements.push_back(derive_hash_sum_branch_statement(variants[variant_index], discriminant_index));
    (discriminant_index = (discriminant_index + 1));
    (variant_index = (variant_index + 1));
  }
  statements.push_back(make_hash_return_h_statement());
  return statements;
}
std::shared_ptr<cpp_ast::CppDeclaration> hash_specialization_declaration(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStdHashSpecialization{type_name, body_statements});
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  return hash_specialization_declaration(type_name, derive_hash_record_body_statements(field_definitions));
}
std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return hash_specialization_declaration(type_name, derive_hash_sum_body_statements(variants));
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_hash_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if (variants_is_single_record(variants))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_hash_record_cpp(type_name, derive_record_field_definitions(variants))};
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{derive_hash_sum_cpp(type_name, variants)};
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_trait_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
  if ((trait_name == mlc::String("Display", 7)))   {
    return gen_derive_display_cpp(type_name, variants);
  } else if ((trait_name == mlc::String("Eq", 2)))   {
    return gen_derive_eq_cpp(type_name, variants);
  } else if ((trait_name == mlc::String("Ord", 3)))   {
    return gen_derive_ord_cpp(type_name, variants);
  } else if ((trait_name == mlc::String("Hash", 4)))   {
    return gen_derive_hash_cpp(type_name, variants);
  } else   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_methods_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
  if ((derive_traits.length() == 0))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  } else   {
    auto declarations = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
    auto trait_index = 0;
    while ((trait_index < derive_traits.length()))     {
      auto trait_declarations = gen_derive_trait_cpp(type_name, variants, derive_traits[trait_index]);
      auto declaration_index = 0;
      while ((declaration_index < trait_declarations.length()))       {
        declarations.push_back(trait_declarations[declaration_index]);
        (declaration_index = (declaration_index + 1));
      }
      (trait_index = (trait_index + 1));
    }
    return declarations;
  }
}

} // namespace derive_methods_cpp
