#define main mlc_user_main
#include "pass_manager.hpp"

#include "ast.hpp"
#include "preserved_analyses.hpp"

namespace pass_manager {

using namespace ast;
using namespace preserved_analyses;
using namespace ast_tokens;

PassManager pass_manager_new(bool validate_keys) noexcept{
  return PassManager{{}, validate_keys, 16};
}
bool pass_name_character_is_safe(mlc::String character) noexcept{
  return (((((character >= mlc::String("a", 1)) && (character <= mlc::String("z", 1))) || ((character >= mlc::String("A", 1)) && (character <= mlc::String("Z", 1)))) || ((character >= mlc::String("0", 1)) && (character <= mlc::String("9", 1)))) || (character == mlc::String("_", 1)));
}
bool pass_name_is_safe(mlc::String name) noexcept{
  if (((name == mlc::String("", 0)) || (name.length() > 64)))   {
    return false;
  } else   {
    auto index = 0;
    while ((index < name.length()))     {
      if ((!pass_name_character_is_safe(name.char_at(index))))       {
        return false;
      }
      (index = mlc::arith::checked_add(index, 1));
    }
    return true;
  }
}
bool descriptor_name_exists(mlc::Array<PassDescriptor> descriptors, mlc::String name) noexcept{
  auto index = 0;
  while ((index < descriptors.length()))   {
    if ((descriptors[index].name == name))     {
      return true;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return false;
}
ast::Result<PassManager, mlc::Array<mlc::String>> pass_manager_register(PassManager manager, PassDescriptor descriptor) noexcept{
  if ((!pass_name_is_safe(descriptor.name)))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("pass_manager: unsafe pass name", 30)}};
  }
  if (descriptor_name_exists(manager.descriptors, descriptor.name))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("pass_manager: duplicate pass ", 29) + mlc::to_string(descriptor.name)) + mlc::String("", 0))}};
  }
  if ((manager.descriptors.length() >= manager.max_pass_count))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("pass_manager: pass limit exceeded", 33)}};
  }
  auto descriptors = manager.descriptors;
  descriptors.push_back(descriptor);
  return ast::Ok<PassManager>{PassManager{descriptors, manager.validate_keys, manager.max_pass_count}};
}
bool context_has_key(mlc::Array<mlc::String> produced_keys, mlc::String key) noexcept{
  auto index = 0;
  while ((index < produced_keys.length()))   {
    if ((produced_keys[index] == key))     {
      return true;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return false;
}
mlc::Array<mlc::String> context_mark_keys(mlc::Array<mlc::String> produced_keys, mlc::Array<mlc::String> keys) noexcept{
  auto merged = produced_keys;
  auto index = 0;
  while ((index < keys.length()))   {
    if ((!context_has_key(merged, keys[index])))     {
      merged.push_back(keys[index]);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return merged;
}
ast::Result<mlc::String, mlc::Array<mlc::String>> pass_manager_validate_descriptor(PassManager manager, PassDescriptor descriptor, mlc::Array<mlc::String> produced_keys) noexcept{
  if ((!manager.validate_keys))   {
    return ast::Ok<mlc::String>{mlc::String("", 0)};
  }
  auto missing = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < descriptor.required_keys.length()))   {
    auto key = descriptor.required_keys[index];
    if ((!context_has_key(produced_keys, key)))     {
      missing.push_back(key);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  if ((missing.length() > 0))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((((mlc::String("pass_manager: pass ", 19) + mlc::to_string(descriptor.name)) + mlc::String(" missing keys: ", 15)) + mlc::to_string(missing.join(mlc::String(", ", 2)))) + mlc::String("", 0))}};
  } else   {
    return ast::Ok<mlc::String>{mlc::String("", 0)};
  }
}
preserved_analyses::PreservedAnalyses pass_manager_apply_preserved(preserved_analyses::PreservedAnalyses preserved, PassDescriptor descriptor) noexcept{
  if (descriptor.invalidates_analyses)   {
    return preserved_analyses::preserved_analyses_invalidate_all(preserved);
  } else   {
    auto next = preserved;
    auto index = 0;
    while ((index < descriptor.preserves_analyses.length()))     {
      (next = preserved_analyses::preserved_analyses_mark(next, descriptor.preserves_analyses[index]));
      (index = mlc::arith::checked_add(index, 1));
    }
    return next;
  }
}
ast::Result<PassManager, mlc::Array<mlc::String>> build_compiler_pass_manager() noexcept{
  auto manager_after_verify = ({ auto __q = pass_manager_register(pass_manager_new(true), PassDescriptor{mlc::String("verify_ast", 10), mlc::Array<mlc::String>{mlc::String("modular_input", 13)}, mlc::Array<mlc::String>{mlc::String("verify_ast", 10)}, {}, false}); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
  auto manager_after_checker = ({ auto __q = pass_manager_register(manager_after_verify, PassDescriptor{mlc::String("checker", 7), mlc::Array<mlc::String>{mlc::String("modular_input", 13)}, mlc::Array<mlc::String>{mlc::String("checked", 7)}, mlc::Array<mlc::String>{mlc::String("type_registry", 13)}, false}); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
  auto manager_after_transform = ({ auto __q = pass_manager_register(manager_after_checker, PassDescriptor{mlc::String("transform", 9), mlc::Array<mlc::String>{mlc::String("checked", 7)}, mlc::Array<mlc::String>{mlc::String("transformed", 11)}, {}, true}); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
  return pass_manager_register(manager_after_transform, PassDescriptor{mlc::String("codegen", 7), mlc::Array<mlc::String>{mlc::String("transformed", 11)}, mlc::Array<mlc::String>{mlc::String("codegen", 7)}, {}, true});
}
mlc::Array<mlc::String> pass_manager_pass_names(PassManager manager) noexcept{
  auto names = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < manager.descriptors.length()))   {
    names.push_back(manager.descriptors[index].name);
    (index = mlc::arith::checked_add(index, 1));
  }
  return names;
}

} // namespace pass_manager
