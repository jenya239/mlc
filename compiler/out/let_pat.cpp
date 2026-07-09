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

mlc::String primitive_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code) noexcept;

mlc::String record_codegen_name_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;

bool value_type_is_tuple_or_pair(std::shared_ptr<registry::Type> value_type) noexcept;

mlc::String append_tuple_member_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String member_label, mlc::String temp_name) noexcept;

mlc::String record_ordered_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String record_codegen_name, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;

mlc::String tuple_pattern_output_for_value_type(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;

mlc::String record_or_fallback_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;

mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept;

mlc::String append_array_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String temp_name, int index) noexcept;

mlc::String append_record_field_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept;

mlc::String record_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, mlc::String value_code) noexcept;

mlc::String array_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String rest, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;

mlc::String wild_pattern_output(mlc::String value_code) noexcept;

let_pat::GenLetPatternResult ctor_pattern_gen_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept;

let_pat::GenLetPatternResult gen_let_pattern_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept;

mlc::String gen_let_array_rest_slice(mlc::String temporary_name, int from_index, mlc::String element_cpp) noexcept{return mlc::String("mlc::Array<") + element_cpp + mlc::String(">(") + temporary_name + mlc::String(".cbegin() + ") + mlc::to_string(from_index) + mlc::String(", ") + temporary_name + mlc::String(".cend())");}

mlc::String primitive_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code) noexcept{
mlc::String primitive_tuple_binding_names = match_analysis::pattern_bind_names(sub_patterns);
return mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n") + expr::tuple_destructure_binding(primitive_tuple_binding_names, temp_name) + mlc::String("\n");
}

mlc::String record_codegen_name_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TNamed& tnamed) -> mlc::String { auto [record_nominal_name] = tnamed; return record_nominal_name; },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [record_nominal_name, _w0] = tgeneric; return record_nominal_name; },
  [&](const TI32& ti32) -> mlc::String { return mlc::String(""); },
  [&](const TString& tstring) -> mlc::String { return mlc::String(""); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String(""); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String(""); },
  [&](const TI64& ti64) -> mlc::String { return mlc::String(""); },
  [&](const TF64& tf64) -> mlc::String { return mlc::String(""); },
  [&](const TU8& tu8) -> mlc::String { return mlc::String(""); },
  [&](const TUsize& tusize) -> mlc::String { return mlc::String(""); },
  [&](const TChar& tchar) -> mlc::String { return mlc::String(""); },
  [&](const TArray& tarray) -> mlc::String { auto [_w0] = tarray; return mlc::String(""); },
  [&](const TTuple& ttuple) -> mlc::String { auto [_w0] = ttuple; return mlc::String(""); },
  [&](const TPair& tpair) -> mlc::String { auto [_w0, _w1] = tpair; return mlc::String(""); },
  [&](const TShared& tshared) -> mlc::String { auto [_w0] = tshared; return mlc::String(""); },
  [&](const TFn& tfn) -> mlc::String { auto [_w0, _w1] = tfn; return mlc::String(""); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [_w0, _w1] = tassoc; return mlc::String(""); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String(""); }
}, (*value_type));}

bool value_type_is_tuple_or_pair(std::shared_ptr<registry::Type> value_type) noexcept{return [&]() { if (std::holds_alternative<registry::TTuple>((*value_type))) { auto _v_ttuple = std::get<registry::TTuple>((*value_type)); auto [_w0] = _v_ttuple; return true; } if (std::holds_alternative<registry::TPair>((*value_type))) { auto _v_tpair = std::get<registry::TPair>((*value_type)); auto [_w0, _w1] = _v_tpair; return true; } return false; }();}

mlc::String append_tuple_member_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String member_label, mlc::String temp_name) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> mlc::String { auto [binding_member_name, _w0] = patternident; return fragment + expr::auto_binding_statement(cpp_naming::cpp_safe(binding_member_name), temp_name + mlc::String(".") + cpp_naming::cpp_safe(member_label)); },
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return fragment; },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return fragment; },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return fragment; },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return fragment; },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return fragment; },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return fragment; },
  [&](const PatternCtor& patternctor) -> mlc::String { auto [_w0, _w1, _w2] = patternctor; return fragment; },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [_w0, _w1, _w2] = patternrecord; return fragment; },
  [&](const PatternTuple& patterntuple) -> mlc::String { auto [_w0, _w1] = patterntuple; return fragment; },
  [&](const PatternArray& patternarray) -> mlc::String { auto [_w0, _w1, _w2] = patternarray; return fragment; },
  [&](const PatternOr& patternor) -> mlc::String { auto [_w0, _w1] = patternor; return fragment; }
}, (*sub_pattern));}

