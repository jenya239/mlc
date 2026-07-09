#define main mlc_user_main
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

mlc::String pattern_ident_binding_name(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return std::visit(overloaded{[&](const ast::PatternIdent& patternIdent) { auto [binding_name, __1] = patternIdent; return binding_name; },
[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return mlc::String("", 0); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return mlc::String("", 0); },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return mlc::String("", 0); },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return mlc::String("", 0); },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return mlc::String("", 0); },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return mlc::String("", 0); },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return mlc::String("", 0); },
[&](const ast::PatternRecord& patternRecord) { auto [__0, __1, __2] = patternRecord; return mlc::String("", 0); },
[&](const ast::PatternTuple& patternTuple) { auto [__0, __1] = patternTuple; return mlc::String("", 0); },
[&](const ast::PatternArray& patternArray) { auto [__0, __1, __2] = patternArray; return mlc::String("", 0); },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return mlc::String("", 0); }
}, (*pattern));
}
bool value_type_is_tuple_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return true; },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TUnknown& tUnknown) { return false; },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return false; },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return false; },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return false; },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return false; },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return false; }
}, (*value_type));
}
bool value_type_is_pair_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TPair& tPair) { auto [left, right] = tPair; return true; },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TUnknown& tUnknown) { return false; },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return false; },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return false; },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return false; },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return false; },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return false; }
}, (*value_type));
}
bool value_type_is_array_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return true; },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TUnknown& tUnknown) { return false; },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return false; },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return false; },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return false; },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return false; },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return false; }
}, (*value_type));
}
mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return types; },
[&](const registry::TI32& tI32) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TString& tString) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TBool& tBool) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUnit& tUnit) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TI64& tI64) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TF64& tF64) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TU8& tU8) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUsize& tUsize) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TChar& tChar) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TUnknown& tUnknown) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return [&]() {
auto empty = mlc::Array<std::shared_ptr<registry::Type>>{};
return empty;
}(); }
}, (*value_type));
}
std::shared_ptr<registry::Type> pair_first_member_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TPair& tPair) { auto [first_member_type, __1] = tPair; return first_member_type; },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*value_type));
}
std::shared_ptr<registry::Type> pair_second_member_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TPair& tPair) { auto [__0, second_member_type] = tPair; return second_member_type; },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*value_type));
}
std::shared_ptr<registry::Type> array_element_type_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [element_type_inside_array] = tArray; return element_type_inside_array; },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*value_type));
}
Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept{
  auto tuple_member_type = tuple_member_types[state.next_component_index];
  return Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern, tuple_member_type, state.pattern_environment, registry), (state.next_component_index + 1)};
}
Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept{
  return Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(array_cell_pattern, array_element_type, state.pattern_environment, registry), (state.next_component_index + 1)};
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pattern> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept{
  auto record_field_binding_name = pattern_ident_binding_name(field_subpattern);
  if ((record_field_binding_name != mlc::String("", 0)))   {
    auto field_object_type_from_registry = registry::field_type_from_object(let_rhs_object_type, record_field_binding_name, registry);
    return apply_let_pattern_to_env(field_subpattern, field_object_type_from_registry, field_pattern_environment, registry);
  } else   {
    return field_pattern_environment;
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_member_types(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  if ((tuple_component_patterns.length() != tuple_member_types.length()))   {
    return base_env;
  } else   {
    return tuple_component_patterns.fold(Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [=](Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern) mutable { return let_tuple_environment_fold_step(state, tuple_component_pattern, tuple_member_types, registry); }).pattern_environment;
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_pair(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> first_member_type, std::shared_ptr<registry::Type> second_member_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  if ((tuple_component_patterns.length() != 2))   {
    return base_env;
  } else   {
    auto environment_after_first_tuple_component = apply_let_pattern_to_env(tuple_component_patterns[0], first_member_type, base_env, registry);
    return apply_let_pattern_to_env(tuple_component_patterns[1], second_member_type, environment_after_first_tuple_component, registry);
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  if (value_type_is_tuple_type(value_type))   {
    return apply_let_tuple_patterns_with_member_types(tuple_component_patterns, tuple_member_types_from_value_type(value_type), base_env, registry);
  } else if (value_type_is_pair_type(value_type))   {
    return apply_let_tuple_patterns_with_pair(tuple_component_patterns, pair_first_member_type(value_type), pair_second_member_type(value_type), base_env, registry);
  } else   {
    return tuple_pattern_environment_using_ordered_record_fields(tuple_component_patterns, value_type, base_env, registry);
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  auto element_type_inside_array = array_element_type_from_value_type(value_type);
  auto environment_after_fixed_array_cells = cell_patterns.fold(Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [=](Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> cell_pattern) mutable { return let_array_element_patterns_fold_step(state, cell_pattern, element_type_inside_array, registry); }).pattern_environment;
  if ((remainder_binding_name != mlc::String("", 0)))   {
    auto remainder_tail_environment = environment_after_fixed_array_cells;
    remainder_tail_environment.set(remainder_binding_name, value_type);
    return remainder_tail_environment;
  } else   {
    return environment_after_fixed_array_cells;
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_record_pattern_to_env(mlc::String record_heading_name_or_empty, mlc::Array<std::shared_ptr<ast::Pattern>> literal_field_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  if ((record_heading_name_or_empty != mlc::String("", 0)))   {
    return base_env;
  } else   {
    return literal_field_patterns.fold(base_env, [=](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment_so_far, std::shared_ptr<ast::Pattern> field_subpattern_under_pattern_node) mutable { return let_record_literal_field_pattern_fold_step(field_pattern_environment_so_far, field_subpattern_under_pattern_node, value_type, registry); });
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ctor_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  return pattern_env::env_for_pattern_substituted(base_env, pattern, registry, substitution::substitution_from_generic(value_type, registry), value_type);
}
Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept{
  auto label_for_ordered_slot = ordered_field_labels[state.next_component_index];
  auto inferred_field_member_type = registry::field_type_from_object(let_rhs_record_object_type, label_for_ordered_slot, registry);
  return Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern_in_record_order, inferred_field_member_type, state.pattern_environment, registry), (state.next_component_index + 1)};
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ident_pattern_to_env(mlc::String let_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env) noexcept{
  auto environment_where_ident_binding_is_added = base_env;
  environment_where_ident_binding_is_added.set(let_binding_name, value_type);
  return environment_where_ident_binding_is_added;
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_dispatch(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  if (value_type_is_array_type(value_type))   {
    return apply_let_array_pattern_to_env(cell_patterns, remainder_binding_name, value_type, base_env, registry);
  } else   {
    return base_env;
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  return std::visit(overloaded{[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return base_env; },
[&](const ast::PatternIdent& patternIdent) { auto [let_binding_name, __1] = patternIdent; return apply_let_ident_pattern_to_env(let_binding_name, value_type, base_env); },
[&](const ast::PatternTuple& patternTuple) { auto [tuple_component_patterns, __1] = patternTuple; return apply_let_tuple_pattern_to_env(tuple_component_patterns, value_type, base_env, registry); },
[&](const ast::PatternArray& patternArray) { auto [cell_patterns, remainder_binding_name, __2] = patternArray; return apply_let_array_pattern_dispatch(cell_patterns, remainder_binding_name, value_type, base_env, registry); },
[&](const ast::PatternRecord& patternRecord) { auto [record_heading_name_or_empty, literal_field_patterns, __2] = patternRecord; return apply_let_record_pattern_to_env(record_heading_name_or_empty, literal_field_patterns, value_type, base_env, registry); },
[&](const ast::PatternCtor& patternCtor) { auto [__0, __1, __2] = patternCtor; return apply_let_ctor_pattern_to_env(pattern, value_type, base_env, registry); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return base_env; },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return base_env; },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return base_env; },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return base_env; },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return base_env; },
[&](const ast::PatternOr& patternOr) { auto [__0, __1] = patternOr; return base_env; }
}, (*pattern));
}
mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [record_name_without_parameters] = tNamed; return record_name_without_parameters; },
[&](const registry::TGeneric& tGeneric) { auto [generic_algebraic_outer_name, __1] = tGeneric; return generic_algebraic_outer_name; },
[&](const registry::TI32& tI32) { return mlc::String("", 0); },
[&](const registry::TString& tString) { return mlc::String("", 0); },
[&](const registry::TBool& tBool) { return mlc::String("", 0); },
[&](const registry::TUnit& tUnit) { return mlc::String("", 0); },
[&](const registry::TI64& tI64) { return mlc::String("", 0); },
[&](const registry::TF64& tF64) { return mlc::String("", 0); },
[&](const registry::TU8& tU8) { return mlc::String("", 0); },
[&](const registry::TUsize& tUsize) { return mlc::String("", 0); },
[&](const registry::TChar& tChar) { return mlc::String("", 0); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("", 0); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return mlc::String("", 0); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return mlc::String("", 0); },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return mlc::String("", 0); },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return mlc::String("", 0); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return mlc::String("", 0); },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return mlc::String("", 0); }
}, (*value_type));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept{
  auto record_lookup_name = record_base_name_for_ordered_tuple_pattern(value_type_shaped_like_record_under_tuple_syntax);
  if (((record_lookup_name == mlc::String("", 0)) || (!registry::TypeRegistry_has_fields(registry, record_lookup_name))))   {
    return base_environment_before_ordered_fields;
  } else   {
    auto ordered_field_labels_from_registry = registry::TypeRegistry_record_field_names_ordered_for(registry, record_lookup_name);
    if ((ordered_field_labels_from_registry.length() != tuple_component_patterns.length()))     {
      return base_environment_before_ordered_fields;
    } else     {
      return tuple_component_patterns.fold(Tuple_or_slot_pattern_environment_fold_state{base_environment_before_ordered_fields, 0}, [=](Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_registry_field_order) mutable { return ordered_record_tuple_environment_fold_step(state, tuple_component_pattern_in_registry_field_order, ordered_field_labels_from_registry, value_type_shaped_like_record_under_tuple_syntax, registry); }).pattern_environment;
    }
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
  return apply_let_pattern_to_env(pattern, value_type, base_env, registry);
}

} // namespace let_pattern_infer
