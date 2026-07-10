#define main mlc_user_main
#include "mir_dump.hpp"

#include "mir_types.hpp"

namespace mir_dump {

using namespace mir_types;

mlc::String print_mir_stmt(mir_types::MirStmt statement) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirAssign& mirAssign) { auto [local_id, __1] = mirAssign; return ((mlc::String("assign l", 8) + mlc::to_string(local_id.index)) + mlc::String("", 0)); },
[&](const mir_types::MirCallAssign& mirCallAssign) { auto [local_id, callee_name, arguments] = mirCallAssign; return ((((((mlc::String("call l", 6) + mlc::to_string(local_id.index)) + mlc::String(" = ", 3)) + mlc::to_string(callee_name)) + mlc::String("(", 1)) + mlc::to_string(arguments.length())) + mlc::String(")", 1)); }
}, statement);
}
mlc::String print_mir_block(mir_types::MirBlock block) noexcept{
  auto result = ((((((mlc::String("block ", 6) + mlc::to_string(block.id.index)) + mlc::String(" (", 2)) + mlc::to_string(block.label)) + mlc::String(") stmts=", 8)) + mlc::to_string(block.stmts.length())) + mlc::String("", 0));
  auto index = 0;
  while ((index < block.stmts.length()))   {
    (result = (result + ((mlc::String("\n    ", 5) + mlc::to_string(print_mir_stmt(block.stmts[index]))) + mlc::String("", 0))));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::String print_mir_function(mir_types::MirFunction function) noexcept{
  auto result = ((((((mlc::String("mir fn ", 7) + mlc::to_string(function.name)) + mlc::String(" blocks=", 8)) + mlc::to_string(function.blocks.length())) + mlc::String(" locals=", 8)) + mlc::to_string(function.locals.length())) + mlc::String("", 0));
  auto index = 0;
  while ((index < function.blocks.length()))   {
    (result = (result + ((mlc::String("\n  ", 3) + mlc::to_string(print_mir_block(function.blocks[index]))) + mlc::String("", 0))));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::String print_mir_module(mir_types::MirModule mir_module) noexcept{
  auto result = ((mlc::String("mir module functions=", 21) + mlc::to_string(mir_module.functions.length())) + mlc::String("", 0));
  auto index = 0;
  while ((index < mir_module.functions.length()))   {
    (result = (result + ((mlc::String("\n", 1) + mlc::to_string(print_mir_function(mir_module.functions[index]))) + mlc::String("", 0))));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::String print_mir_program(mir_types::MirProgram program) noexcept{
  auto result = ((mlc::String("mir program modules=", 20) + mlc::to_string(program.modules.length())) + mlc::String("", 0));
  auto index = 0;
  while ((index < program.modules.length()))   {
    (result = (result + ((mlc::String("\n", 1) + mlc::to_string(print_mir_module(program.modules[index]))) + mlc::String("", 0))));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}

} // namespace mir_dump