mlc::String record_ordered_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String record_codegen_name, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{
mlc::Array<mlc::String> ordered_member_labels = context::lookup_fields_for_context(codegen_context, record_codegen_name);
return ordered_member_labels.size() == sub_patterns.size() && sub_patterns.size() > 0 ? [&]() -> mlc::String { 
  mlc::String record_ordered_tuple_fragment = mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n");
  int member_slot_index = 0;
  while (member_slot_index < sub_patterns.size()){
{
record_ordered_tuple_fragment = append_tuple_member_ident_binding(record_ordered_tuple_fragment, sub_patterns[member_slot_index], ordered_member_labels[member_slot_index], temp_name);
member_slot_index = member_slot_index + 1;
}
}
  return record_ordered_tuple_fragment;
 }() : primitive_tuple_pattern_output(sub_patterns, temp_name, value_code);
}

mlc::String tuple_pattern_output_for_value_type(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{return value_type_is_tuple_or_pair(value_type) ? primitive_tuple_pattern_output(sub_patterns, temp_name, value_code) : record_or_fallback_tuple_pattern_output(sub_patterns, value_type, temp_name, value_code, codegen_context);}

mlc::String record_or_fallback_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{
mlc::String record_codegen_name = record_codegen_name_from_value_type(value_type);
return record_codegen_name != mlc::String("") ? record_ordered_tuple_pattern_output(sub_patterns, record_codegen_name, temp_name, value_code, codegen_context) : primitive_tuple_pattern_output(sub_patterns, temp_name, value_code);
}

mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept{
return [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [inner] = _v_tarray; return type_gen::sem_type_to_cpp(codegen_context, inner); } return mlc::String("auto"); }();
}

mlc::String append_array_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String temp_name, int index) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> mlc::String { auto [name, _w0] = patternident; return fragment + mlc::String("auto ") + cpp_naming::cpp_safe(name) + mlc::String(" = ") + temp_name + mlc::String("[") + mlc::to_string(index) + mlc::String("];\n"); },
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return fragment; },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return fragment; },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return fragment; },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return fragment; },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return fragment; },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return fragment; },
  [&](const PatternCtor& patternctor) -> mlc::String { auto [_w0, _w1, _w2] = patternctor; return fragment; },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [_w0, _w1, _w2] = patternrecord; return fragment; },
  [&](const PatternTuple& patterntuple) -> mlc::String { auto [_w0, _w1] = patterntuple; return fragment; },
  [&](const PatternArray& patternarray) -> mlc::String { auto [_w0, _w1, _w2] = patternarray; return fragment; },
  [&](const PatternOr& patternor) -> mlc::String { auto [_w0, _w1] = patternor; return fragment; }
}, (*sub_pattern));}

mlc::String append_record_field_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> mlc::String { auto [field_name, _w0] = patternident; return fragment + expr::auto_binding_statement(cpp_naming::cpp_safe(field_name), temp_name + mlc::String(".") + cpp_naming::cpp_safe(field_name)); },
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return fragment; },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return fragment; },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return fragment; },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return fragment; },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return fragment; },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return fragment; },
  [&](const PatternCtor& patternctor) -> mlc::String { auto [_w0, _w1, _w2] = patternctor; return fragment; },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [_w0, _w1, _w2] = patternrecord; return fragment; },
  [&](const PatternTuple& patterntuple) -> mlc::String { auto [_w0, _w1] = patterntuple; return fragment; },
  [&](const PatternArray& patternarray) -> mlc::String { auto [_w0, _w1, _w2] = patternarray; return fragment; },
  [&](const PatternOr& patternor) -> mlc::String { auto [_w0, _w1] = patternor; return fragment; }
}, (*field_pattern));}

mlc::String record_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, mlc::String value_code) noexcept{
mlc::String binding_fragment = mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n");
int index = 0;
while (index < field_patterns.size()){
{
binding_fragment = append_record_field_ident_binding(binding_fragment, field_patterns[index], temp_name);
index = index + 1;
}
}
return binding_fragment;
}

mlc::String array_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String rest, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept{
mlc::String element_cpp = array_element_cpp_for_value_type(value_type, codegen_context);
mlc::String binding_fragment = mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n");
int index = 0;
while (index < sub_patterns.size()){
{
binding_fragment = append_array_ident_binding(binding_fragment, sub_patterns[index], temp_name, index);
index = index + 1;
}
}
if (rest != mlc::String("")){
{
binding_fragment = binding_fragment + mlc::String("auto ") + cpp_naming::cpp_safe(rest) + mlc::String(" = ") + gen_let_array_rest_slice(temp_name, sub_patterns.size(), element_cpp) + mlc::String(";\n");
}
}
return binding_fragment;
}

