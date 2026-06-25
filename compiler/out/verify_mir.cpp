#include "verify_mir.hpp"

#include "mir_types.hpp"

namespace verify_mir {

using namespace mir_types;

mlc::Array<mlc::String> verify_mir_append(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept;

mlc::Array<mlc::String> verify_mir_local_id(mlc::String label, mir_types::LocalId local_id) noexcept;

mlc::Array<mlc::String> verify_mir_block_id(mlc::String label, mir_types::BlockId block_id) noexcept;

bool verify_mir_block_exists(mlc::Array<mir_types::MirBlock> blocks, mir_types::BlockId block_id) noexcept;

mlc::Array<mlc::String> verify_mir_terminator(mir_types::MirFunction function, mir_types::MirBlock block) noexcept;

mlc::Array<mlc::String> verify_mir_block(mir_types::MirFunction function, mir_types::MirBlock block) noexcept;

mlc::Array<mlc::String> verify_mir_function(mir_types::MirFunction function) noexcept;

mlc::Array<mlc::String> verify_mir_module(mir_types::MirModule mir_module) noexcept;

mlc::Array<mlc::String> verify_mir_program(mir_types::MirProgram program) noexcept;

mlc::Array<mlc::String> verify_mir_append(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept{
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

mlc::Array<mlc::String> verify_mir_local_id(mlc::String label, mir_types::LocalId local_id) noexcept{return mir_types::mir_id_is_valid(local_id.index) ? mlc::Array<mlc::String>{} : mlc::Array<mlc::String>{label + mlc::String(": invalid local id ") + mlc::to_string(local_id.index)};}

mlc::Array<mlc::String> verify_mir_block_id(mlc::String label, mir_types::BlockId block_id) noexcept{return mir_types::mir_id_is_valid(block_id.index) ? mlc::Array<mlc::String>{} : mlc::Array<mlc::String>{label + mlc::String(": invalid block id ") + mlc::to_string(block_id.index)};}

bool verify_mir_block_exists(mlc::Array<mir_types::MirBlock> blocks, mir_types::BlockId block_id) noexcept{
int index = 0;
while (index < blocks.size()){
{
if (blocks[index].id.index == block_id.index){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

mlc::Array<mlc::String> verify_mir_terminator(mir_types::MirFunction function, mir_types::MirBlock block) noexcept{
mlc::Array<mlc::String> errors = {};
return std::visit(overloaded{
  [&](const MirReturn& mirreturn) -> mlc::Array<mlc::String> { auto [_w0] = mirreturn; return errors; },
  [&](const MirJump& mirjump) -> mlc::Array<mlc::String> { auto [target] = mirjump; return verify_mir_block_exists(function.blocks, target) ? errors : verify_mir_append(errors, mlc::Array<mlc::String>{mlc::String("mir jump: unknown block ") + mlc::to_string(target.index)}); },
  [&](const MirCondJump& mircondjump) -> mlc::Array<mlc::String> { auto [cond, then_block, else_block] = mircondjump; return [&]() -> mlc::Array<mlc::String> { 
  if (!verify_mir_block_exists(function.blocks, then_block)){
{
errors = verify_mir_append(errors, mlc::Array<mlc::String>{mlc::String("mir cond jump: unknown then block ") + mlc::to_string(then_block.index)});
}
}
  if (!verify_mir_block_exists(function.blocks, else_block)){
{
errors = verify_mir_append(errors, mlc::Array<mlc::String>{mlc::String("mir cond jump: unknown else block ") + mlc::to_string(else_block.index)});
}
}
  return errors;
 }(); },
  [&](const MirUnreachable& mirunreachable) -> mlc::Array<mlc::String> { return errors; }
}, block.term);
}

mlc::Array<mlc::String> verify_mir_block(mir_types::MirFunction function, mir_types::MirBlock block) noexcept{
mlc::Array<mlc::String> errors = {};
if (!mir_types::mir_block_label_is_safe(block.label)){
{
errors.push_back(mlc::String("mir block: unsafe label"));
}
}
errors = verify_mir_append(errors, verify_mir_block_id(mlc::String("mir block"), block.id));
return verify_mir_append(errors, verify_mir_terminator(function, block));
}

mlc::Array<mlc::String> verify_mir_function(mir_types::MirFunction function) noexcept{
mlc::Array<mlc::String> errors = {};
if (!mir_types::mir_function_name_is_safe(function.name)){
{
errors.push_back(mlc::String("mir function: unsafe name"));
}
}
if (function.blocks.size() == 0){
{
errors.push_back(mlc::String("mir function: no blocks"));
}
}
int param_index = 0;
while (param_index < function.params.size()){
{
param_index = param_index + 1;
}
}
int local_index = 0;
while (local_index < function.locals.size()){
{
mir_types::MirLocal local = function.locals[local_index];
errors = verify_mir_append(errors, verify_mir_local_id(mlc::String("mir local"), local.id));
local_index = local_index + 1;
}
}
if (function.blocks.size() > 0 && !verify_mir_block_exists(function.blocks, function.blocks[0].id)){
{
errors.push_back(mlc::String("mir function: entry block missing from block list"));
}
}
int block_index = 0;
while (block_index < function.blocks.size()){
{
errors = verify_mir_append(errors, verify_mir_block(function, function.blocks[block_index]));
block_index = block_index + 1;
}
}
return errors;
}

mlc::Array<mlc::String> verify_mir_module(mir_types::MirModule mir_module) noexcept{
mlc::Array<mlc::String> errors = {};
int index = 0;
while (index < mir_module.functions.size()){
{
errors = verify_mir_append(errors, verify_mir_function(mir_module.functions[index]));
index = index + 1;
}
}
return errors;
}

mlc::Array<mlc::String> verify_mir_program(mir_types::MirProgram program) noexcept{
mlc::Array<mlc::String> errors = {};
int index = 0;
while (index < program.modules.size()){
{
errors = verify_mir_append(errors, verify_mir_module(program.modules[index]));
index = index + 1;
}
}
return errors;
}

} // namespace verify_mir
