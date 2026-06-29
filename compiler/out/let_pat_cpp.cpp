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

mlc::String print_statements_as_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;

std::shared_ptr<cpp_ast::CppExpression> member_access(mlc::String object_name, mlc::String field_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> index_access(mlc::String object_name, int index) noexcept;

std::shared_ptr<cpp_ast::CppExpression> method_call(mlc::String object_name, mlc::String method_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> variant_holds_alternative_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> variant_get_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> array_rest_slice_expression(mlc::String array_name, int from_index, mlc::String element_cpp_type) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> extra) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> constructor_mismatch_else_statements(bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept;

std::shared_ptr<cpp_ast::CppStatement> variant_get_binding_statement(mlc::String variant_type_name, mlc::String binding_name, mlc::String subject_name) noexcept;

std::shared_ptr<cpp_ast::CppExpression> tuple_element_access(mlc::String source_name, int element_index) noexcept;

bool pattern_is_identifier(std::shared_ptr<ast::Pattern> pattern) noexcept;

bool tuple_subpatterns_are_simple_identifiers(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> simple_tuple_ident_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept;

std::shared_ptr<cpp_ast::CppStatement> tuple_ident_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String source_name, int index) noexcept;

std::shared_ptr<cpp_ast::CppStatement> tuple_structured_binding_statement(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept;

std::shared_ptr<cpp_ast::CppStatement> let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, std::shared_ptr<cpp_ast::CppExpression> value_expression, mlc::String temp_name) noexcept;

mlc::String record_codegen_name_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;

bool value_type_is_tuple_or_pair(std::shared_ptr<registry::Type> value_type) noexcept;

mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept;

let_pat_cpp::GenLetPatternCppResult tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> record_member_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::Array<mlc::String> ordered_member_labels, mlc::String temp_name) noexcept;

std::shared_ptr<cpp_ast::CppStatement> record_member_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String member_label, mlc::String temp_name) noexcept;

let_pat_cpp::GenLetPatternCppResult record_or_fallback_tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;

let_pat_cpp::GenLetPatternCppResult record_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;

std::shared_ptr<cpp_ast::CppStatement> record_field_binding_statement(std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept;

let_pat_cpp::GenLetPatternCppResult array_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String rest_name, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;

std::shared_ptr<cpp_ast::CppStatement> array_element_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String temp_name, int index) noexcept;

let_pat_cpp::GenLetPatternCppResult ctor_pattern_cpp_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept;

let_pat_cpp::GenLetPatternCppResult gen_let_pattern_cpp_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept;

mlc::String print_statements_as_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{return statements.map([](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return print::print_statement(statement) + mlc::String("\n"); }).join(mlc::String(""));}

std::shared_ptr<cpp_ast::CppExpression> member_access(mlc::String object_name, mlc::String field_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(emit_helpers::make_identifier_cpp_expression(object_name), field_name, false));}

std::shared_ptr<cpp_ast::CppExpression> index_access(mlc::String object_name, int index) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIndex(emit_helpers::make_identifier_cpp_expression(object_name), emit_helpers::make_integer_cpp_expression(index)));}

std::shared_ptr<cpp_ast::CppExpression> method_call(mlc::String object_name, mlc::String method_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(emit_helpers::make_identifier_cpp_expression(object_name), method_name, false)), {}));}

std::shared_ptr<cpp_ast::CppExpression> variant_holds_alternative_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("std::holds_alternative<") + variant_type_name + mlc::String(">"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{emit_helpers::make_identifier_cpp_expression(subject_name)}));}

std::shared_ptr<cpp_ast::CppExpression> variant_get_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("std::get<") + variant_type_name + mlc::String(">"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{emit_helpers::make_identifier_cpp_expression(subject_name)}));}

std::shared_ptr<cpp_ast::CppExpression> array_rest_slice_expression(mlc::String array_name, int from_index, mlc::String element_cpp_type) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("mlc::Array<") + element_cpp_type + mlc::String(">"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(mlc::String("+"), method_call(array_name, mlc::String("cbegin")), emit_helpers::make_integer_cpp_expression(from_index))), method_call(array_name, mlc::String("cend"))}));}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> extra) noexcept{
int index = 0;
while (index < extra.size()){
{
output.push_back(extra[index]);
index = index + 1;
}
}
return output;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> constructor_mismatch_else_statements(bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept{return has_else ? mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_expression_cpp_statement(expression_codegen(else_body, context))} : mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_expression_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("std::abort"))), {})))};}

