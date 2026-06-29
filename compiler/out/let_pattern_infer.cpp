#include "let_pattern_infer.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "pattern_env.hpp"
#include "substitution.hpp"

namespace let_pattern_infer {

using namespace ast;
using namespace registry;
using namespace pattern_env;
using namespace substitution;
using namespace ast_tokens;

struct Tuple_or_slot_pattern_environment_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> pattern_environment;int next_component_index;};

mlc::String pattern_ident_binding_name(std::shared_ptr<ast::Pattern> pattern) noexcept;

bool value_type_is_tuple_type(std::shared_ptr<registry::Type> value_type) noexcept;

bool value_type_is_pair_type(std::shared_ptr<registry::Type> value_type) noexcept;

bool value_type_is_array_type(std::shared_ptr<registry::Type> value_type) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;

std::shared_ptr<registry::Type> pair_first_member_type(std::shared_ptr<registry::Type> value_type) noexcept;

std::shared_ptr<registry::Type> pair_second_member_type(std::shared_ptr<registry::Type> value_type) noexcept;

std::shared_ptr<registry::Type> array_element_type_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept;

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pattern> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_member_types(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_pair(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> first_member_type, std::shared_ptr<registry::Type> second_member_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_record_pattern_to_env(mlc::String record_heading_name_or_empty, mlc::Array<std::shared_ptr<ast::Pattern>> literal_field_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ctor_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ident_pattern_to_env(mlc::String let_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_dispatch(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::String pattern_ident_binding_name(std::shared_ptr<ast::Pattern> pattern) noexcept{return std::visit(overloaded{
  [&](const PatternIdent& patternident) -> mlc::String { auto [binding_name, _w0] = patternident; return binding_name; },
  [&](const PatternWild& patternwild) -> mlc::String { auto [_w0] = patternwild; return mlc::String(""); },
  [&](const PatternInt& patternint) -> mlc::String { auto [_w0, _w1] = patternint; return mlc::String(""); },
  [&](const PatternStr& patternstr) -> mlc::String { auto [_w0, _w1] = patternstr; return mlc::String(""); },
  [&](const PatternStringLit& patternstringlit) -> mlc::String { auto [_w0, _w1] = patternstringlit; return mlc::String(""); },
  [&](const PatternBool& patternbool) -> mlc::String { auto [_w0, _w1] = patternbool; return mlc::String(""); },
  [&](const PatternUnit& patternunit) -> mlc::String { auto [_w0] = patternunit; return mlc::String(""); },
  [&](const PatternCtor& patternctor) -> mlc::String { auto [_w0, _w1, _w2] = patternctor; return mlc::String(""); },
  [&](const PatternRecord& patternrecord) -> mlc::String { auto [_w0, _w1, _w2] = patternrecord; return mlc::String(""); },
  [&](const PatternTuple& patterntuple) -> mlc::String { auto [_w0, _w1] = patterntuple; return mlc::String(""); },
  [&](const PatternArray& patternarray) -> mlc::String { auto [_w0, _w1, _w2] = patternarray; return mlc::String(""); },
  [&](const PatternOr& patternor) -> mlc::String { auto [_w0, _w1] = patternor; return mlc::String(""); }
}, (*pattern));}

bool value_type_is_tuple_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TTuple& ttuple) -> bool { auto [_w0] = ttuple; return true; },
  [&](const TI32& ti32) -> bool { return false; },
  [&](const TString& tstring) -> bool { return false; },
  [&](const TBool& tbool) -> bool { return false; },
  [&](const TUnit& tunit) -> bool { return false; },
  [&](const TI64& ti64) -> bool { return false; },
  [&](const TF64& tf64) -> bool { return false; },
  [&](const TU8& tu8) -> bool { return false; },
  [&](const TUsize& tusize) -> bool { return false; },
  [&](const TChar& tchar) -> bool { return false; },
  [&](const TUnknown& tunknown) -> bool { return false; },
  [&](const TArray& tarray) -> bool { auto [inner] = tarray; return false; },
  [&](const TShared& tshared) -> bool { auto [inner] = tshared; return false; },
  [&](const TNamed& tnamed) -> bool { auto [type_name] = tnamed; return false; },
  [&](const TGeneric& tgeneric) -> bool { auto [type_name, type_arguments] = tgeneric; return false; },
  [&](const TPair& tpair) -> bool { auto [left, right] = tpair; return false; },
  [&](const TFn& tfn) -> bool { auto [param_types, return_type] = tfn; return false; },
  [&](const TAssoc& tassoc) -> bool { auto [param, assoc] = tassoc; return false; }
}, (*value_type));}

bool value_type_is_pair_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TPair& tpair) -> bool { auto [left, right] = tpair; return true; },
  [&](const TI32& ti32) -> bool { return false; },
  [&](const TString& tstring) -> bool { return false; },
  [&](const TBool& tbool) -> bool { return false; },
  [&](const TUnit& tunit) -> bool { return false; },
  [&](const TI64& ti64) -> bool { return false; },
  [&](const TF64& tf64) -> bool { return false; },
  [&](const TU8& tu8) -> bool { return false; },
  [&](const TUsize& tusize) -> bool { return false; },
  [&](const TChar& tchar) -> bool { return false; },
  [&](const TUnknown& tunknown) -> bool { return false; },
  [&](const TArray& tarray) -> bool { auto [inner] = tarray; return false; },
  [&](const TShared& tshared) -> bool { auto [inner] = tshared; return false; },
  [&](const TNamed& tnamed) -> bool { auto [type_name] = tnamed; return false; },
  [&](const TGeneric& tgeneric) -> bool { auto [type_name, type_arguments] = tgeneric; return false; },
  [&](const TTuple& ttuple) -> bool { auto [types] = ttuple; return false; },
  [&](const TFn& tfn) -> bool { auto [param_types, return_type] = tfn; return false; },
  [&](const TAssoc& tassoc) -> bool { auto [param, assoc] = tassoc; return false; }
}, (*value_type));}

