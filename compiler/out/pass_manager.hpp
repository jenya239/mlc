#ifndef PASS_MANAGER_HPP
#define PASS_MANAGER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "preserved_analyses.hpp"

namespace pass_manager {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct PassDescriptor {mlc::String name;mlc::Array<mlc::String> required_keys;mlc::Array<mlc::String> produced_keys;mlc::Array<mlc::String> preserves_analyses;bool invalidates_analyses;};

struct PassManager {mlc::Array<pass_manager::PassDescriptor> descriptors;bool validate_keys;int max_pass_count;};

pass_manager::PassManager pass_manager_new(bool validate_keys) noexcept;

bool pass_name_is_safe(mlc::String name) noexcept;

ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> pass_manager_register(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor) noexcept;

bool context_has_key(mlc::Array<mlc::String> produced_keys, mlc::String key) noexcept;

mlc::Array<mlc::String> context_mark_keys(mlc::Array<mlc::String> produced_keys, mlc::Array<mlc::String> keys) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> pass_manager_validate_descriptor(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor, mlc::Array<mlc::String> produced_keys) noexcept;

preserved_analyses::PreservedAnalyses pass_manager_apply_preserved(preserved_analyses::PreservedAnalyses preserved, pass_manager::PassDescriptor descriptor) noexcept;

ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> build_compiler_pass_manager() noexcept;

mlc::Array<mlc::String> pass_manager_pass_names(pass_manager::PassManager manager) noexcept;

} // namespace pass_manager

#endif // PASS_MANAGER_HPP
