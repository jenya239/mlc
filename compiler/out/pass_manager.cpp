#include "pass_manager.hpp"

#include "ast.hpp"
#include "preserved_analyses.hpp"

namespace pass_manager {

using namespace ast;
using namespace preserved_analyses;
using namespace ast_tokens;

pass_manager::PassManager pass_manager_new(bool validate_keys) noexcept;

bool pass_name_character_is_safe(mlc::String character) noexcept;

bool pass_name_is_safe(mlc::String name) noexcept;

bool descriptor_name_exists(mlc::Array<pass_manager::PassDescriptor> descriptors, mlc::String name) noexcept;

ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> pass_manager_register(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor) noexcept;

bool context_has_key(mlc::Array<mlc::String> produced_keys, mlc::String key) noexcept;

mlc::Array<mlc::String> context_mark_keys(mlc::Array<mlc::String> produced_keys, mlc::Array<mlc::String> keys) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> pass_manager_validate_descriptor(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor, mlc::Array<mlc::String> produced_keys) noexcept;

preserved_analyses::PreservedAnalyses pass_manager_apply_preserved(preserved_analyses::PreservedAnalyses preserved, pass_manager::PassDescriptor descriptor) noexcept;

ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> build_compiler_pass_manager() noexcept;

mlc::Array<mlc::String> pass_manager_pass_names(pass_manager::PassManager manager) noexcept;

pass_manager::PassManager pass_manager_new(bool validate_keys) noexcept{return pass_manager::PassManager{{}, validate_keys, 16};}

bool pass_name_character_is_safe(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character >= mlc::String("0") && character <= mlc::String("9") || character == mlc::String("_");}

bool pass_name_is_safe(mlc::String name) noexcept{
return name == mlc::String("") || name.length() > 64 ? false : [&]() -> bool { 
  int index = 0;
  while (index < name.length()){
{
if (!pass_name_character_is_safe(name.char_at(index))){
{
return false;
}
}
index = index + 1;
}
}
  return true;
 }();
}

bool descriptor_name_exists(mlc::Array<pass_manager::PassDescriptor> descriptors, mlc::String name) noexcept{
int index = 0;
while (index < descriptors.size()){
{
if (descriptors[index].name == name){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> pass_manager_register(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor) noexcept{
if (!pass_name_is_safe(descriptor.name)){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pass_manager: unsafe pass name")});
}
}
if (descriptor_name_exists(manager.descriptors, descriptor.name)){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pass_manager: duplicate pass ") + descriptor.name});
}
}
if (manager.descriptors.size() >= manager.max_pass_count){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pass_manager: pass limit exceeded")});
}
}
mlc::Array<pass_manager::PassDescriptor> descriptors = manager.descriptors;
descriptors.push_back(descriptor);
return ast::Ok<pass_manager::PassManager>(pass_manager::PassManager{descriptors, manager.validate_keys, manager.max_pass_count});
}

bool context_has_key(mlc::Array<mlc::String> produced_keys, mlc::String key) noexcept{
int index = 0;
while (index < produced_keys.size()){
{
if (produced_keys[index] == key){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

mlc::Array<mlc::String> context_mark_keys(mlc::Array<mlc::String> produced_keys, mlc::Array<mlc::String> keys) noexcept{
mlc::Array<mlc::String> merged = produced_keys;
int index = 0;
while (index < keys.size()){
{
if (!context_has_key(merged, keys[index])){
{
merged.push_back(keys[index]);
}
}
index = index + 1;
}
}
return merged;
}

ast::Result<mlc::String, mlc::Array<mlc::String>> pass_manager_validate_descriptor(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor, mlc::Array<mlc::String> produced_keys) noexcept{
if (!manager.validate_keys){
{
return ast::Ok<mlc::String>(mlc::String(""));
}
}
mlc::Array<mlc::String> missing = {};
int index = 0;
while (index < descriptor.required_keys.size()){
{
mlc::String key = descriptor.required_keys[index];
if (!context_has_key(produced_keys, key)){
{
missing.push_back(key);
}
}
index = index + 1;
}
}
return missing.size() > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pass_manager: pass ") + descriptor.name + mlc::String(" missing keys: ") + missing.join(mlc::String(", "))})) : ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(mlc::String("")));
}

preserved_analyses::PreservedAnalyses pass_manager_apply_preserved(preserved_analyses::PreservedAnalyses preserved, pass_manager::PassDescriptor descriptor) noexcept{
return descriptor.invalidates_analyses ? preserved_analyses::preserved_analyses_invalidate_all(preserved) : [&]() -> preserved_analyses::PreservedAnalyses { 
  preserved_analyses::PreservedAnalyses next = std::move(preserved);
  int index = 0;
  while (index < descriptor.preserves_analyses.size()){
{
next = preserved_analyses::preserved_analyses_mark(next, descriptor.preserves_analyses[index]);
index = index + 1;
}
}
  return next;
 }();
}

ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>> build_compiler_pass_manager() noexcept{
auto __try_manager_after_verify = pass_manager_register(pass_manager_new(true), pass_manager::PassDescriptor{mlc::String("verify_ast"), mlc::Array<mlc::String>{mlc::String("modular_input")}, mlc::Array<mlc::String>{mlc::String("verify_ast")}, {}, false});
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_manager_after_verify)) return ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_manager_after_verify));
pass_manager::PassManager manager_after_verify = std::get<ast::Ok<pass_manager::PassManager>>(__try_manager_after_verify).field0;

auto __try_manager_after_checker = pass_manager_register(manager_after_verify, pass_manager::PassDescriptor{mlc::String("checker"), mlc::Array<mlc::String>{mlc::String("modular_input")}, mlc::Array<mlc::String>{mlc::String("checked")}, mlc::Array<mlc::String>{mlc::String("type_registry")}, false});
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_manager_after_checker)) return ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_manager_after_checker));
pass_manager::PassManager manager_after_checker = std::get<ast::Ok<pass_manager::PassManager>>(__try_manager_after_checker).field0;

auto __try_manager_after_transform = pass_manager_register(manager_after_checker, pass_manager::PassDescriptor{mlc::String("transform"), mlc::Array<mlc::String>{mlc::String("checked")}, mlc::Array<mlc::String>{mlc::String("transformed")}, {}, true});
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_manager_after_transform)) return ast::Result<pass_manager::PassManager, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_manager_after_transform));
pass_manager::PassManager manager_after_transform = std::get<ast::Ok<pass_manager::PassManager>>(__try_manager_after_transform).field0;

return pass_manager_register(manager_after_transform, pass_manager::PassDescriptor{mlc::String("codegen"), mlc::Array<mlc::String>{mlc::String("transformed")}, mlc::Array<mlc::String>{mlc::String("codegen")}, {}, true});
}

mlc::Array<mlc::String> pass_manager_pass_names(pass_manager::PassManager manager) noexcept{
mlc::Array<mlc::String> names = {};
int index = 0;
while (index < manager.descriptors.size()){
{
names.push_back(manager.descriptors[index].name);
index = index + 1;
}
}
return names;
}

} // namespace pass_manager