bool value_type_is_array_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TArray& tarray) -> bool { auto [inner] = tarray; return true; },
  [&](const TI32& ti32) -> bool { return false; },
  [&](const TString& tstring) -> bool { return false; },
  [&](const TBool& tbool) -> bool { return false; },
  [&](const TUnit& tunit) -> bool { return false; },
  [&](const TI64& ti64) -> bool { return false; },
  [&](const TF64& tf64) -> bool { return false; },
  [&](const TU8& tu8) -> bool { return false; },
  [&](const TUsize& tusize) -> bool { return false; },
  [&](const TChar& tchar) -> bool { return false; },
  [&](const TUnknown& tunknown) -> bool { return false; },
  [&](const TShared& tshared) -> bool { auto [inner] = tshared; return false; },
  [&](const TNamed& tnamed) -> bool { auto [type_name] = tnamed; return false; },
  [&](const TGeneric& tgeneric) -> bool { auto [type_name, type_arguments] = tgeneric; return false; },
  [&](const TPair& tpair) -> bool { auto [left, right] = tpair; return false; },
  [&](const TTuple& ttuple) -> bool { auto [types] = ttuple; return false; },
  [&](const TFn& tfn) -> bool { auto [param_types, return_type] = tfn; return false; },
  [&](const TAssoc& tassoc) -> bool { auto [param, assoc] = tassoc; return false; }
}, (*value_type));}

mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TTuple& ttuple) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [types] = ttuple; return types; },
  [&](const TI32& ti32) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TString& tstring) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TBool& tbool) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUnit& tunit) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TI64& ti64) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TF64& tf64) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TU8& tu8) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUsize& tusize) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TChar& tchar) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUnknown& tunknown) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TArray& tarray) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [inner] = tarray; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TShared& tshared) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [inner] = tshared; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TNamed& tnamed) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [type_name] = tnamed; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TGeneric& tgeneric) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [type_name, type_arguments] = tgeneric; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TPair& tpair) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [left, right] = tpair; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TFn& tfn) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [param_types, return_type] = tfn; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TAssoc& tassoc) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [param, assoc] = tassoc; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); }
}, (*value_type));}

std::shared_ptr<registry::Type> pair_first_member_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [first_member_type, _w0] = tpair; return first_member_type; },
  [&](const TI32& ti32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TString& tstring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TBool& tbool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnit& tunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TI64& ti64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TF64& tf64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TU8& tu8) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUsize& tusize) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TChar& tchar) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [inner] = tarray; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [inner] = tshared; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [type_name] = tnamed; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [type_name, type_arguments] = tgeneric; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [types] = ttuple; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TFn& tfn) -> std::shared_ptr<registry::Type> { auto [param_types, return_type] = tfn; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TAssoc& tassoc) -> std::shared_ptr<registry::Type> { auto [param, assoc] = tassoc; return std::make_shared<registry::Type>((registry::TUnknown{})); }
}, (*value_type));}

std::shared_ptr<registry::Type> pair_second_member_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [_w0, second_member_type] = tpair; return second_member_type; },
  [&](const TI32& ti32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TString& tstring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TBool& tbool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnit& tunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TI64& ti64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TF64& tf64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TU8& tu8) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUsize& tusize) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TChar& tchar) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [inner] = tarray; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [inner] = tshared; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [type_name] = tnamed; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [type_name, type_arguments] = tgeneric; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [types] = ttuple; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TFn& tfn) -> std::shared_ptr<registry::Type> { auto [param_types, return_type] = tfn; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TAssoc& tassoc) -> std::shared_ptr<registry::Type> { auto [param, assoc] = tassoc; return std::make_shared<registry::Type>((registry::TUnknown{})); }
}, (*value_type));}

