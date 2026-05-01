#ifndef PATTERN_ENV_HPP
#define PATTERN_ENV_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace pattern_env {

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

} // namespace pattern_env

#endif // PATTERN_ENV_HPP
