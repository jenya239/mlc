#define main mlc_user_main
#include "mir_types.hpp"

#include "registry.hpp"

namespace mir_types {

using namespace registry;

LocalId mir_local_id(int index) noexcept{
  return LocalId{index};
}
BlockId mir_block_id(int index) noexcept{
  return BlockId{index};
}
bool mir_id_is_valid(int index) noexcept{
  return ((index >= 0) && (index < 1048576));
}
MirProgram mir_program_empty() noexcept{
  return MirProgram{{}};
}
bool mir_name_character_is_safe(mlc::String character) noexcept{
  return (((((character >= mlc::String("a", 1)) && (character <= mlc::String("z", 1))) || ((character >= mlc::String("A", 1)) && (character <= mlc::String("Z", 1)))) || ((character >= mlc::String("0", 1)) && (character <= mlc::String("9", 1)))) || (character == mlc::String("_", 1)));
}
bool mir_function_name_is_safe(mlc::String name) noexcept{
  if (((name == mlc::String("", 0)) || (name.length() > 128)))   {
    return false;
  } else   {
    auto index = 0;
    while ((index < name.length()))     {
      if ((!mir_name_character_is_safe(name.char_at(index))))       {
        return false;
      }
      (index = mlc::arith::checked_add(index, 1));
    }
    return true;
  }
}
bool mir_block_label_is_safe(mlc::String label) noexcept{
  if (((label == mlc::String("", 0)) || (label.length() > 64)))   {
    return false;
  } else   {
    auto index = 0;
    while ((index < label.length()))     {
      if ((!mir_name_character_is_safe(label.char_at(index))))       {
        return false;
      }
      (index = mlc::arith::checked_add(index, 1));
    }
    return true;
  }
}

} // namespace mir_types
