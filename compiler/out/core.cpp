#include "core.hpp"

#include "registry.hpp"

namespace core {

using namespace registry;

core::CoreNodeId core_node_id_new(int value) noexcept;

bool core_node_id_is_valid(core::CoreNodeId identifier) noexcept;

core::CoreModule core_module_empty() noexcept;

bool core_name_character_is_safe(mlc::String character) noexcept;

bool core_function_name_is_safe(mlc::String name) noexcept;

bool core_block_label_is_safe(mlc::String label) noexcept;

core::CoreNodeId core_node_id_new(int value) noexcept{return core::CoreNodeId{value};}

bool core_node_id_is_valid(core::CoreNodeId identifier) noexcept{return identifier.value >= 0 && identifier.value < 1048576;}

core::CoreModule core_module_empty() noexcept{return core::CoreModule{{}};}

bool core_name_character_is_safe(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character >= mlc::String("0") && character <= mlc::String("9") || character == mlc::String("_");}

bool core_function_name_is_safe(mlc::String name) noexcept{
return name == mlc::String("") || name.length() > 128 ? false : [&]() -> bool { 
  int index = 0;
  while (index < name.length()){
{
if (!core_name_character_is_safe(name.char_at(index))){
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

bool core_block_label_is_safe(mlc::String label) noexcept{
return label == mlc::String("") || label.length() > 64 ? false : [&]() -> bool { 
  int index = 0;
  while (index < label.length()){
{
if (!core_name_character_is_safe(label.char_at(index))){
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

} // namespace core