std::shared_ptr<registry::Type> array_element_type_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [element_type_inside_array] = tarray; return element_type_inside_array; },
  [&](const TI32& ti32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TString& tstring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TBool& tbool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnit& tunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TI64& ti64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TF64& tf64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TU8& tu8) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUsize& tusize) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TChar& tchar) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [inner] = tshared; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [type_name] = tnamed; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [type_name, type_arguments] = tgeneric; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [left, right] = tpair; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [types] = ttuple; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TFn& tfn) -> std::shared_ptr<registry::Type> { auto [param_types, return_type] = tfn; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TAssoc& tassoc) -> std::shared_ptr<registry::Type> { auto [param, assoc] = tassoc; return std::make_shared<registry::Type>((registry::TUnknown{})); }
}, (*value_type));}

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> tuple_member_type = tuple_member_types[state.next_component_index];
return let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern, tuple_member_type, state.pattern_environment, registry), state.next_component_index + 1};
}

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept{return let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(array_cell_pattern, array_element_type, state.pattern_environment, registry), state.next_component_index + 1};}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pattern> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept{
mlc::String record_field_binding_name = pattern_ident_binding_name(field_subpattern);
return record_field_binding_name != mlc::String("") ? [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  std::shared_ptr<registry::Type> field_object_type_from_registry = registry::field_type_from_object(let_rhs_object_type, record_field_binding_name, registry);
  return apply_let_pattern_to_env(field_subpattern, field_object_type_from_registry, field_pattern_environment, registry);
 }() : field_pattern_environment;
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_member_types(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return tuple_component_patterns.size() != tuple_member_types.size() ? base_env : tuple_component_patterns.fold(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [tuple_member_types, registry](let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern) mutable { return let_tuple_environment_fold_step(state, tuple_component_pattern, tuple_member_types, registry); }).pattern_environment;}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_pair(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> first_member_type, std::shared_ptr<registry::Type> second_member_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return tuple_component_patterns.size() != 2 ? base_env : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_first_tuple_component = apply_let_pattern_to_env(tuple_component_patterns[0], first_member_type, base_env, registry);
  return apply_let_pattern_to_env(tuple_component_patterns[1], second_member_type, environment_after_first_tuple_component, registry);
 }();}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return value_type_is_tuple_type(value_type) ? apply_let_tuple_patterns_with_member_types(tuple_component_patterns, tuple_member_types_from_value_type(value_type), base_env, registry) : value_type_is_pair_type(value_type) ? apply_let_tuple_patterns_with_pair(tuple_component_patterns, pair_first_member_type(value_type), pair_second_member_type(value_type), base_env, registry) : tuple_pattern_environment_using_ordered_record_fields(tuple_component_patterns, value_type, base_env, registry);}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> element_type_inside_array = array_element_type_from_value_type(value_type);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_fixed_array_cells = cell_patterns.fold(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [element_type_inside_array, registry](let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> cell_pattern) mutable { return let_array_element_patterns_fold_step(state, cell_pattern, element_type_inside_array, registry); }).pattern_environment;
