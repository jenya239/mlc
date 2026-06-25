#include "core_dump.hpp"

#include "core.hpp"

namespace core_dump {

using namespace core;

mlc::String print_core_basic_block(core::CoreBasicBlock block) noexcept;

mlc::String print_core_function(core::CoreFunction function) noexcept;

mlc::String print_core_module(core::CoreModule core_module) noexcept;

mlc::String print_core_basic_block(core::CoreBasicBlock block) noexcept{return mlc::String("block ") + mlc::to_string(block.identifier.value) + mlc::String(" (") + block.label + mlc::String(") stmts=") + mlc::to_string(block.statements.size());}

mlc::String print_core_function(core::CoreFunction function) noexcept{
mlc::String result = mlc::String("core fn ") + function.name + mlc::String(" blocks=") + mlc::to_string(function.blocks.size()) + mlc::String(" entry=") + mlc::to_string(function.entry_block.value);
int index = 0;
while (index < function.blocks.size()){
{
result = result + mlc::String("\n  ") + print_core_basic_block(function.blocks[index]);
index = index + 1;
}
}
return result;
}

mlc::String print_core_module(core::CoreModule core_module) noexcept{
mlc::String result = mlc::String("core module functions=") + mlc::to_string(core_module.functions.size());
int index = 0;
while (index < core_module.functions.size()){
{
result = result + mlc::String("\n") + print_core_function(core_module.functions[index]);
index = index + 1;
}
}
return result;
}

} // namespace core_dump
