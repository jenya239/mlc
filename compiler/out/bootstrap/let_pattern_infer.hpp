#ifndef LET_PATTERN_INFER_HPP
#define LET_PATTERN_INFER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "pattern_env.hpp"

namespace let_pattern_infer {

struct Let_generic_substitution_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution_map;int next_algebraic_type_parameter_index;};
struct Tuple_or_slot_pattern_environment_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> pattern_environment;int next_component_index;};
Let_generic_substitution_fold_state let_generic_substitution_fold_step(Let_generic_substitution_fold_state state, mlc::String algebraic_type_parameter_name, mlc::Array<std::shared_ptr<registry::Type>> algebraic_type_arguments) noexcept;
Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept;
Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pat> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept;
Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pat> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> register_algebraic_type_arguments_into_substitution_map(registry::TypeRegistry registry, mlc::String algebraic_outer_type_name, mlc::Array<std::shared_ptr<registry::Type>> algebraic_type_arguments, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution_destinations) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_let_substitution(std::shared_ptr<registry::Type> value_type, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pat>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

} // namespace let_pattern_infer

#endif // LET_PATTERN_INFER_HPP