return remainder_binding_name != mlc::String("") ? [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> remainder_tail_environment = environment_after_fixed_array_cells;
  remainder_tail_environment.set(remainder_binding_name, value_type);
  return remainder_tail_environment;
 }() : environment_after_fixed_array_cells;
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_record_pattern_to_env(mlc::String record_heading_name_or_empty, mlc::Array<std::shared_ptr<ast::Pattern>> literal_field_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return record_heading_name_or_empty != mlc::String("") ? base_env : literal_field_patterns.fold(base_env, [value_type, registry](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment_so_far, std::shared_ptr<ast::Pattern> field_subpattern_under_pattern_node) mutable { return let_record_literal_field_pattern_fold_step(field_pattern_environment_so_far, field_subpattern_under_pattern_node, value_type, registry); });}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ctor_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return pattern_env::env_for_pattern_substituted(base_env, pattern, registry, substitution::substitution_from_generic(value_type, registry), value_type);}

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept{
mlc::String label_for_ordered_slot = ordered_field_labels[state.next_component_index];
std::shared_ptr<registry::Type> inferred_field_member_type = registry::field_type_from_object(let_rhs_record_object_type, label_for_ordered_slot, registry);
return let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern_in_record_order, inferred_field_member_type, state.pattern_environment, registry), state.next_component_index + 1};
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ident_pattern_to_env(mlc::String let_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_where_ident_binding_is_added = base_env;
environment_where_ident_binding_is_added.set(let_binding_name, value_type);
return environment_where_ident_binding_is_added;
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_dispatch(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return value_type_is_array_type(value_type) ? apply_let_array_pattern_to_env(cell_patterns, remainder_binding_name, value_type, base_env, registry) : base_env;}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return std::visit(overloaded{
  [&](const PatternWild& patternwild) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0] = patternwild; return base_env; },
  [&](const PatternIdent& patternident) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [let_binding_name, _w0] = patternident; return apply_let_ident_pattern_to_env(let_binding_name, value_type, base_env); },
  [&](const PatternTuple& patterntuple) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [tuple_component_patterns, _w0] = patterntuple; return apply_let_tuple_pattern_to_env(tuple_component_patterns, value_type, base_env, registry); },
  [&](const PatternArray& patternarray) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [cell_patterns, remainder_binding_name, _w0] = patternarray; return apply_let_array_pattern_dispatch(cell_patterns, remainder_binding_name, value_type, base_env, registry); },
  [&](const PatternRecord& patternrecord) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [record_heading_name_or_empty, literal_field_patterns, _w0] = patternrecord; return apply_let_record_pattern_to_env(record_heading_name_or_empty, literal_field_patterns, value_type, base_env, registry); },
  [&](const PatternCtor& patternctor) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0, _w1, _w2] = patternctor; return apply_let_ctor_pattern_to_env(pattern, value_type, base_env, registry); },
  [&](const PatternInt& patternint) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0, _w1] = patternint; return base_env; },
  [&](const PatternStr& patternstr) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0, _w1] = patternstr; return base_env; },
  [&](const PatternStringLit& patternstringlit) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0, _w1] = patternstringlit; return base_env; },
  [&](const PatternBool& patternbool) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0, _w1] = patternbool; return base_env; },
  [&](const PatternUnit& patternunit) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0] = patternunit; return base_env; },
  [&](const PatternOr& patternor) -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { auto [_w0, _w1] = patternor; return base_env; }
}, (*pattern));}

mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TNamed& tnamed) -> mlc::String { auto [record_name_without_parameters] = tnamed; return record_name_without_parameters; },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [generic_algebraic_outer_name, _w0] = tgeneric; return generic_algebraic_outer_name; },
  [&](const TI32& ti32) -> mlc::String { return mlc::String(""); },
  [&](const TString& tstring) -> mlc::String { return mlc::String(""); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String(""); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String(""); },
  [&](const TI64& ti64) -> mlc::String { return mlc::String(""); },
  [&](const TF64& tf64) -> mlc::String { return mlc::String(""); },
  [&](const TU8& tu8) -> mlc::String { return mlc::String(""); },
  [&](const TUsize& tusize) -> mlc::String { return mlc::String(""); },
  [&](const TChar& tchar) -> mlc::String { return mlc::String(""); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String(""); },
  [&](const TArray& tarray) -> mlc::String { auto [inner] = tarray; return mlc::String(""); },
  [&](const TShared& tshared) -> mlc::String { auto [inner] = tshared; return mlc::String(""); },
  [&](const TPair& tpair) -> mlc::String { auto [left, right] = tpair; return mlc::String(""); },
  [&](const TTuple& ttuple) -> mlc::String { auto [types] = ttuple; return mlc::String(""); },
  [&](const TFn& tfn) -> mlc::String { auto [param_types, return_type] = tfn; return mlc::String(""); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [param, assoc] = tassoc; return mlc::String(""); }
}, (*value_type));}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept{
mlc::String record_lookup_name = record_base_name_for_ordered_tuple_pattern(value_type_shaped_like_record_under_tuple_syntax);
return record_lookup_name == mlc::String("") || !registry::TypeRegistry_has_fields(registry, record_lookup_name) ? base_environment_before_ordered_fields : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<mlc::String> ordered_field_labels_from_registry = registry::TypeRegistry_record_field_names_ordered_for(registry, record_lookup_name);
  return ordered_field_labels_from_registry.size() != tuple_component_patterns.size() ? base_environment_before_ordered_fields : tuple_component_patterns.fold(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{base_environment_before_ordered_fields, 0}, [ordered_field_labels_from_registry, value_type_shaped_like_record_under_tuple_syntax, registry](let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_registry_field_order) mutable { return ordered_record_tuple_environment_fold_step(state, tuple_component_pattern_in_registry_field_order, ordered_field_labels_from_registry, value_type_shaped_like_record_under_tuple_syntax, registry); }).pattern_environment;
 }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return apply_let_pattern_to_env(pattern, value_type, base_env, registry);}

} // namespace let_pattern_infer
