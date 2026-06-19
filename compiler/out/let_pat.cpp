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

mlc::String gen_let_array_rest_slice(mlc::String temporary_name, int from_index, mlc::String element_cpp) noexcept;

let_pat::GenLetPatternResult gen_let_pattern_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept;

mlc::String gen_let_array_rest_slice(mlc::String temporary_name, int from_index, mlc::String element_cpp) noexcept{return mlc::String("mlc::Array<") + element_cpp + mlc::String(">(") + temporary_name + mlc::String(".cbegin() + ") + mlc::to_string(from_index) + mlc::String(", ") + temporary_name + mlc::String(".cend())");}

let_pat::GenLetPatternResult gen_let_pattern_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept{
mlc::String value_code = eval_fn(value, context);
context::TempNameResult first_temp = context::take_temp_name(context);
mlc::String temp_name = first_temp.name;
context::CodegenContext codegen_context = first_temp.codegen_context;
return [&]() -> let_pat::GenLetPatternResult { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [binding_name, _w0] = _v_patternident; return let_pat::GenLetPatternResult{expr::auto_binding_statement(cpp_naming::cpp_safe(binding_name), value_code), codegen_context}; } if (std::holds_alternative<ast::PatternTuple>((*pattern))) { auto _v_patterntuple = std::get<ast::PatternTuple>((*pattern)); auto [sub_patterns, _w0] = _v_patterntuple; return [&]() -> let_pat::GenLetPatternResult { 
  mlc::String output = [&]() -> mlc::String { if (std::holds_alternative<registry::TTuple>((*value_type))) { auto _v_ttuple = std::get<registry::TTuple>((*value_type)); auto [_w0] = _v_ttuple; return [&]() -> mlc::String { 
  mlc::String primitive_tuple_binding_names = match_analysis::pattern_bind_names(sub_patterns);
  return mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n") + expr::tuple_destructure_binding(primitive_tuple_binding_names, temp_name) + mlc::String("\n");
 }(); } else if (std::holds_alternative<registry::TPair>((*value_type))) { auto _v_tpair = std::get<registry::TPair>((*value_type)); return [&]() -> mlc::String { 
  mlc::String primitive_tuple_binding_names = match_analysis::pattern_bind_names(sub_patterns);
  return mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n") + expr::tuple_destructure_binding(primitive_tuple_binding_names, temp_name) + mlc::String("\n");
 }(); } return [&]() -> mlc::String { 
  mlc::String record_codegen_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*value_type))) { auto _v_tnamed = std::get<registry::TNamed>((*value_type)); auto [record_nominal_name] = _v_tnamed; return record_nominal_name; } if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [record_nominal_name, _w0] = _v_tgeneric; return record_nominal_name; } return mlc::String(""); }();
  return record_codegen_name != mlc::String("") ? [&]() -> mlc::String { 
  mlc::Array<mlc::String> ordered_member_labels = context::lookup_fields_for_context(codegen_context, record_codegen_name);
  return ordered_member_labels.size() == sub_patterns.size() && sub_patterns.size() > 0 ? [&]() -> mlc::String { 
  mlc::String record_ordered_tuple_fragment = mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n");
  int member_slot_index = 0;
  while (member_slot_index < sub_patterns.size()){
{
std::visit(overloaded{
  [&](const PatternIdent& patternident) {
auto [binding_member_name, _w0] = patternident;
{
record_ordered_tuple_fragment = record_ordered_tuple_fragment + expr::auto_binding_statement(cpp_naming::cpp_safe(binding_member_name), temp_name + mlc::String(".") + cpp_naming::cpp_safe(ordered_member_labels[member_slot_index]));
}
},
  [&](const auto& _unused) {
{
}
}
}, (*sub_patterns[member_slot_index]));
member_slot_index = member_slot_index + 1;
}
}
  return record_ordered_tuple_fragment;
 }() : [&]() -> mlc::String { 
  mlc::String fallback_binding_names = match_analysis::pattern_bind_names(sub_patterns);
  return mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n") + expr::tuple_destructure_binding(fallback_binding_names, temp_name) + mlc::String("\n");
 }();
 }() : [&]() -> mlc::String { 
  mlc::String outer_fallback_binding_names = match_analysis::pattern_bind_names(sub_patterns);
  return mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n") + expr::tuple_destructure_binding(outer_fallback_binding_names, temp_name) + mlc::String("\n");
 }();
 }(); }();
  return let_pat::GenLetPatternResult{output, codegen_context};
 }(); } if (std::holds_alternative<ast::PatternRecord>((*pattern))) { auto _v_patternrecord = std::get<ast::PatternRecord>((*pattern)); auto [_w0, field_patterns, _w1] = _v_patternrecord; return [&]() -> let_pat::GenLetPatternResult { 
  mlc::String binding_fragment = mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n");
  int index = 0;
  while (index < field_patterns.size()){
{
std::visit(overloaded{
  [&](const PatternIdent& patternident) {
auto [field_name, _w0] = patternident;
{
binding_fragment = binding_fragment + expr::auto_binding_statement(cpp_naming::cpp_safe(field_name), temp_name + mlc::String(".") + cpp_naming::cpp_safe(field_name));
}
},
  [&](const auto& _unused) {
{
}
}
}, (*field_patterns[index]));
index = index + 1;
}
}
  return let_pat::GenLetPatternResult{binding_fragment, codegen_context};
 }(); } if (std::holds_alternative<ast::PatternArray>((*pattern))) { auto _v_patternarray = std::get<ast::PatternArray>((*pattern)); auto [sub_patterns, rest, _w0] = _v_patternarray; return [&]() -> let_pat::GenLetPatternResult { 
  mlc::String element_cpp = [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [inner] = _v_tarray; return type_gen::sem_type_to_cpp(codegen_context, inner); } return mlc::String("auto"); }();
  mlc::String binding_fragment = mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n");
  int index = 0;
  while (index < sub_patterns.size()){
{
std::visit(overloaded{
  [&](const PatternIdent& patternident) {
auto [name, _w0] = patternident;
{
binding_fragment = binding_fragment + mlc::String("auto ") + cpp_naming::cpp_safe(name) + mlc::String(" = ") + temp_name + mlc::String("[") + mlc::to_string(index) + mlc::String("];\n");
}
},
  [&](const auto& _unused) {
{
}
}
}, (*sub_patterns[index]));
index = index + 1;
}
}
  if (rest != mlc::String("")){
{
binding_fragment = binding_fragment + mlc::String("auto ") + cpp_naming::cpp_safe(rest) + mlc::String(" = ") + gen_let_array_rest_slice(temp_name, sub_patterns.size(), element_cpp) + mlc::String(";\n");
}
}
  return let_pat::GenLetPatternResult{binding_fragment, codegen_context};
 }(); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [constructor_name, sub_patterns, _w0] = _v_patternctor; return [&]() -> let_pat::GenLetPatternResult { 
  mlc::String qualified_name = context::CodegenContext_resolve(codegen_context, constructor_name);
  context::TempNameResult second_temp = context::take_temp_name(codegen_context);
  mlc::String variant_binding_name = second_temp.name;
  codegen_context = second_temp.codegen_context;
  mlc::String bind = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), variant_binding_name);
  bool is_generic = decl_index::list_contains(codegen_context.generic_variants, constructor_name);
  mlc::String type_arguments = is_generic ? mlc::String("<auto>") : mlc::String("");
  mlc::String else_code = has_else ? eval_fn(else_body, codegen_context) : mlc::String("std::abort();");
  return let_pat::GenLetPatternResult{mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n") + mlc::String("if (!std::holds_alternative<") + qualified_name + type_arguments + mlc::String(">(") + temp_name + mlc::String(")) { ") + else_code + mlc::String(" }\n") + mlc::String("const ") + qualified_name + type_arguments + mlc::String("& ") + variant_binding_name + mlc::String(" = std::get<") + qualified_name + type_arguments + mlc::String(">(") + temp_name + mlc::String(");\n") + bind + mlc::String("\n"), codegen_context};
 }(); } return let_pat::GenLetPatternResult{expr::suffix_semicolon_newline(value_code), codegen_context}; }();
}

} // namespace let_pat
