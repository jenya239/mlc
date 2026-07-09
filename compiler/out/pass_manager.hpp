#ifndef PASS_MANAGER_HPP
#define PASS_MANAGER_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "preserved_analyses.hpp"
namespace pass_manager {

struct PassDescriptor {
  mlc::String name;
  mlc::Array<mlc::String> required_keys;
  mlc::Array<mlc::String> produced_keys;
  mlc::Array<mlc::String> preserves_analyses;
  bool invalidates_analyses;
};
struct PassManager {
  mlc::Array<PassDescriptor> descriptors;
  bool validate_keys;
  int max_pass_count;
};
PassManager pass_manager_new(bool validate_keys) noexcept;
bool pass_name_character_is_safe(mlc::String character) noexcept;
bool pass_name_is_safe(mlc::String name) noexcept;
bool descriptor_name_exists(mlc::Array<PassDescriptor> descriptors, mlc::String name) noexcept;
ast::Result<PassManager, mlc::Array<mlc::String>> pass_manager_register(PassManager manager, PassDescriptor descriptor) noexcept;
bool context_has_key(mlc::Array<mlc::String> produced_keys, mlc::String key) noexcept;
mlc::Array<mlc::String> context_mark_keys(mlc::Array<mlc::String> produced_keys, mlc::Array<mlc::String> keys) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> pass_manager_validate_descriptor(PassManager manager, PassDescriptor descriptor, mlc::Array<mlc::String> produced_keys) noexcept;
preserved_analyses::PreservedAnalyses pass_manager_apply_preserved(preserved_analyses::PreservedAnalyses preserved, PassDescriptor descriptor) noexcept;
ast::Result<PassManager, mlc::Array<mlc::String>> build_compiler_pass_manager() noexcept;
mlc::Array<mlc::String> pass_manager_pass_names(PassManager manager) noexcept;

} // namespace pass_manager

#endif // PASS_MANAGER_HPP