mlc::String wild_pattern_output(mlc::String value_code) noexcept{return expr::suffix_semicolon_newline(value_code);}

let_pat::GenLetPatternResult ctor_pattern_gen_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(codegen_context, constructor_name);
context::TempNameResult second_temp = context::take_temp_name(codegen_context);
mlc::String variant_binding_name = second_temp.name;
context::CodegenContext updated_codegen_context = second_temp.codegen_context;
mlc::String bind = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), variant_binding_name);
bool is_generic = decl_index::list_contains(updated_codegen_context.generic_variants, constructor_name);
mlc::String type_arguments = is_generic ? mlc::String("<auto>") : mlc::String("");
mlc::String else_code = has_else ? eval_fn(else_body, updated_codegen_context) : mlc::String("std::abort();");
return let_pat::GenLetPatternResult{mlc::String("auto ") + temp_name + mlc::String(" = ") + value_code + mlc::String(";\n") + mlc::String("if (!std::holds_alternative<") + qualified_name + type_arguments + mlc::String(">(") + temp_name + mlc::String(")) { ") + else_code + mlc::String(" }\n") + mlc::String("const ") + qualified_name + type_arguments + mlc::String("& ") + variant_binding_name + mlc::String(" = std::get<") + qualified_name + type_arguments + mlc::String(">(") + temp_name + mlc::String(");\n") + bind + mlc::String("\n"), updated_codegen_context};
}

let_pat::GenLetPatternResult gen_let_pattern_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept{
mlc::String value_code = eval_fn(value, context);
context::TempNameResult first_temp = context::take_temp_name(context);
mlc::String temp_name = first_temp.name;
context::CodegenContext codegen_context = first_temp.codegen_context;
return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> let_pat::GenLetPatternResult { auto [binding_name, _w0] = patternident; return let_pat::GenLetPatternResult{expr::auto_binding_statement(cpp_naming::cpp_safe(binding_name), value_code), codegen_context}; },
  [&](const PatternTuple& patterntuple) -> let_pat::GenLetPatternResult { auto [sub_patterns, _w0] = patterntuple; return let_pat::GenLetPatternResult{tuple_pattern_output_for_value_type(sub_patterns, value_type, temp_name, value_code, codegen_context), codegen_context}; },
  [&](const PatternRecord& patternrecord) -> let_pat::GenLetPatternResult { auto [_w0, field_patterns, _w1] = patternrecord; return let_pat::GenLetPatternResult{record_pattern_output(field_patterns, temp_name, value_code), codegen_context}; },
  [&](const PatternArray& patternarray) -> let_pat::GenLetPatternResult { auto [sub_patterns, rest, _w0] = patternarray; return let_pat::GenLetPatternResult{array_pattern_output(sub_patterns, rest, value_type, temp_name, value_code, codegen_context), codegen_context}; },
  [&](const PatternCtor& patternctor) -> let_pat::GenLetPatternResult { auto [constructor_name, sub_patterns, _w0] = patternctor; return ctor_pattern_gen_result(constructor_name, sub_patterns, temp_name, value_code, has_else, else_body, codegen_context, eval_fn); },
  [&](const PatternWild& patternwild) -> let_pat::GenLetPatternResult { auto [_w0] = patternwild; return let_pat::GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
  [&](const PatternInt& patternint) -> let_pat::GenLetPatternResult { auto [_w0, _w1] = patternint; return let_pat::GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
  [&](const PatternStr& patternstr) -> let_pat::GenLetPatternResult { auto [_w0, _w1] = patternstr; return let_pat::GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
  [&](const PatternStringLit& patternstringlit) -> let_pat::GenLetPatternResult { auto [_w0, _w1] = patternstringlit; return let_pat::GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
  [&](const PatternBool& patternbool) -> let_pat::GenLetPatternResult { auto [_w0, _w1] = patternbool; return let_pat::GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
  [&](const PatternUnit& patternunit) -> let_pat::GenLetPatternResult { auto [_w0] = patternunit; return let_pat::GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; },
  [&](const PatternOr& patternor) -> let_pat::GenLetPatternResult { auto [_w0, _w1] = patternor; return let_pat::GenLetPatternResult{wild_pattern_output(value_code), codegen_context}; }
}, (*pattern));
}

} // namespace let_pat