std::shared_ptr<cpp_ast::CppStatement> variant_get_binding_statement(mlc::String variant_type_name, mlc::String binding_name, mlc::String subject_name) noexcept{return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppConstDecl(binding_name, std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRef(std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(variant_type_name)))), variant_get_expression(variant_type_name, subject_name)));}

std::shared_ptr<cpp_ast::CppExpression> tuple_element_access(mlc::String source_name, int element_index) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("std::get<") + mlc::to_string(element_index) + mlc::String(">"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{emit_helpers::make_identifier_cpp_expression(source_name)}));}

bool pattern_is_identifier(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [_w0, _w1] = _v_patternident; return true; } return false; }();}

bool tuple_subpatterns_are_simple_identifiers(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns) noexcept{
int index = 0;
bool all_simple = true;
while (index < subpatterns.size()){
{
if (!pattern_is_identifier(subpatterns[index])){
{
all_simple = false;
}
}
index = index + 1;
}
}
return all_simple;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> simple_tuple_ident_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
int index = 0;
while (index < subpatterns.size()){
{
statements.push_back(tuple_ident_binding_statement(subpatterns[index], source_name, index));
index = index + 1;
}
}
return statements;
}

std::shared_ptr<cpp_ast::CppStatement> tuple_ident_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String source_name, int index) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> std::shared_ptr<cpp_ast::CppStatement> { auto [binding_name, _w0] = patternident; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(binding_name), tuple_element_access(source_name, index)); },
  [&](const PatternWild& patternwild) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternwild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternInt& patternint) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternint; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStr& patternstr) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStringLit& patternstringlit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstringlit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternBool& patternbool) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternbool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternUnit& patternunit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternunit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternCtor& patternctor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternctor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternRecord& patternrecord) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternrecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternTuple& patterntuple) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patterntuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternArray& patternarray) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternarray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternOr& patternor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*subpattern));}

std::shared_ptr<cpp_ast::CppStatement> tuple_structured_binding_statement(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept{return emit_helpers::make_structured_binding_cpp_statement(match_analysis::pattern_binding_name_list(subpatterns), emit_helpers::make_identifier_cpp_expression(source_name));}

std::shared_ptr<cpp_ast::CppStatement> let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, std::shared_ptr<cpp_ast::CppExpression> value_expression, mlc::String temp_name) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> block_statements = {};
block_statements.push_back(emit_helpers::make_auto_cpp_statement(temp_name, value_expression));
int index = 0;
while (index < statements.size()){
{
block_statements.push_back(statements[index]);
index = index + 1;
}
}
return emit_helpers::make_fragment_cpp_statement(print_statements_as_fragment(block_statements));
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

mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept{
return [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [inner_type] = _v_tarray; return type_gen::sem_type_to_cpp(codegen_context, inner_type); } return mlc::String("auto"); }();
}

let_pat_cpp::GenLetPatternCppResult tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{return value_type_is_tuple_or_pair(value_type) ? tuple_subpatterns_are_simple_identifiers(subpatterns) ? let_pat_cpp::GenLetPatternCppResult{let_pattern_block(simple_tuple_ident_binding_statements(subpatterns, temp_name), value_expression, temp_name), codegen_context} : let_pat_cpp::GenLetPatternCppResult{let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{tuple_structured_binding_statement(subpatterns, temp_name)}, value_expression, temp_name), codegen_context} : record_or_fallback_tuple_pattern_cpp_result(subpatterns, value_type, temp_name, value_expression, codegen_context);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> record_member_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::Array<mlc::String> ordered_member_labels, mlc::String temp_name) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> member_statements = {};
int member_slot_index = 0;
while (member_slot_index < subpatterns.size()){
{
member_statements.push_back(record_member_binding_statement(subpatterns[member_slot_index], ordered_member_labels[member_slot_index], temp_name));
member_slot_index = member_slot_index + 1;
}
}
return member_statements;
}

