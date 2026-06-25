#include "verify_core.hpp"

#include "core.hpp"

namespace verify_core {

using namespace core;

mlc::Array<mlc::String> verify_core_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept;

bool core_block_id_exists(mlc::Array<core::CoreBasicBlock> blocks, core::CoreNodeId identifier) noexcept;

bool core_block_ids_are_unique(mlc::Array<core::CoreBasicBlock> blocks) noexcept;

mlc::Array<mlc::String> verify_core_node_reference(mlc::String label, core::CoreNodeId identifier) noexcept;

mlc::Array<mlc::String> verify_core_basic_block(core::CoreBasicBlock block) noexcept;

mlc::Array<mlc::String> verify_core_function(core::CoreFunction function) noexcept;

mlc::Array<mlc::String> verify_core_module(core::CoreModule core_module) noexcept;

mlc::Array<mlc::String> verify_core_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept{
mlc::Array<mlc::String> combined = accumulator;
int index = 0;
while (index < more.size()){
{
combined.push_back(more[index]);
index = index + 1;
}
}
return combined;
}

bool core_block_id_exists(mlc::Array<core::CoreBasicBlock> blocks, core::CoreNodeId identifier) noexcept{
int index = 0;
while (index < blocks.size()){
{
if (blocks[index].identifier.value == identifier.value){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

bool core_block_ids_are_unique(mlc::Array<core::CoreBasicBlock> blocks) noexcept{
int index = 0;
while (index < blocks.size()){
{
int inner = index + 1;
while (inner < blocks.size()){
{
if (blocks[index].identifier.value == blocks[inner].identifier.value){
{
return false;
}
}
inner = inner + 1;
}
}
index = index + 1;
}
}
return true;
}

mlc::Array<mlc::String> verify_core_node_reference(mlc::String label, core::CoreNodeId identifier) noexcept{return core::core_node_id_is_valid(identifier) ? mlc::Array<mlc::String>{} : mlc::Array<mlc::String>{label + mlc::String(": invalid node id ") + mlc::to_string(identifier.value)};}

mlc::Array<mlc::String> verify_core_basic_block(core::CoreBasicBlock block) noexcept{
mlc::Array<mlc::String> errors = {};
if (!core::core_block_label_is_safe(block.label)){
{
errors.push_back(mlc::String("core block: unsafe label"));
}
}
errors = verify_core_append_errors(errors, verify_core_node_reference(mlc::String("core block"), block.identifier));
return std::visit(overloaded{
  [&](const CoreTerminatorReturn& coreterminatorreturn) -> mlc::Array<mlc::String> { auto [value] = coreterminatorreturn; return verify_core_append_errors(errors, verify_core_node_reference(mlc::String("core return"), value)); },
  [&](const CoreTerminatorUnreachable& coreterminatorunreachable) -> mlc::Array<mlc::String> { return errors; }
}, block.terminator);
}

mlc::Array<mlc::String> verify_core_function(core::CoreFunction function) noexcept{
mlc::Array<mlc::String> errors = {};
if (!core::core_function_name_is_safe(function.name)){
{
errors.push_back(mlc::String("core function: unsafe name ") + function.name);
}
}
if (function.blocks.size() == 0){
{
errors.push_back(mlc::String("core function: missing basic blocks"));
}
}
if (!core_block_ids_are_unique(function.blocks)){
{
errors.push_back(mlc::String("core function: duplicate block id"));
}
}
if (!core_block_id_exists(function.blocks, function.entry_block)){
{
errors.push_back(mlc::String("core function: entry block not found"));
}
}
int index = 0;
while (index < function.blocks.size()){
{
errors = verify_core_append_errors(errors, verify_core_basic_block(function.blocks[index]));
index = index + 1;
}
}
return errors;
}

mlc::Array<mlc::String> verify_core_module(core::CoreModule core_module) noexcept{
mlc::Array<mlc::String> errors = {};
int index = 0;
while (index < core_module.functions.size()){
{
errors = verify_core_append_errors(errors, verify_core_function(core_module.functions[index]));
index = index + 1;
}
}
return errors;
}

} // namespace verify_core
