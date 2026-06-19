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

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept;

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pattern> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept;

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> tuple_member_type = tuple_member_types[state.next_component_index];
return let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern, tuple_member_type, state.pattern_environment, registry), state.next_component_index + 1};
}

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept{return let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(array_cell_pattern, array_element_type, state.pattern_environment, registry), state.next_component_index + 1};}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pattern> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept{
mlc::String record_field_binding_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatternIdent>((*field_subpattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*field_subpattern)); auto [binding_name_for_field_literal, _w0] = _v_patternident; return binding_name_for_field_literal; } return mlc::String(""); }();
return record_field_binding_name != mlc::String("") ? [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  std::shared_ptr<registry::Type> field_object_type_from_registry = registry::field_type_from_object(let_rhs_object_type, record_field_binding_name, registry);
  return apply_let_pattern_to_env(field_subpattern, field_object_type_from_registry, field_pattern_environment, registry);
 }() : field_pattern_environment;
}

let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept{
mlc::String label_for_ordered_slot = ordered_field_labels[state.next_component_index];
std::shared_ptr<registry::Type> inferred_field_member_type = registry::field_type_from_object(let_rhs_record_object_type, label_for_ordered_slot, registry);
return let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern_in_record_order, inferred_field_member_type, state.pattern_environment, registry), state.next_component_index + 1};
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatternWild>((*pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*pattern)); auto [_w0] = _v_patternwild; return base_env; } if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [let_binding_name, _w0] = _v_patternident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_where_ident_binding_is_added = base_env;
  environment_where_ident_binding_is_added.set(let_binding_name, value_type);
  return environment_where_ident_binding_is_added;
 }(); } if (std::holds_alternative<ast::PatternTuple>((*pattern))) { auto _v_patterntuple = std::get<ast::PatternTuple>((*pattern)); auto [tuple_component_patterns, _w0] = _v_patterntuple; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<registry::TTuple>((*value_type))) { auto _v_ttuple = std::get<registry::TTuple>((*value_type)); auto [tuple_member_types] = _v_ttuple; return tuple_component_patterns.size() != tuple_member_types.size() ? base_env : tuple_component_patterns.fold(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [tuple_member_types, registry](let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern) mutable { return let_tuple_environment_fold_step(state, tuple_component_pattern, tuple_member_types, registry); }).pattern_environment; } if (std::holds_alternative<registry::TPair>((*value_type))) { auto _v_tpair = std::get<registry::TPair>((*value_type)); auto [first_member_type, second_member_type] = _v_tpair; return tuple_component_patterns.size() != 2 ? base_env : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_first_tuple_component = apply_let_pattern_to_env(tuple_component_patterns[0], first_member_type, base_env, registry);
  return apply_let_pattern_to_env(tuple_component_patterns[1], second_member_type, environment_after_first_tuple_component, registry);
 }(); } return tuple_pattern_environment_using_ordered_record_fields(tuple_component_patterns, value_type, base_env, registry); }(); } if (std::holds_alternative<ast::PatternArray>((*pattern))) { auto _v_patternarray = std::get<ast::PatternArray>((*pattern)); auto [cell_patterns, remainder_binding_name, _w0] = _v_patternarray; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [element_type_inside_array] = _v_tarray; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_fixed_array_cells = cell_patterns.fold(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [element_type_inside_array, registry](let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> cell_pattern) mutable { return let_array_element_patterns_fold_step(state, cell_pattern, element_type_inside_array, registry); }).pattern_environment;
  return remainder_binding_name != mlc::String("") ? [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> remainder_tail_environment = environment_after_fixed_array_cells;
  remainder_tail_environment.set(remainder_binding_name, value_type);
  return remainder_tail_environment;
 }() : environment_after_fixed_array_cells;
 }(); } return base_env; }(); } if (std::holds_alternative<ast::PatternRecord>((*pattern))) { auto _v_patternrecord = std::get<ast::PatternRecord>((*pattern)); auto [record_heading_name_or_empty, literal_field_patterns, _w0] = _v_patternrecord; return record_heading_name_or_empty != mlc::String("") ? base_env : literal_field_patterns.fold(base_env, [value_type, registry](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment_so_far, std::shared_ptr<ast::Pattern> field_subpattern_under_pattern_node) mutable { return let_record_literal_field_pattern_fold_step(field_pattern_environment_so_far, field_subpattern_under_pattern_node, value_type, registry); }); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [_w0, _w1, _w2] = _v_patternctor; return pattern_env::env_for_pattern_substituted(base_env, pattern, registry, substitution::substitution_from_generic(value_type, registry), value_type); } return base_env; }();
}

mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*value_type))) { auto _v_tnamed = std::get<registry::TNamed>((*value_type)); auto [record_name_without_parameters] = _v_tnamed; return record_name_without_parameters; } if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [generic_algebraic_outer_name, _w0] = _v_tgeneric; return generic_algebraic_outer_name; } return mlc::String(""); }();}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept{
mlc::String record_lookup_name = record_base_name_for_ordered_tuple_pattern(value_type_shaped_like_record_under_tuple_syntax);
return record_lookup_name == mlc::String("") || !registry::TypeRegistry_has_fields(registry, record_lookup_name) ? base_environment_before_ordered_fields : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<mlc::String> ordered_field_labels_from_registry = registry::TypeRegistry_record_field_names_ordered_for(registry, record_lookup_name);
  return ordered_field_labels_from_registry.size() != tuple_component_patterns.size() ? base_environment_before_ordered_fields : tuple_component_patterns.fold(let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state{base_environment_before_ordered_fields, 0}, [ordered_field_labels_from_registry, value_type_shaped_like_record_under_tuple_syntax, registry](let_pattern_infer::Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_registry_field_order) mutable { return ordered_record_tuple_environment_fold_step(state, tuple_component_pattern_in_registry_field_order, ordered_field_labels_from_registry, value_type_shaped_like_record_under_tuple_syntax, registry); }).pattern_environment;
 }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return apply_let_pattern_to_env(pattern, value_type, base_env, registry);}

} // namespace let_pattern_infer
