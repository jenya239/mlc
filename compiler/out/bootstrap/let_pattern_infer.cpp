#define main mlc_user_main
#include "let_pattern_infer.hpp"

namespace let_pattern_infer {

Let_generic_substitution_fold_state let_generic_substitution_fold_step(Let_generic_substitution_fold_state state, mlc::String algebraic_type_parameter_name, mlc::Array<std::shared_ptr<registry::Type>> algebraic_type_arguments) noexcept{
if ((state.next_algebraic_type_parameter_index < algebraic_type_arguments.length())) {
state.substitution_map.set(algebraic_type_parameter_name, algebraic_type_arguments[state.next_algebraic_type_parameter_index]);
}
return Let_generic_substitution_fold_state{state.substitution_map, (state.next_algebraic_type_parameter_index + 1)};
}
Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept{
auto tuple_member_type = tuple_member_types[state.next_component_index];
return Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern, tuple_member_type, state.pattern_environment, registry), (state.next_component_index + 1)};
}
Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept{
return Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(array_cell_pattern, array_element_type, state.pattern_environment, registry), (state.next_component_index + 1)};
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pat> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept{
auto record_field_binding_name = std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [binding_name_for_field_literal, __1] = patIdent; return binding_name_for_field_literal; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*field_subpattern));
if ((record_field_binding_name != mlc::String("", 0))) {
auto field_object_type_from_registry = registry::field_type_from_object(let_rhs_object_type, record_field_binding_name, registry);
return apply_let_pattern_to_env(field_subpattern, field_object_type_from_registry, field_pattern_environment, registry);
} else {
return field_pattern_environment;
}
}
Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept{
auto label_for_ordered_slot = ordered_field_labels[state.next_component_index];
auto inferred_field_member_type = registry::field_type_from_object(let_rhs_record_object_type, label_for_ordered_slot, registry);
return Tuple_or_slot_pattern_environment_fold_state{apply_let_pattern_to_env(tuple_component_pattern_in_record_order, inferred_field_member_type, state.pattern_environment, registry), (state.next_component_index + 1)};
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> register_algebraic_type_arguments_into_substitution_map(registry::TypeRegistry registry, mlc::String algebraic_outer_type_name, mlc::Array<std::shared_ptr<registry::Type>> algebraic_type_arguments, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution_destinations) noexcept{
auto algebraic_constructor_type_parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, algebraic_outer_type_name);
algebraic_constructor_type_parameter_names.fold(Let_generic_substitution_fold_state{substitution_destinations, 0}, [=](Let_generic_substitution_fold_state accumulated_substitution_fold_state, mlc::String type_parameter_binding_name_from_decl) mutable { return let_generic_substitution_fold_step(accumulated_substitution_fold_state, type_parameter_binding_name_from_decl, algebraic_type_arguments); });
return substitution_destinations;
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_let_substitution(std::shared_ptr<registry::Type> value_type, registry::TypeRegistry registry) noexcept{
auto substitution_map = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [algebraic_outer_type_name, algebraic_type_arguments] = tGeneric; return register_algebraic_type_arguments_into_substitution_map(registry, algebraic_outer_type_name, algebraic_type_arguments, substitution_map); },
[&](const auto& __v) { return substitution_map; }
}, (*value_type));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatWild& patWild) { auto [__0] = patWild; return base_env; },
[&](const ast::PatIdent& patIdent) { auto [let_binding_name, __1] = patIdent; return [&]() {
auto environment_where_ident_binding_is_added = base_env;
environment_where_ident_binding_is_added.set(let_binding_name, value_type);
return environment_where_ident_binding_is_added;
}(); },
[&](const ast::PatTuple& patTuple) { auto [tuple_component_patterns, __1] = patTuple; return std::visit(overloaded{[&](const registry::TTuple& tTuple) { auto [tuple_member_types] = tTuple; return ((tuple_component_patterns.length() != tuple_member_types.length()) ? (base_env) : (tuple_component_patterns.fold(Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [=](Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> tuple_component_pattern) mutable { return let_tuple_environment_fold_step(state, tuple_component_pattern, tuple_member_types, registry); }).pattern_environment)); },
[&](const registry::TPair& tPair) { auto [first_member_type, second_member_type] = tPair; return ((tuple_component_patterns.length() != 2) ? (base_env) : ([&]() {
auto environment_after_first_tuple_component = apply_let_pattern_to_env(tuple_component_patterns[0], first_member_type, base_env, registry);
return apply_let_pattern_to_env(tuple_component_patterns[1], second_member_type, environment_after_first_tuple_component, registry);
}())); },
[&](const auto& __v) { return tuple_pattern_environment_using_ordered_record_fields(tuple_component_patterns, value_type, base_env, registry); }
}, (*value_type)); },
[&](const ast::PatArray& patArray) { auto [cell_patterns, remainder_binding_name, __2] = patArray; return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [element_type_inside_array] = tArray; return [&]() {
auto environment_after_fixed_array_cells = cell_patterns.fold(Tuple_or_slot_pattern_environment_fold_state{base_env, 0}, [=](Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> cell_pattern) mutable { return let_array_element_patterns_fold_step(state, cell_pattern, element_type_inside_array, registry); }).pattern_environment;
return ((remainder_binding_name != mlc::String("", 0)) ? ([&]() {
auto remainder_tail_environment = environment_after_fixed_array_cells;
remainder_tail_environment.set(remainder_binding_name, value_type);
return remainder_tail_environment;
}()) : (environment_after_fixed_array_cells));
}(); },
[&](const auto& __v) { return base_env; }
}, (*value_type)); },
[&](const ast::PatRecord& patRecord) { auto [record_heading_name_or_empty, literal_field_patterns, __2] = patRecord; return ((record_heading_name_or_empty != mlc::String("", 0)) ? (base_env) : (literal_field_patterns.fold(base_env, [=](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment_so_far, std::shared_ptr<ast::Pat> field_subpattern_under_pattern_node) mutable { return let_record_literal_field_pattern_fold_step(field_pattern_environment_so_far, field_subpattern_under_pattern_node, value_type, registry); }))); },
[&](const ast::PatCtor& patCtor) { auto [__0, __1, __2] = patCtor; return pattern_env::env_for_pattern_substituted(base_env, pat, registry, build_let_substitution(value_type, registry), value_type); },
[&](const auto& __v) { return base_env; }
}, (*pat));
}
mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept{
return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [record_name_without_parameters] = tNamed; return record_name_without_parameters; },
[&](const registry::TGeneric& tGeneric) { auto [generic_algebraic_outer_name, __1] = tGeneric; return generic_algebraic_outer_name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*value_type));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pat>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept{
auto record_lookup_name = record_base_name_for_ordered_tuple_pattern(value_type_shaped_like_record_under_tuple_syntax);
if (((record_lookup_name == mlc::String("", 0)) || (!registry::TypeRegistry_has_fields(registry, record_lookup_name)))) {
return base_environment_before_ordered_fields;
} else {
auto ordered_field_labels_from_registry = registry::TypeRegistry_record_field_names_ordered_for(registry, record_lookup_name);
if ((ordered_field_labels_from_registry.length() != tuple_component_patterns.length())) {
return base_environment_before_ordered_fields;
} else {
return tuple_component_patterns.fold(Tuple_or_slot_pattern_environment_fold_state{base_environment_before_ordered_fields, 0}, [=](Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> tuple_component_pattern_in_registry_field_order) mutable { return ordered_record_tuple_environment_fold_step(state, tuple_component_pattern_in_registry_field_order, ordered_field_labels_from_registry, value_type_shaped_like_record_under_tuple_syntax, registry); }).pattern_environment;
}
}
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
return apply_let_pattern_to_env(pat, value_type, base_env, registry);
}

} // namespace let_pattern_infer