std::shared_ptr<cpp_ast::CppStatement> record_member_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String member_label, mlc::String temp_name) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> std::shared_ptr<cpp_ast::CppStatement> { auto [binding_member_name, _w0] = patternident; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(binding_member_name), member_access(temp_name, member_label)); },
  [&](const PatternWild& patternwild) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternwild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternInt& patternint) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternint; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStr& patternstr) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStringLit& patternstringlit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstringlit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternBool& patternbool) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternbool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternUnit& patternunit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternunit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternCtor& patternctor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternctor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternRecord& patternrecord) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternrecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternTuple& patterntuple) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patterntuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternArray& patternarray) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternarray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternOr& patternor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*subpattern));}

let_pat_cpp::GenLetPatternCppResult record_or_fallback_tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{
mlc::String record_codegen_name = record_codegen_name_from_value_type(value_type);
mlc::Array<mlc::String> ordered_member_labels = context::lookup_fields_for_context(codegen_context, record_codegen_name);
return record_codegen_name != mlc::String("") && ordered_member_labels.size() == subpatterns.size() && subpatterns.size() > 0 ? let_pat_cpp::GenLetPatternCppResult{let_pattern_block(record_member_binding_statements(subpatterns, ordered_member_labels, temp_name), value_expression, temp_name), codegen_context} : let_pat_cpp::GenLetPatternCppResult{let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{tuple_structured_binding_statement(subpatterns, temp_name)}, value_expression, temp_name), codegen_context};
}

let_pat_cpp::GenLetPatternCppResult record_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> field_statements = {};
int index = 0;
while (index < field_patterns.size()){
{
field_statements.push_back(record_field_binding_statement(field_patterns[index], temp_name));
index = index + 1;
}
}
return let_pat_cpp::GenLetPatternCppResult{let_pattern_block(field_statements, value_expression, temp_name), codegen_context};
}

std::shared_ptr<cpp_ast::CppStatement> record_field_binding_statement(std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> std::shared_ptr<cpp_ast::CppStatement> { auto [field_name, _w0] = patternident; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(field_name), member_access(temp_name, field_name)); },
  [&](const PatternWild& patternwild) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternwild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternInt& patternint) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternint; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStr& patternstr) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStringLit& patternstringlit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstringlit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternBool& patternbool) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternbool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternUnit& patternunit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternunit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternCtor& patternctor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternctor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternRecord& patternrecord) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternrecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternTuple& patterntuple) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patterntuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternArray& patternarray) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternarray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternOr& patternor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*field_pattern));}

let_pat_cpp::GenLetPatternCppResult array_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String rest_name, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept{
mlc::String element_cpp = array_element_cpp_for_value_type(value_type, codegen_context);
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> array_statements = {};
int index = 0;
while (index < subpatterns.size()){
{
array_statements.push_back(array_element_binding_statement(subpatterns[index], temp_name, index));
index = index + 1;
}
}
if (rest_name != mlc::String("")){
{
array_statements.push_back(emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(rest_name), array_rest_slice_expression(temp_name, subpatterns.size(), element_cpp)));
}
}
return let_pat_cpp::GenLetPatternCppResult{let_pattern_block(array_statements, value_expression, temp_name), codegen_context};
}

std::shared_ptr<cpp_ast::CppStatement> array_element_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String temp_name, int index) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> std::shared_ptr<cpp_ast::CppStatement> { auto [name, _w0] = patternident; return emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), index_access(temp_name, index)); },
  [&](const PatternWild& patternwild) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternwild; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternInt& patternint) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternint; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStr& patternstr) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstr; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternStringLit& patternstringlit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternstringlit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternBool& patternbool) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternbool; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternUnit& patternunit) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0] = patternunit; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternCtor& patternctor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternctor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternRecord& patternrecord) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternrecord; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternTuple& patterntuple) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patterntuple; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternArray& patternarray) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1, _w2] = patternarray; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); },
  [&](const PatternOr& patternor) -> std::shared_ptr<cpp_ast::CppStatement> { auto [_w0, _w1] = patternor; return emit_helpers::make_expression_cpp_statement(emit_helpers::make_integer_cpp_expression(0)); }
}, (*subpattern));}

