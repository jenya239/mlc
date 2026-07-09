#include "mir_dump.hpp"

#include "mir_types.hpp"

namespace mir_dump {

using namespace mir_types;

mlc::String print_mir_block(mir_types::MirBlock block) noexcept;

mlc::String print_mir_function(mir_types::MirFunction function) noexcept;

mlc::String print_mir_module(mir_types::MirModule mir_module) noexcept;

mlc::String print_mir_program(mir_types::MirProgram program) noexcept;

mlc::String print_mir_block(mir_types::MirBlock block) noexcept{return mlc::String("block ") + mlc::to_string(block.id.index) + mlc::String(" (") + block.label + mlc::String(") stmts=") + mlc::to_string(block.stmts.size());}

mlc::String print_mir_function(mir_types::MirFunction function) noexcept{
mlc::String result = mlc::String("mir fn ") + function.name + mlc::String(" blocks=") + mlc::to_string(function.blocks.size()) + mlc::String(" locals=") + mlc::to_string(function.locals.size());
int index = 0;
while (index < function.blocks.size()){
{
result = result + mlc::String("\n  ") + print_mir_block(function.blocks[index]);
index = index + 1;
}
}
return result;
}

mlc::String print_mir_module(mir_types::MirModule mir_module) noexcept{
mlc::String result = mlc::String("mir module functions=") + mlc::to_string(mir_module.functions.size());
int index = 0;
while (index < mir_module.functions.size()){
{
result = result + mlc::String("\n") + print_mir_function(mir_module.functions[index]);
index = index + 1;
}
}
return result;
}

mlc::String print_mir_program(mir_types::MirProgram program) noexcept{
mlc::String result = mlc::String("mir program modules=") + mlc::to_string(program.modules.size());
int index = 0;
while (index < program.modules.size()){
{
result = result + mlc::String("\n") + print_mir_module(program.modules[index]);
index = index + 1;
}
}
return result;
}

} // namespace mir_dump
