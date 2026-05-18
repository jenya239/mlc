#ifndef PATTERN_ENV_HPP
#define PATTERN_ENV_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace pattern_env {

struct Constructor_pattern_environment_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment;int next_constructor_subpattern_index;};
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;
Constructor_pattern_environment_fold_state constructor_subpattern_environment_fold_step(Constructor_pattern_environment_fold_state accumulator, std::shared_ptr<ast::Pat> sub_pattern_value, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_all_constructor_subpatterns(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> initial_environment, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type) noexcept;

} // namespace pattern_env

#endif // PATTERN_ENV_HPP