let_pat_cpp::GenLetPatternCppResult ctor_pattern_cpp_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept{
mlc::String qualified_name = context::CodegenContext_resolve(codegen_context, constructor_name);
bool is_generic = decl_index::list_contains(codegen_context.generic_variants, constructor_name);
mlc::String type_arguments = is_generic ? mlc::String("<auto>") : mlc::String("");
mlc::String variant_type_name = qualified_name + type_arguments;
context::TempNameResult second_temp = context::take_temp_name(codegen_context);
mlc::String variant_binding_name = second_temp.name;
context::CodegenContext updated_codegen_context = second_temp.codegen_context;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> constructor_statements = {};
constructor_statements.push_back(emit_helpers::make_auto_cpp_statement(temp_name, value_expression));
constructor_statements.push_back(emit_helpers::make_if_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppUnary(mlc::String("!"), variant_holds_alternative_expression(variant_type_name, temp_name))), emit_helpers::make_block_cpp_statement(constructor_mismatch_else_statements(has_else, else_body, updated_codegen_context, expression_codegen)), emit_helpers::make_block_cpp_statement({})));
constructor_statements.push_back(variant_get_binding_statement(variant_type_name, variant_binding_name, temp_name));
if (subpatterns.size() > 0){
{
if (tuple_subpatterns_are_simple_identifiers(subpatterns)){
constructor_statements = append_statements(constructor_statements, simple_tuple_ident_binding_statements(subpatterns, variant_binding_name));
} else {
constructor_statements.push_back(tuple_structured_binding_statement(subpatterns, variant_binding_name));
}
}
}
return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_fragment_cpp_statement(print_statements_as_fragment(constructor_statements)), updated_codegen_context};
}

let_pat_cpp::GenLetPatternCppResult gen_let_pattern_cpp_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept{
std::shared_ptr<cpp_ast::CppExpression> value_expression = expression_codegen(value, context);
context::TempNameResult first_temp = context::take_temp_name(context);
mlc::String temp_name = first_temp.name;
context::CodegenContext codegen_context = first_temp.codegen_context;
return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> let_pat_cpp::GenLetPatternCppResult { auto [name, _w0] = patternident; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), value_expression), codegen_context}; },
  [&](const PatternTuple& patterntuple) -> let_pat_cpp::GenLetPatternCppResult { auto [subpatterns, _w0] = patterntuple; return tuple_pattern_cpp_result(subpatterns, value_type, temp_name, value_expression, codegen_context); },
  [&](const PatternRecord& patternrecord) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0, field_patterns, _w1] = patternrecord; return record_pattern_cpp_result(field_patterns, temp_name, value_expression, codegen_context); },
  [&](const PatternArray& patternarray) -> let_pat_cpp::GenLetPatternCppResult { auto [subpatterns, rest_name, _w0] = patternarray; return array_pattern_cpp_result(subpatterns, rest_name, value_type, temp_name, value_expression, codegen_context); },
  [&](const PatternCtor& patternctor) -> let_pat_cpp::GenLetPatternCppResult { auto [constructor_name, subpatterns, _w0] = patternctor; return ctor_pattern_cpp_result(constructor_name, subpatterns, temp_name, value_expression, has_else, else_body, codegen_context, expression_codegen); },
  [&](const PatternWild& patternwild) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0] = patternwild; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
  [&](const PatternInt& patternint) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0, _w1] = patternint; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
  [&](const PatternStr& patternstr) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0, _w1] = patternstr; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
  [&](const PatternStringLit& patternstringlit) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0, _w1] = patternstringlit; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
  [&](const PatternBool& patternbool) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0, _w1] = patternbool; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
  [&](const PatternUnit& patternunit) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0] = patternunit; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; },
  [&](const PatternOr& patternor) -> let_pat_cpp::GenLetPatternCppResult { auto [_w0, _w1] = patternor; return let_pat_cpp::GenLetPatternCppResult{emit_helpers::make_expression_cpp_statement(value_expression), codegen_context}; }
}, (*pattern));
}

} // namespace let_pat_cpp
