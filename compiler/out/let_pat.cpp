#define main mlc_user_main
#include "let_pat.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"

namespace let_pat {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace decl_index;
using namespace type_gen;
using namespace cpp_naming;
using namespace match_analysis;
using namespace expr;
using namespace ast_tokens;

mlc::String gen_let_array_rest_slice(mlc::String temporary_name, int from_index, mlc::String element_cpp) noexcept{
  return ((((((((mlc::String("mlc::Array<", 11) + element_cpp) + mlc::String(">(", 2)) + temporary_name) + mlc::String(".cbegin() + ", 12)) + mlc::to_string(from_index)) + mlc::String(", ", 2)) + temporary_name) + mlc::String(".cend())", 8));
}
mlc::String primitive_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code) noexcept{
  auto primitive_tuple_binding_names = match_analysis::pattern_bind_names(sub_patterns);
  return ((((((mlc::String("auto ", 5) + temp_name) + mlc::String(" = ", 3)) + value_code) + mlc::String(";\n", 2)) + expr::tuple_destructure_binding(primitive_tuple_binding_names, temp_name)) + mlc::String("\n", 1));
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
mlc::String append_tuple_member_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String member_label, mlc::String temp_name) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [binding_member_name, __1] = patternIdent; return (fragment + expr::auto_binding_statement(cpp_naming::cpp_safe(binding_member_name), ((((mlc::String("", 0) + mlc::to_string(temp_name)) + mlc::String(".", 1)) + mlc::to_string(cpp_naming::cpp_safe(member_label))) + mlc::String("", 0)))); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return fragment; },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return fragment; },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return fragment; },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return fragment; },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return fragment; },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return fragment; },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return fragment; },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return fragment; },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return fragment; },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return fragment; },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return fragment; }
}, (*sub_pattern));
}
mlc::String record_ordered_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String record_codegen_name, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{
  auto ordered_member_labels = context::lookup_fields_for_context(codegen_context, record_codegen_name);
  if (((ordered_member_labels.length() == sub_patterns.length()) && (sub_patterns.length() > 0)))   {
    auto record_ordered_tuple_fragment = ((((mlc::String("auto ", 5) + temp_name) + mlc::String(" = ", 3)) + value_code) + mlc::String(";\n", 2));
    auto member_slot_index = 0;
    while ((member_slot_index < sub_patterns.length()))     {
      (record_ordered_tuple_fragment = append_tuple_member_ident_binding(record_ordered_tuple_fragment, sub_patterns[member_slot_index], ordered_member_labels[member_slot_index], temp_name));
      (member_slot_index = mlc::arith::checked_add(member_slot_index, 1));
    }
    return record_ordered_tuple_fragment;
  } else   {
    return primitive_tuple_pattern_output(sub_patterns, temp_name, value_code);
  }
}
mlc::String tuple_pattern_output_for_value_type(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{
  if (value_type_is_tuple_or_pair(value_type))   {
    return primitive_tuple_pattern_output(sub_patterns, temp_name, value_code);
  } else   {
    return record_or_fallback_tuple_pattern_output(sub_patterns, value_type, temp_name, value_code, codegen_context);
  }
}
mlc::String record_or_fallback_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{
  auto record_codegen_name = record_codegen_name_from_value_type(value_type);
  if ((record_codegen_name != mlc::String("", 0)))   {
    return record_ordered_tuple_pattern_output(sub_patterns, record_codegen_name, temp_name, value_code, codegen_context);
  } else   {
    return primitive_tuple_pattern_output(sub_patterns, temp_name, value_code);
  }
}
mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = value_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner] = tArray; return type_gen::sem_type_to_cpp(codegen_context, inner);
}
return mlc::String("auto", 4);
std::abort();
}();
}
mlc::String append_array_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String temp_name, int index) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [name, __1] = patternIdent; return (fragment + ((((((mlc::String("auto ", 5) + mlc::to_string(cpp_naming::cpp_safe(name))) + mlc::String(" = ", 3)) + mlc::to_string(temp_name)) + mlc::String("[", 1)) + mlc::to_string(mlc::to_string(index))) + mlc::String("];\n", 3))); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return fragment; },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return fragment; },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return fragment; },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return fragment; },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return fragment; },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return fragment; },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return fragment; },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return fragment; },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return fragment; },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return fragment; },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return fragment; }
}, (*sub_pattern));
}
mlc::String append_record_field_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [field_name, __1] = patternIdent; return (fragment + expr::auto_binding_statement(cpp_naming::cpp_safe(field_name), ((((mlc::String("", 0) + mlc::to_string(temp_name)) + mlc::String(".", 1)) + mlc::to_string(cpp_naming::cpp_safe(field_name))) + mlc::String("", 0)))); },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return fragment; },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return fragment; },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return fragment; },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return fragment; },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return fragment; },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return fragment; },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return fragment; },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return fragment; },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return fragment; },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return fragment; },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return fragment; }
}, (*field_pattern));
}
mlc::String record_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, mlc::String value_code) noexcept{
  auto binding_fragment = ((((mlc::String("auto ", 5) + temp_name) + mlc::String(" = ", 3)) + value_code) + mlc::String(";\n", 2));
  auto index = 0;
  while ((index < field_patterns.length()))   {
    (binding_fragment = append_record_field_ident_binding(binding_fragment, field_patterns[index], temp_name));
    (index = mlc::arith::checked_add(index, 1));
  }
  return binding_fragment;
}
mlc::String array_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String rest, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{
  auto element_cpp = array_element_cpp_for_value_type(value_type, codegen_context);
  auto binding_fragment = ((((mlc::String("auto ", 5) + temp_name) + mlc::String(" = ", 3)) + value_code) + mlc::String(";\n", 2));
  auto index = 0;
  while ((index < sub_patterns.length()))   {
    (binding_fragment = append_array_ident_binding(binding_fragment, sub_patterns[index], temp_name, index));
    (index = mlc::arith::checked_add(index, 1));
  }
  if ((rest != mlc::String("", 0)))   {
    (binding_fragment = (((((binding_fragment + mlc::String("auto ", 5)) + cpp_naming::cpp_safe(rest)) + mlc::String(" = ", 3)) + gen_let_array_rest_slice(temp_name, sub_patterns.length(), element_cpp)) + mlc::String(";\n", 2)));
  }
  return binding_fragment;
}
mlc::String wild_pattern_output(mlc::String value_code) noexcept{
  return expr::suffix_semicolon_newline(value_code);
}
GenLetPatternResult gen_let_pattern_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept{
  auto value_code = eval_fn(value, context);
  auto first_temp = context::take_temp_name(context);
  auto temp_name = first_temp.name;
  auto codegen_context = first_temp.codegen_context;
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) -> GenLetPatternResult { auto [binding_name, __1] = patternIdent; return GenLetPatternResult{expr::auto_binding_statement(cpp_naming::cpp_safe(binding_name), value_code), codegen_context}; },
[&](const ast::PatternTuple& patternTuple) -> GenLetPatternResult { auto [sub_patterns, __1] = patternTuple; return GenLetPatternResult{tuple_pattern_output_for_value_type(sub_patterns, value_type, temp_name, value_code, codegen_context), codegen_context}; },
[&](const ast::PatternRecord& patternRecord) -> GenLetPatternResult { auto [__0, field_patterns, __2] = patternRecord; return GenLetPatternResult{record_pattern_output(field_patterns, temp_name, value_code), codegen_context}; },
[&](const ast::PatternArray& patternArray) -> GenLetPatternResult { auto [sub_patterns, rest, __2] = patternArray; return GenLetPatternResult{array_pattern_output(sub_patterns, rest, value_type, temp_name, value_code, codegen_context), codegen_context}; },
[&](const ast::PatternCtor& patternCtor) -> GenLetPatternResult { auto [constructor_name, sub_patterns, __2] = patternCtor; return ctor_pattern_gen_result(constructor_name, sub_patterns, temp_name, value_code, has_else, else_body, codegen_context, eval_fn); },
[&](const ast::PatternWild& patternWild) -> GenLetPatternResult { auto [__0] = patternWild; return GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
[&](const ast::PatternInt& patternInt) -> GenLetPatternResult { auto [__0, __1] = patternInt; return GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
[&](const ast::PatternStr& patternStr) -> GenLetPatternResult { auto [__0, __1] = patternStr; return GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
[&](const ast::PatternStringLit& patternStringLit) -> GenLetPatternResult { auto [__0, __1] = patternStringLit; return GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
[&](const ast::PatternBool& patternBool) -> GenLetPatternResult { auto [__0, __1] = patternBool; return GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
[&](const ast::PatternUnit& patternUnit) -> GenLetPatternResult { auto [__0] = patternUnit; return GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
[&](const ast::PatternOr& patternOr) -> GenLetPatternResult { auto [__0, __1] = patternOr; return GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; }
}, (*pattern));
}

} // namespace let_pat
