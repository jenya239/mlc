#define main mlc_user_main
#include "let_pat_cpp.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "print.hpp"

namespace let_pat_cpp {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace decl_index;
using namespace type_gen;
using namespace cpp_naming;
using namespace match_analysis;
using namespace print;
using namespace ast_tokens;

mlc::String print_statements_as_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
  return statements.map([=](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return (print::print_statement(statement) + mlc::String("\n", 1)); }).join(mlc::String("", 0));
}
std::shared_ptr<cpp_ast::CppExpression> member_access(mlc::String object_name, mlc::String field_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{emit_helpers::make_identifier_cpp_expression(object_name), field_name, false});
}
std::shared_ptr<cpp_ast::CppExpression> index_access(mlc::String object_name, int index) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIndex{emit_helpers::make_identifier_cpp_expression(object_name), emit_helpers::make_integer_cpp_expression(index)});
}
std::shared_ptr<cpp_ast::CppExpression> method_call(mlc::String object_name, mlc::String method_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{emit_helpers::make_identifier_cpp_expression(object_name), method_name, false}), {}});
}
std::shared_ptr<cpp_ast::CppExpression> variant_holds_alternative_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{((mlc::String("std::holds_alternative<", 23) + variant_type_name) + mlc::String(">", 1))}), mlc::Array{emit_helpers::make_identifier_cpp_expression(subject_name)}});
}
std::shared_ptr<cpp_ast::CppExpression> variant_get_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{((mlc::String("std::get<", 9) + variant_type_name) + mlc::String(">", 1))}), mlc::Array{emit_helpers::make_identifier_cpp_expression(subject_name)}});
}
std::shared_ptr<cpp_ast::CppExpression> array_rest_slice_expression(mlc::String array_name, int from_index, mlc::String element_cpp_type) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{((mlc::String("mlc::Array<", 11) + element_cpp_type) + mlc::String(">", 1))}), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("+", 1), method_call(array_name, mlc::String("cbegin", 6)), emit_helpers::make_integer_cpp_expression(from_index)}), method_call(array_name, mlc::String("cend", 4))}});
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> extra) noexcept{
  auto index = 0;
  while ((index < extra.length()))   {
    output.push_back(extra[index]);
    (index = (index + 1));
  }
  return output;
}
std::shared_ptr<cpp_ast::CppStatement> variant_get_binding_statement(mlc::String variant_type_name, mlc::String binding_name, mlc::String subject_name) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppConstDecl{binding_name, std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRef{std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{variant_type_name})}), variant_get_expression(variant_type_name, subject_name)});
}
std::shared_ptr<cpp_ast::CppExpression> tuple_element_access(mlc::String source_name, int element_index) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{((mlc::String("std::get<", 9) + mlc::to_string(element_index)) + mlc::String(">", 1))}), mlc::Array{emit_helpers::make_identifier_cpp_expression(source_name)}});
}
bool pattern_is_identifier(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> bool {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [__0, __1] = patternIdent; return true;
}
return false;
std::abort();
}();
}
bool tuple_subpatterns_are_simple_identifiers(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns) noexcept{
  auto index = 0;
  auto all_simple = true;
  while ((index < subpatterns.length()))   {
    if ((!pattern_is_identifier(subpatterns[index])))     {
      (all_simple = false);
    }
    (index = (index + 1));
  }
  return all_simple;
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> simple_tuple_ident_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept{
  auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  auto index = 0;
  while ((index < subpatterns.length()))   {
    statements.push_back(tuple_ident_binding_statement(subpatterns[index], source_name, index));
    (index = (index + 1));
  }
  return statements;
}
std::shared_ptr<cpp_ast::CppStatement> tuple_ident_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String source_name, int index) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [binding_name, __1] = patternIdent; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(binding_name), tuple_element_access(source_name, index)); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*subpattern));
}
std::shared_ptr<cpp_ast::CppStatement> tuple_structured_binding_statement(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept{
  return emit_helpers::make_structured_binding_cpp_statement(match_analysis::pattern_binding_name_list(subpatterns), emit_helpers::make_identifier_cpp_expression(source_name));
}
std::shared_ptr<cpp_ast::CppStatement> let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, std::shared_ptr<cpp_ast::CppExpression> value_expression, mlc::String temp_name) noexcept{
  auto block_statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  block_statements.push_back(emit_helpers::make_auto_cpp_statement(temp_name, value_expression));
  auto index = 0;
  while ((index < statements.length()))   {
    block_statements.push_back(statements[index]);
    (index = (index + 1));
  }
  return emit_helpers::make_fragment_cpp_statement(print_statements_as_fragment(block_statements));
}
mlc::String record_codegen_name_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [record_nominal_name] = tNamed; return record_nominal_name; },
[&](const registry::TGeneric& tGeneric) { auto [record_nominal_name, __1] = tGeneric; return record_nominal_name; },
[&](const registry::TI32& tI32) { return mlc::String("", 0); },
[&](const registry::TString& tString) { return mlc::String("", 0); },
[&](const registry::TBool& tBool) { return mlc::String("", 0); },
[&](const registry::TUnit& tUnit) { return mlc::String("", 0); },
[&](const registry::TI64& tI64) { return mlc::String("", 0); },
[&](const registry::TF64& tF64) { return mlc::String("", 0); },
[&](const registry::TU8& tU8) { return mlc::String("", 0); },
[&](const registry::TUsize& tUsize) { return mlc::String("", 0); },
[&](const registry::TChar& tChar) { return mlc::String("", 0); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return mlc::String("", 0); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return mlc::String("", 0); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return mlc::String("", 0); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("", 0); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return mlc::String("", 0); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return mlc::String("", 0); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("", 0); }
}, (*value_type));
}
bool value_type_is_tuple_or_pair(std::shared_ptr<registry::Type> value_type) noexcept{
  return [&]() -> bool {
auto __match_subject = value_type;
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [__0] = tTuple; return true;
}
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [__0, __1] = tPair; return true;
}
return false;
std::abort();
}();
}
mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = value_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner_type] = tArray; return type_gen::sem_type_to_cpp(codegen_context, inner_type);
}
return mlc::String("auto", 4);
std::abort();
}();
}
GenLetPatternCppResult tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{
  if (value_type_is_tuple_or_pair(value_type))   {
    if (tuple_subpatterns_are_simple_identifiers(subpatterns))     {
      return GenLetPatternCppResult{let_pattern_block(simple_tuple_ident_binding_statements(subpatterns, temp_name), value_expression, temp_name), codegen_context};
    } else     {
      return GenLetPatternCppResult{let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{tuple_structured_binding_statement(subpatterns, temp_name)}, value_expression, temp_name), codegen_context};
    }
  } else   {
    return record_or_fallback_tuple_pattern_cpp_result(subpatterns, value_type, temp_name, value_expression, codegen_context);
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> record_member_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::Array<mlc::String> ordered_member_labels, mlc::String temp_name) noexcept{
  auto member_statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  auto member_slot_index = 0;
  while ((member_slot_index < subpatterns.length()))   {
    member_statements.push_back(record_member_binding_statement(subpatterns[member_slot_index], ordered_member_labels[member_slot_index], temp_name));
    (member_slot_index = (member_slot_index + 1));
  }
  return member_statements;
}
std::shared_ptr<cpp_ast::CppStatement> record_member_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String member_label, mlc::String temp_name) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [binding_member_name, __1] = patternIdent; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(binding_member_name), member_access(temp_name, member_label)); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*subpattern));
}
GenLetPatternCppResult record_or_fallback_tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{
  auto record_codegen_name = record_codegen_name_from_value_type(value_type);
  auto ordered_member_labels = context::lookup_fields_for_context(codegen_context, record_codegen_name);
  if ((((record_codegen_name != mlc::String("", 0)) && (ordered_member_labels.length() == subpatterns.length())) && (subpatterns.length() > 0)))   {
    return GenLetPatternCppResult{let_pattern_block(record_member_binding_statements(subpatterns, ordered_member_labels, temp_name), value_expression, temp_name), codegen_context};
  } else   {
    return GenLetPatternCppResult{let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{tuple_structured_binding_statement(subpatterns, temp_name)}, value_expression, temp_name), codegen_context};
  }
}
GenLetPatternCppResult record_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{
  auto field_statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  auto index = 0;
  while ((index < field_patterns.length()))   {
    field_statements.push_back(record_field_binding_statement(field_patterns[index], temp_name));
    (index = (index + 1));
  }
  return GenLetPatternCppResult{let_pattern_block(field_statements, value_expression, temp_name), codegen_context};
}
std::shared_ptr<cpp_ast::CppStatement> record_field_binding_statement(std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [field_name, __1] = patternIdent; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(field_name), member_access(temp_name, field_name)); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*field_pattern));
}
GenLetPatternCppResult array_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String rest_name, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{
  auto element_cpp = array_element_cpp_for_value_type(value_type, codegen_context);
  auto array_statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  auto index = 0;
  while ((index < subpatterns.length()))   {
    array_statements.push_back(array_element_binding_statement(subpatterns[index], temp_name, index));
    (index = (index + 1));
  }
  if ((rest_name != mlc::String("", 0)))   {
    array_statements.push_back(emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(rest_name), array_rest_slice_expression(temp_name, subpatterns.length(), element_cpp)));
  }
  return GenLetPatternCppResult{let_pattern_block(array_statements, value_expression, temp_name), codegen_context};
}
std::shared_ptr<cpp_ast::CppStatement> array_element_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String temp_name, int index) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [name, __1] = patternIdent; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), index_access(temp_name, index)); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*subpattern));
}
GenLetPatternCppResult ctor_pattern_cpp_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept{
  auto qualified_name = context::CodegenContext_resolve(codegen_context, constructor_name);
  auto is_generic = decl_index::list_contains(codegen_context.generic_variants, constructor_name);
  auto type_arguments = (is_generic ? (mlc::String("<auto>", 6)) : (mlc::String("", 0)));
  auto variant_type_name = (qualified_name + type_arguments);
  auto second_temp = context::take_temp_name(codegen_context);
  auto variant_binding_name = second_temp.name;
  auto updated_codegen_context = second_temp.codegen_context;
  auto constructor_statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  constructor_statements.push_back(emit_helpers::make_auto_cpp_statement(temp_name, value_expression));
  constructor_statements.push_back(emit_helpers::make_if_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppUnary{mlc::String("!", 1), variant_holds_alternative_expression(variant_type_name, temp_name)}), emit_helpers::make_block_cpp_statement(constructor_mismatch_else_statements(has_else, else_body, updated_codegen_context, expression_codegen)), emit_helpers::make_block_cpp_statement({})));
  constructor_statements.push_back(variant_get_binding_statement(variant_type_name, variant_binding_name, temp_name));
  if ((subpatterns.length() > 0))   {
    if (tuple_subpatterns_are_simple_identifiers(subpatterns))     {
      (constructor_statements = append_statements(constructor_statements, simple_tuple_ident_binding_statements(subpatterns, variant_binding_name)));
    } else     {
      constructor_statements.push_back(tuple_structured_binding_statement(subpatterns, variant_binding_name));
    }
  }
  return GenLetPatternCppResult{emit_helpers::make_fragment_cpp_statement(print_statements_as_fragment(constructor_statements)), updated_codegen_context};
}
GenLetPatternCppResult gen_let_pattern_cpp_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept{
  auto value_expression = expression_codegen(value, context);
  auto first_temp = context::take_temp_name(context);
  auto temp_name = first_temp.name;
  auto codegen_context = first_temp.codegen_context;
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) -> GenLetPatternCppResult { auto [name, __1] = patternIdent; return GenLetPatternCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), value_expression), codegen_context}; },
[&](const ast::PatternTuple& patternTuple) -> GenLetPatternCppResult { auto [subpatterns, __1] = patternTuple; return tuple_pattern_cpp_result(subpatterns, value_type, temp_name, value_expression, codegen_context); },
[&](const ast::PatternRecord& patternRecord) -> GenLetPatternCppResult { auto [__0, field_patterns, __2] = patternRecord; return record_pattern_cpp_result(field_patterns, temp_name, value_expression, codegen_context); },
[&](const ast::PatternArray& patternArray) -> GenLetPatternCppResult { auto [subpatterns, rest_name, __2] = patternArray; return array_pattern_cpp_result(subpatterns, rest_name, value_type, temp_name, value_expression, codegen_context); },
[&](const ast::PatternCtor& patternCtor) -> GenLetPatternCppResult { auto [constructor_name, subpatterns, __2] = patternCtor; return ctor_pattern_cpp_result(constructor_name, subpatterns, temp_name, value_expression, has_else, else_body, codegen_context, expression_codegen); },
[&](const ast::PatternWild& patternWild) -> GenLetPatternCppResult { auto [__0] = patternWild; return GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
[&](const ast::PatternInt& patternInt) -> GenLetPatternCppResult { auto [__0, __1] = patternInt; return GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
[&](const ast::PatternStr& patternStr) -> GenLetPatternCppResult { auto [__0, __1] = patternStr; return GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
[&](const ast::PatternStringLit& patternStringLit) -> GenLetPatternCppResult { auto [__0, __1] = patternStringLit; return GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
[&](const ast::PatternBool& patternBool) -> GenLetPatternCppResult { auto [__0, __1] = patternBool; return GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
[&](const ast::PatternUnit& patternUnit) -> GenLetPatternCppResult { auto [__0] = patternUnit; return GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
[&](const ast::PatternOr& patternOr) -> GenLetPatternCppResult { auto [__0, __1] = patternOr; return GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; }
}, (*pattern));
}

} // namespace let_pat_cpp
