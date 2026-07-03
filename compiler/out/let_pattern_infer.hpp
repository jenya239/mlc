#ifndef LET_PATTERN_INFER_HPP
#define LET_PATTERN_INFER_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "pattern_env.hpp"
#include "substitution.hpp"
namespace let_pattern_infer {

struct Tuple_or_slot_pattern_environment_fold_state {
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> pattern_environment;
  int next_component_index;
};
mlc::String pattern_ident_binding_name(std::shared_ptr<ast::Pattern> pattern) noexcept;
bool value_type_is_tuple_type(std::shared_ptr<registry::Type> value_type) noexcept;
bool value_type_is_pair_type(std::shared_ptr<registry::Type> value_type) noexcept;
bool value_type_is_array_type(std::shared_ptr<registry::Type> value_type) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;
std::shared_ptr<registry::Type> pair_first_member_type(std::shared_ptr<registry::Type> value_type) noexcept;
std::shared_ptr<registry::Type> pair_second_member_type(std::shared_ptr<registry::Type> value_type) noexcept;
std::shared_ptr<registry::Type> array_element_type_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;
Tuple_or_slot_pattern_environment_fold_state let_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, registry::TypeRegistry registry) noexcept;
Tuple_or_slot_pattern_environment_fold_state let_array_element_patterns_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> array_cell_pattern, std::shared_ptr<registry::Type> array_element_type, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> let_record_literal_field_pattern_fold_step(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_pattern_environment, std::shared_ptr<ast::Pattern> field_subpattern, std::shared_ptr<registry::Type> let_rhs_object_type, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_member_types(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, mlc::Array<std::shared_ptr<registry::Type>> tuple_member_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_patterns_with_pair(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> first_member_type, std::shared_ptr<registry::Type> second_member_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_tuple_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_to_env(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_record_pattern_to_env(mlc::String record_heading_name_or_empty, mlc::Array<std::shared_ptr<ast::Pattern>> literal_field_patterns, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ctor_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
Tuple_or_slot_pattern_environment_fold_state ordered_record_tuple_environment_fold_step(Tuple_or_slot_pattern_environment_fold_state state, std::shared_ptr<ast::Pattern> tuple_component_pattern_in_record_order, mlc::Array<mlc::String> ordered_field_labels, std::shared_ptr<registry::Type> let_rhs_record_object_type, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_ident_pattern_to_env(mlc::String let_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_array_pattern_dispatch(mlc::Array<std::shared_ptr<ast::Pattern>> cell_patterns, mlc::String remainder_binding_name, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;
mlc::String record_base_name_for_ordered_tuple_pattern(std::shared_ptr<registry::Type> value_type) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> tuple_pattern_environment_using_ordered_record_fields(mlc::Array<std::shared_ptr<ast::Pattern>> tuple_component_patterns, std::shared_ptr<registry::Type> value_type_shaped_like_record_under_tuple_syntax, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_environment_before_ordered_fields, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

} // namespace let_pattern_infer

#endif // LET_PATTERN_INFER_HPP
