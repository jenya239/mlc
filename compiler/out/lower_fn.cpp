#define main mlc_user_main
#include "lower_fn.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "mir_types.hpp"
#include "mir_ids.hpp"

namespace lower_fn {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace mir_types;
using namespace mir_ids;
using namespace ast_tokens;

std::shared_ptr<registry::Type> mir_lower_unknown_type() noexcept{
  return std::make_shared<registry::Type>(registry::TUnknown{});
}
bool mir_lower_is_assign_operation(mlc::String operation) noexcept{
  return (operation == mlc::String("=", 1));
}
MirLowerState mir_lower_state_new() noexcept{
  return MirLowerState{{}, {}, {}, {}, mir_types::mir_block_id(0), mlc::String("entry", 5), {}, mir_ids::mir_id_pool_new()};
}
MirLowerState mir_lower_bind_lambda(MirLowerState state, mlc::String name, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept{
  auto bindings = state.lambda_bindings;
  bindings.push_back(MirLambdaBinding{name, parameter_names, body});
  return MirLowerState{state.name_environment, bindings, state.locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, state.id_pool};
}
ast::Result<MirLambdaBinding, mlc::Array<mlc::String>> mir_lower_lookup_lambda(MirLowerState state, mlc::String name) noexcept{
  auto index = 0;
  while ((index < state.lambda_bindings.length()))   {
    auto binding = state.lambda_bindings[index];
    if ((binding.name == name))     {
      return ast::Ok<MirLambdaBinding>{binding};
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("mir lower: unknown lambda ", 26) + mlc::to_string(name)) + mlc::String("", 0))}};
}
MirLowerBlockStep mir_lower_allocate_block_index(MirLowerState state) noexcept{
  auto block_step = mir_ids::mir_id_pool_allocate_block(state.id_pool);
  return MirLowerBlockStep{MirLowerState{state.name_environment, state.lambda_bindings, state.locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, block_step.pool}, block_step.block_id};
}
MirLowerState mir_lower_finish_current_block(MirLowerState state, mir_types::MirTerminator terminator) noexcept{
  auto finished_blocks = state.finished_blocks;
  finished_blocks.push_back(mir_types::MirBlock{state.current_block_id, state.current_label, state.current_stmts, terminator});
  return MirLowerState{state.name_environment, state.lambda_bindings, state.locals, finished_blocks, state.current_block_id, state.current_label, {}, state.id_pool};
}
MirLowerState mir_lower_start_block(MirLowerState state, mir_types::BlockId block_id, mlc::String label) noexcept{
  return MirLowerState{state.name_environment, state.lambda_bindings, state.locals, state.finished_blocks, block_id, label, {}, state.id_pool};
}
MirLowerState mir_lower_bind_params(MirLowerState state, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
  auto environment = state.name_environment;
  auto index = 0;
  while ((index < params.length()))   {
    environment.set(params[index]->name, mir_types::mir_local_id(index));
    (index = mlc::arith::checked_add(index, 1));
  }
  return MirLowerState{environment, state.lambda_bindings, state.locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, mir_ids::mir_id_pool_locals_reserved(state.id_pool, index)};
}
MirLowerLocalStep mir_lower_allocate_local(MirLowerState state, mlc::String name, std::shared_ptr<registry::Type> type_value) noexcept{
  auto local_step = mir_ids::mir_id_pool_allocate_local(state.id_pool);
  auto locals = state.locals;
  locals.push_back(mir_types::MirLocal{local_step.local_id, name, type_value});
  return MirLowerLocalStep{MirLowerState{state.name_environment, state.lambda_bindings, locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, local_step.pool}, local_step.local_id};
}
MirLowerState mir_lower_bind_name(MirLowerState state, mlc::String name, mir_types::LocalId local_id) noexcept{
  auto environment = state.name_environment;
  environment.set(name, local_id);
  return MirLowerState{environment, state.lambda_bindings, state.locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, state.id_pool};
}
ast::Result<mir_types::LocalId, mlc::Array<mlc::String>> mir_lower_lookup_local(MirLowerState state, mlc::String name) noexcept{
  if (state.name_environment.has(name))   {
    return ast::Ok<mir_types::LocalId>{state.name_environment.get(name)};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("mir lower: unknown local ", 25) + mlc::to_string(name)) + mlc::String("", 0))}};
  }
}
MirLowerState mir_lower_emit_assign(MirLowerState state, mir_types::LocalId local_id, mir_types::MirRvalue rvalue) noexcept{
  auto statements = state.current_stmts;
  statements.push_back(mir_types::MirAssign{local_id, rvalue});
  return MirLowerState{state.name_environment, state.lambda_bindings, state.locals, state.finished_blocks, state.current_block_id, state.current_label, statements, state.id_pool};
}
MirLowerState mir_lower_emit_call_assign(MirLowerState state, mir_types::LocalId local_id, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept{
  auto statements = state.current_stmts;
  statements.push_back(mir_types::MirCallAssign{local_id, callee_name, arguments});
  return MirLowerState{state.name_environment, state.lambda_bindings, state.locals, state.finished_blocks, state.current_block_id, state.current_label, statements, state.id_pool};
}
bool mir_lower_is_variant_ctor_name(mlc::String name) noexcept{
  return ((((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1))) && (!name.contains(mlc::String("_", 1))));
}
ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_call_target_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return ast::Ok<mlc::String>{name};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [receiver, field_name, __2, __3] = semanticExpressionField; return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> {
auto __match_subject = receiver;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [type_name, __1, __2] = semanticExpressionIdent; return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> {
  if (((type_name == mlc::String("Map", 3)) && (field_name == mlc::String("new", 3))))   {
    return ast::Ok<mlc::String>{mlc::String("__mir_map_empty", 15)};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: unsupported static field call", 40)}};
  }
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: call target must be identifier", 41)}};
std::abort();
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: call target must be identifier", 41)}};
std::abort();
}();
}
mlc::Array<mir_types::MirOperand> mir_lower_variant_new_arguments(mlc::String tag, mlc::Array<mir_types::MirOperand> payload_operands) noexcept{
  auto arguments = mlc::Array<mir_types::MirOperand>{mir_types::MirOperandConstStr{tag}};
  auto index = 0;
  while ((index < payload_operands.length()))   {
    arguments.push_back(payload_operands[index]);
    (index = mlc::arith::checked_add(index, 1));
  }
  return arguments;
}
ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_method_native_name(mlc::String method_name) noexcept{
  if ((method_name == mlc::String("push", 4)))   {
    return ast::Ok<mlc::String>{mlc::String("__mir_array_push", 16)};
  } else if ((method_name == mlc::String("pop", 3)))   {
    return ast::Ok<mlc::String>{mlc::String("__mir_array_pop", 15)};
  } else if ((method_name == mlc::String("get", 3)))   {
    return ast::Ok<mlc::String>{mlc::String("__mir_map_get", 13)};
  } else if ((method_name == mlc::String("set", 3)))   {
    return ast::Ok<mlc::String>{mlc::String("__mir_map_set", 13)};
  } else if ((method_name == mlc::String("length", 6)))   {
    return ast::Ok<mlc::String>{mlc::String("__mir_length", 12)};
  } else if ((method_name == mlc::String("contains", 8)))   {
    return ast::Ok<mlc::String>{mlc::String("__mir_string_contains", 21)};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("mir lower: unsupported method ", 30) + mlc::to_string(method_name)) + mlc::String("", 0))}};
  }
}
bool mir_lower_is_mutating_method(mlc::String method_name) noexcept{
  return (((method_name == mlc::String("push", 4)) || (method_name == mlc::String("set", 3))) || (method_name == mlc::String("pop", 3)));
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_push_elements(MirLowerState state, mir_types::LocalId array_local_id, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, int index) noexcept{
  if ((index >= elements.length()))   {
    return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{state, array_local_id}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [element_step] = ok; return [&]() {
auto push_result_step = mir_lower_allocate_local(element_step.state, mlc::String("", 0), mir_lower_unknown_type());
auto push_state = mir_lower_emit_call_assign(push_result_step.state, push_result_step.local_id, mlc::String("__mir_array_push", 16), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{array_local_id}, element_step.operand});
return mir_lower_array_push_elements(push_state, push_result_step.local_id, elements, mlc::arith::checked_add(index, 1));
}(); }
}, mir_lower_operand_from_expression(state, elements[index]));
  }
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_literal_to_local(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements) noexcept{
  auto empty_step = mir_lower_allocate_local(state, mlc::String("", 0), mir_lower_unknown_type());
  auto empty_state = mir_lower_emit_call_assign(empty_step.state, empty_step.local_id, mlc::String("__mir_array_empty", 17), mlc::Array<mir_types::MirOperand>{});
  return mir_lower_array_push_elements(empty_state, empty_step.local_id, elements, 0);
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_index_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [object_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [index_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(index_step.state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_array_get", 15), mlc::Array<mir_types::MirOperand>{object_step.operand, index_step.operand}), local_step.local_id}};
}(); }
}, mir_lower_operand_from_expression(object_step.state, index_expression)); }
}, mir_lower_operand_from_expression(state, object));
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_map_new_local(MirLowerState state) noexcept{
  auto local_step = mir_lower_allocate_local(state, mlc::String("", 0), mir_lower_unknown_type());
  return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_map_empty", 15), mlc::Array<mir_types::MirOperand>{}), local_step.local_id}};
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_shared_new_to_local(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{
  if ((arguments.length() != 1))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: Shared.new expects 1 argument", 40)}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [argument_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(argument_step.state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_shared_new", 16), mlc::Array<mir_types::MirOperand>{argument_step.operand}), local_step.local_id}};
}(); }
}, mir_lower_operand_from_expression(state, arguments[0]));
  }
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_pop_expression(MirLowerState state, mlc::String array_name) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [array_local_id] = ok; return [&]() {
auto length_step = mir_lower_allocate_local(state, mlc::String("", 0), mir_lower_unknown_type());
auto next_state = mir_lower_emit_call_assign(length_step.state, length_step.local_id, mlc::String("__mir_length", 12), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{array_local_id}});
auto subtract_op = mlc::String("-", 1);
auto last_index_step = mir_lower_temp_operand_from_rvalue(next_state, mir_types::MirRvalueBinary{subtract_op, mir_types::MirOperandLocal{length_step.local_id}, mir_types::MirOperandConstInt{1}});
(next_state = last_index_step.state);
auto value_step = mir_lower_allocate_local(next_state, mlc::String("", 0), mir_lower_unknown_type());
(next_state = mir_lower_emit_call_assign(value_step.state, value_step.local_id, mlc::String("__mir_array_get", 15), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{array_local_id}, last_index_step.operand}));
auto popped_step = mir_lower_allocate_local(next_state, mlc::String("", 0), mir_lower_unknown_type());
(next_state = mir_lower_emit_call_assign(popped_step.state, popped_step.local_id, mlc::String("__mir_array_pop", 15), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{array_local_id}}));
(next_state = mir_lower_emit_assign(next_state, array_local_id, mir_types::MirRvalueUse{mir_types::MirOperandLocal{popped_step.local_id}}));
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{next_state, value_step.local_id}};
}(); }
}, mir_lower_lookup_local(state, array_name));
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{
  if (((method_name == mlc::String("pop", 3)) && (arguments.length() == 0)))   {
    return [&]() -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> {
auto __match_subject = object;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [array_name, __1, __2] = semanticExpressionIdent; return mir_lower_array_pop_expression(state, array_name);
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: pop requires identifier receiver", 43)}};
std::abort();
}();
  } else   {
    return [&]() -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> {
auto __match_subject = object;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [type_name, __1, __2] = semanticExpressionIdent; return ((((type_name == mlc::String("Map", 3)) && (method_name == mlc::String("new", 3))) && (arguments.length() == 0)) ? (mir_lower_map_new_local(state)) : ((((type_name == mlc::String("Shared", 6)) && (method_name == mlc::String("new", 3))) ? (mir_lower_shared_new_to_local(state, arguments)) : (mir_lower_method_call_on_operand(state, object, method_name, arguments)))));
}
return mir_lower_method_call_on_operand(state, object, method_name, arguments);
std::abort();
}();
  }
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_call_on_operand(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mlc::String>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [native_name] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [object_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerMethodOperandsStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(operands_step.state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, native_name, operands_step.operands), local_step.local_id}};
}(); }
}, mir_lower_method_operands_with_receiver(object_step.state, object_step.operand, arguments)); }
}, mir_lower_operand_from_expression(state, object)); }
}, mir_lower_method_native_name(method_name));
}
ast::Result<MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_method_operands(MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept{
  if ((index >= arguments.length()))   {
    return ast::Ok<MirLowerMethodOperandsStep>{MirLowerMethodOperandsStep{state, operands}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerMethodOperandsStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerMethodOperandsStep, mlc::Array<mlc::String>> { auto [argument_step] = ok; return [&]() {
auto next_operands = operands;
next_operands.push_back(argument_step.operand);
return mir_lower_collect_method_operands(argument_step.state, object_operand, arguments, mlc::arith::checked_add(index, 1), next_operands);
}(); }
}, mir_lower_operand_from_expression(state, arguments[index]));
  }
}
ast::Result<MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_method_operands_with_receiver(MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{
  auto operands = mlc::Array<mir_types::MirOperand>{object_operand};
  return mir_lower_collect_method_operands(state, object_operand, arguments, 0, operands);
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_mutating_method_statement(MirLowerState state, mir_types::LocalId local_id, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mlc::String>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [native_name] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerMethodOperandsStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() {
auto result_step = mir_lower_allocate_local(operands_step.state, mlc::String("", 0), mir_lower_unknown_type());
auto call_state = mir_lower_emit_call_assign(result_step.state, result_step.local_id, native_name, operands_step.operands);
return ast::Ok<MirLowerState>{mir_lower_emit_assign(call_state, local_id, mir_types::MirRvalueUse{mir_types::MirOperandLocal{result_step.local_id}})};
}(); }
}, mir_lower_method_operands_with_receiver(state, mir_types::MirOperandLocal{local_id}, arguments)); }
}, mir_lower_method_native_name(method_name));
}
ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_operands(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept{
  if ((index >= arguments.length()))   {
    return ast::Ok<MirLowerCallOperandsStep>{MirLowerCallOperandsStep{state, operands}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> { auto [step] = ok; return [&]() {
auto next_operands = operands;
next_operands.push_back(step.operand);
return mir_lower_collect_operands(step.state, arguments, mlc::arith::checked_add(index, 1), next_operands);
}(); }
}, mir_lower_operand_from_expression(state, arguments[index]));
  }
}
ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_operands_from_arguments(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{
  return mir_lower_collect_operands(state, arguments, 0, mlc::Array<mir_types::MirOperand>{});
}
ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_record_new_arguments_from_fields(MirLowerState state, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, int index, mlc::Array<mir_types::MirOperand> arguments) noexcept{
  if ((index >= field_values.length()))   {
    return ast::Ok<MirLowerCallOperandsStep>{MirLowerCallOperandsStep{state, arguments}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> { auto [value_step] = ok; return [&]() {
auto next_arguments = arguments;
next_arguments.push_back(mir_types::MirOperandConstStr{field_values[index]->name});
next_arguments.push_back(value_step.operand);
return mir_lower_record_new_arguments_from_fields(value_step.state, type_name, field_values, mlc::arith::checked_add(index, 1), next_arguments);
}(); }
}, mir_lower_operand_from_expression(state, field_values[index]->value));
  }
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_record_literal_to_local(MirLowerState state, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerCallOperandsStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [arguments_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(arguments_step.state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_record_new", 16), arguments_step.operands), local_step.local_id}};
}(); }
}, mir_lower_record_new_arguments_from_fields(state, type_name, field_values, 0, mlc::Array<mir_types::MirOperand>{mir_types::MirOperandConstStr{type_name}}));
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_record_field_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [object_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(object_step.state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_record_field", 18), mlc::Array<mir_types::MirOperand>{object_step.operand, mir_types::MirOperandConstStr{field_name}}), local_step.local_id}};
}(); }
}, mir_lower_operand_from_expression(state, object));
}
MirLowerOperandStep mir_lower_temp_operand_from_rvalue(MirLowerState state, mir_types::MirRvalue rvalue) noexcept{
  auto local_step = mir_lower_allocate_local(state, mlc::String("", 0), mir_lower_unknown_type());
  auto assigned_state = mir_lower_emit_assign(local_step.state, local_step.local_id, rvalue);
  return MirLowerOperandStep{assigned_state, mir_types::MirOperandLocal{local_step.local_id}};
}
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_int(MirLowerState state, mir_types::MirOperand left_operand, int right_value) noexcept{
  return ast::Ok<MirLowerOperandStep>{mir_lower_temp_operand_from_rvalue(state, mir_types::MirRvalueBinary{mlc::String("==", 2), left_operand, mir_types::MirOperandConstInt{right_value}})};
}
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_bool(MirLowerState state, mir_types::MirOperand left_operand, bool right_value) noexcept{
  return ast::Ok<MirLowerOperandStep>{mir_lower_temp_operand_from_rvalue(state, mir_types::MirRvalueBinary{mlc::String("==", 2), left_operand, mir_types::MirOperandConstBool{right_value}})};
}
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_from_expression(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*__match_subject))) {
const semantic_ir::SemanticExpressionInt& semanticExpressionInt = std::get<semantic_ir::SemanticExpressionInt>((*__match_subject));
auto [value, __1, __2] = semanticExpressionInt; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{state, mir_types::MirOperandConstInt{value}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*__match_subject))) {
const semantic_ir::SemanticExpressionBool& semanticExpressionBool = std::get<semantic_ir::SemanticExpressionBool>((*__match_subject));
auto [value, __1, __2] = semanticExpressionBool; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{state, mir_types::MirOperandConstBool{value}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*__match_subject))) {
const semantic_ir::SemanticExpressionStr& semanticExpressionStr = std::get<semantic_ir::SemanticExpressionStr>((*__match_subject));
auto [value, __1, __2] = semanticExpressionStr; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{state, mir_types::MirOperandConstStr{value}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{state, mir_types::MirOperandUnit{}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*__match_subject))) {
const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple = std::get<semantic_ir::SemanticExpressionTuple>((*__match_subject));
auto [elements, __1, __2] = semanticExpressionTuple; return [&]() -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> {
  if ((elements.length() == 0))   {
    return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{state, mir_types::MirOperandUnit{}}};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: non-empty tuple is not an operand", 44)}};
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return std::visit(overloaded{[&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_id] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{state, mir_types::MirOperandLocal{local_id}}}; },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [__0] = err; return [&]() -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> {
  if (mir_lower_is_variant_ctor_name(name))   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("mir lower: unknown identifier ", 30) + mlc::to_string(name)) + mlc::String("", 0))}};
  }
}(); }
}, mir_lower_lookup_local(state, name));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*__match_subject))) {
const semantic_ir::SemanticExpressionBin& semanticExpressionBin = std::get<semantic_ir::SemanticExpressionBin>((*__match_subject));
auto [operation, __1, __2, __3, __4] = semanticExpressionBin; return [&]() -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> {
  if (mir_lower_is_assign_operation(operation))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: assignment expression is not an operand", 50)}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<MirLowerOperandStep>{mir_lower_temp_operand_from_rvalue(rvalue_step.state, rvalue_step.rvalue)}; }
}, mir_lower_rvalue_from_expression(state, expression));
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*__match_subject))) {
const semantic_ir::SemanticExpressionUn& semanticExpressionUn = std::get<semantic_ir::SemanticExpressionUn>((*__match_subject));
auto [operation, __1, __2, __3] = semanticExpressionUn; return [&]() -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> {
  if ((operation == mlc::String("move", 4)))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: move expression is not supported in MIR operand context", 66)}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<MirLowerOperandStep>{mir_lower_temp_operand_from_rvalue(rvalue_step.state, rvalue_step.rvalue)}; }
}, mir_lower_rvalue_from_expression(state, expression));
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [function, arguments, __2, __3, __4] = semanticExpressionCall; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [__0, __1, __2] = semanticExpressionArray; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionIndex; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord = std::get<semantic_ir::SemanticExpressionRecord>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionRecord; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionField; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [__0, __1, __2] = semanticExpressionQuestion; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3, __4] = semanticExpressionIf; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_if_to_local(state, condition, then_expression, else_expression));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: unsupported expression in operand context", 52)}};
std::abort();
}();
}
ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> mir_lower_rvalue_from_expression(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*__match_subject))) {
const semantic_ir::SemanticExpressionInt& semanticExpressionInt = std::get<semantic_ir::SemanticExpressionInt>((*__match_subject));
auto [value, __1, __2] = semanticExpressionInt; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{state, mir_types::MirRvalueUse{mir_types::MirOperandConstInt{value}}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*__match_subject))) {
const semantic_ir::SemanticExpressionBool& semanticExpressionBool = std::get<semantic_ir::SemanticExpressionBool>((*__match_subject));
auto [value, __1, __2] = semanticExpressionBool; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{state, mir_types::MirRvalueUse{mir_types::MirOperandConstBool{value}}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*__match_subject))) {
const semantic_ir::SemanticExpressionStr& semanticExpressionStr = std::get<semantic_ir::SemanticExpressionStr>((*__match_subject));
auto [value, __1, __2] = semanticExpressionStr; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{state, mir_types::MirRvalueUse{mir_types::MirOperandConstStr{value}}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{state, mir_types::MirRvalueUse{mir_types::MirOperandUnit{}}}};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return std::visit(overloaded{[&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_id] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_id}}}}; },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [__0] = err; return [&]() -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> {
  if (mir_lower_is_variant_ctor_name(name))   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("mir lower: unknown identifier ", 30) + mlc::to_string(name)) + mlc::String("", 0))}};
  }
}(); }
}, mir_lower_lookup_local(state, name));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*__match_subject))) {
const semantic_ir::SemanticExpressionBin& semanticExpressionBin = std::get<semantic_ir::SemanticExpressionBin>((*__match_subject));
auto [operation, left, right, __3, __4] = semanticExpressionBin; return [&]() -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> {
  if (mir_lower_is_assign_operation(operation))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: assignment is not an rvalue", 38)}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [left_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [right_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{right_step.state, mir_types::MirRvalueBinary{operation, left_step.operand, right_step.operand}}}; }
}, mir_lower_operand_from_expression(left_step.state, right)); }
}, mir_lower_operand_from_expression(state, left));
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*__match_subject))) {
const semantic_ir::SemanticExpressionUn& semanticExpressionUn = std::get<semantic_ir::SemanticExpressionUn>((*__match_subject));
auto [operation, inner, __2, __3] = semanticExpressionUn; return [&]() -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> {
  if ((operation == mlc::String("move", 4)))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: move expression is not supported in MIR rvalue context", 65)}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [inner_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{inner_step.state, mir_types::MirRvalueUnary{operation, inner_step.operand}}}; }
}, mir_lower_operand_from_expression(state, inner));
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [function, arguments, __2, __3, __4] = semanticExpressionCall; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [__0, __1, __2] = semanticExpressionArray; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionIndex; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord = std::get<semantic_ir::SemanticExpressionRecord>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionRecord; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionField; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [__0, __1, __2] = semanticExpressionQuestion; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_expression_to_local(state, expression));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3, __4] = semanticExpressionIf; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerRvalueStep>{MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse{mir_types::MirOperandLocal{local_step.local_id}}}}; }
}, mir_lower_if_to_local(state, condition, then_expression, else_expression));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: unsupported expression in rvalue context", 51)}};
std::abort();
}();
}
MirLowerState mir_lower_finish_return(MirLowerState state, mir_types::MirOperand operand) noexcept{
  return mir_lower_finish_current_block(state, mir_types::MirReturn{operand});
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_return_expression(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [statement_state] = ok; return mir_lower_return_expression(statement_state, result_expression); }
}, mir_lower_statements(state, statements));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3, __4] = semanticExpressionIf; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() {
auto then_block_step = mir_lower_allocate_block_index(condition_step.state);
auto else_block_step = mir_lower_allocate_block_index(then_block_step.state);
auto branch_state = mir_lower_finish_current_block(else_block_step.state, mir_types::MirCondJump{condition_step.operand, then_block_step.block_id, else_block_step.block_id});
(branch_state = mir_lower_start_block(branch_state, then_block_step.block_id, mlc::String("then", 4)));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [then_state] = ok; return [&]() {
auto else_state = mir_lower_start_block(then_state, else_block_step.block_id, mlc::String("else", 4));
return mir_lower_return_expression(else_state, else_expression);
}(); }
}, mir_lower_return_expression(branch_state, then_expression));
}(); }
}, mir_lower_operand_from_expression(state, condition));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return ast::Ok<MirLowerState>{mir_lower_finish_return(state, mir_types::MirOperandUnit{})};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*__match_subject))) {
const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch = std::get<semantic_ir::SemanticExpressionMatch>((*__match_subject));
auto [subject, arms, __2, __3] = semanticExpressionMatch; return mir_lower_match_return(state, subject, arms);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [inner, __1, __2] = semanticExpressionQuestion; return mir_lower_question_return(state, inner);
}
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [operand_step] = ok; return ast::Ok<MirLowerState>{mir_lower_finish_return(operand_step.state, operand_step.operand)}; }
}, mir_lower_operand_from_expression(state, expression));
std::abort();
}();
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_emit_call_to_local(MirLowerState state, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept{
  auto local_step = mir_lower_allocate_local(state, mlc::String("", 0), mir_lower_unknown_type());
  return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, callee_name, arguments), local_step.local_id}};
}
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_and(MirLowerState state, mir_types::MirOperand left_operand, mir_types::MirOperand right_operand) noexcept{
  return ast::Ok<MirLowerOperandStep>{mir_lower_temp_operand_from_rvalue(state, mir_types::MirRvalueBinary{mlc::String("&&", 2), left_operand, right_operand})};
}
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_variant_is(MirLowerState state, mir_types::MirOperand scrutinee, mlc::String tag) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal{local_step.local_id}}}; }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_is", 16), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstStr{tag}}));
}
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_ctor_subpattern_tests(MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mir_types::MirOperand accumulated, int index) noexcept{
  if ((index >= sub_patterns.length()))   {
    return ast::Ok<MirLowerOperandStep>{MirLowerOperandStep{state, accumulated}};
  } else   {
    return [&]() -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> {
auto __match_subject = sub_patterns[index];
if (std::holds_alternative<ast::PatternInt>((*__match_subject))) {
const ast::PatternInt& patternInt = std::get<ast::PatternInt>((*__match_subject));
auto [value, __1] = patternInt; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [field_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [eq_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [and_step] = ok; return mir_lower_ctor_subpattern_tests(and_step.state, scrutinee, sub_patterns, and_step.operand, mlc::arith::checked_add(index, 1)); }
}, mir_lower_operand_and(eq_step.state, accumulated, eq_step.operand)); }
}, mir_lower_operand_equals_const_int(field_step.state, mir_types::MirOperandLocal{field_step.local_id}, value)); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field", 19), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt{index}}));
}
if (std::holds_alternative<ast::PatternBool>((*__match_subject))) {
const ast::PatternBool& patternBool = std::get<ast::PatternBool>((*__match_subject));
auto [value, __1] = patternBool; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [field_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [eq_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [and_step] = ok; return mir_lower_ctor_subpattern_tests(and_step.state, scrutinee, sub_patterns, and_step.operand, mlc::arith::checked_add(index, 1)); }
}, mir_lower_operand_and(eq_step.state, accumulated, eq_step.operand)); }
}, mir_lower_operand_equals_const_bool(field_step.state, mir_types::MirOperandLocal{field_step.local_id}, value)); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field", 19), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt{index}}));
}
if (std::holds_alternative<ast::PatternWild>((*__match_subject))) {
const ast::PatternWild& patternWild = std::get<ast::PatternWild>((*__match_subject));
auto [__0] = patternWild; return mir_lower_ctor_subpattern_tests(state, scrutinee, sub_patterns, accumulated, mlc::arith::checked_add(index, 1));
}
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [__0, __1] = patternIdent; return mir_lower_ctor_subpattern_tests(state, scrutinee, sub_patterns, accumulated, mlc::arith::checked_add(index, 1));
}
if (std::holds_alternative<ast::PatternUnit>((*__match_subject))) {
const ast::PatternUnit& patternUnit = std::get<ast::PatternUnit>((*__match_subject));
auto [__0] = patternUnit; return mir_lower_ctor_subpattern_tests(state, scrutinee, sub_patterns, accumulated, mlc::arith::checked_add(index, 1));
}
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, __1, __2] = patternCtor; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [field_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [nested_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [and_step] = ok; return mir_lower_ctor_subpattern_tests(and_step.state, scrutinee, sub_patterns, and_step.operand, mlc::arith::checked_add(index, 1)); }
}, mir_lower_operand_and(nested_step.state, accumulated, nested_step.operand)); }
}, mir_lower_pattern_matches_operand(field_step.state, mir_types::MirOperandLocal{field_step.local_id}, sub_patterns[index])); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field", 19), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt{index}}));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: unsupported variant subpattern in match test", 55)}};
std::abort();
}();
  }
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpattern(MirLowerState state, mir_types::MirOperand scrutinee, int field_index, std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternWild>((*__match_subject))) {
const ast::PatternWild& patternWild = std::get<ast::PatternWild>((*__match_subject));
auto [__0] = patternWild; return ast::Ok<MirLowerState>{state};
}
if (std::holds_alternative<ast::PatternUnit>((*__match_subject))) {
const ast::PatternUnit& patternUnit = std::get<ast::PatternUnit>((*__match_subject));
auto [__0] = patternUnit; return ast::Ok<MirLowerState>{state};
}
if (std::holds_alternative<ast::PatternInt>((*__match_subject))) {
const ast::PatternInt& patternInt = std::get<ast::PatternInt>((*__match_subject));
auto [__0, __1] = patternInt; return ast::Ok<MirLowerState>{state};
}
if (std::holds_alternative<ast::PatternBool>((*__match_subject))) {
const ast::PatternBool& patternBool = std::get<ast::PatternBool>((*__match_subject));
auto [__0, __1] = patternBool; return ast::Ok<MirLowerState>{state};
}
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [name, __1] = patternIdent; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [field_step] = ok; return ast::Ok<MirLowerState>{mir_lower_bind_name(field_step.state, name, field_step.local_id)}; }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field", 19), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt{field_index}}));
}
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, nested_sub_patterns, __2] = patternCtor; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [field_step] = ok; return mir_lower_bind_ctor_subpatterns(field_step.state, mir_types::MirOperandLocal{field_step.local_id}, nested_sub_patterns, 0); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field", 19), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt{field_index}}));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: unsupported variant subpattern binding", 49)}};
std::abort();
}();
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpatterns(MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, int index) noexcept{
  if ((index >= sub_patterns.length()))   {
    return ast::Ok<MirLowerState>{state};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [bound_state] = ok; return mir_lower_bind_ctor_subpatterns(bound_state, scrutinee, sub_patterns, mlc::arith::checked_add(index, 1)); }
}, mir_lower_bind_ctor_subpattern(state, scrutinee, index, sub_patterns[index]));
  }
}
bool mir_lower_pattern_needs_conditional_test(std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> bool {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternInt>((*__match_subject))) {
const ast::PatternInt& patternInt = std::get<ast::PatternInt>((*__match_subject));
auto [__0, __1] = patternInt; return true;
}
if (std::holds_alternative<ast::PatternBool>((*__match_subject))) {
const ast::PatternBool& patternBool = std::get<ast::PatternBool>((*__match_subject));
auto [__0, __1] = patternBool; return true;
}
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, __1, __2] = patternCtor; return true;
}
return false;
std::abort();
}();
}
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_pattern_matches_operand(MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternInt>((*__match_subject))) {
const ast::PatternInt& patternInt = std::get<ast::PatternInt>((*__match_subject));
auto [value, __1] = patternInt; return mir_lower_operand_equals_const_int(state, scrutinee, value);
}
if (std::holds_alternative<ast::PatternBool>((*__match_subject))) {
const ast::PatternBool& patternBool = std::get<ast::PatternBool>((*__match_subject));
auto [value, __1] = patternBool; return mir_lower_operand_equals_const_bool(state, scrutinee, value);
}
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [tag, sub_patterns, __2] = patternCtor; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> { auto [is_step] = ok; return mir_lower_ctor_subpattern_tests(is_step.state, scrutinee, sub_patterns, is_step.operand, 0); }
}, mir_lower_operand_variant_is(state, scrutinee, tag));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: staged match supports int, bool, and variant ctor patterns", 69)}};
std::abort();
}();
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_record_field_patterns(MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, int index) noexcept{
  if ((index >= field_patterns.length()))   {
    return ast::Ok<MirLowerState>{state};
  } else   {
    return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = field_patterns[index];
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [field_name, __1] = patternIdent; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [field_step] = ok; return mir_lower_bind_record_field_patterns(mir_lower_bind_name(field_step.state, field_name, field_step.local_id), scrutinee, field_patterns, mlc::arith::checked_add(index, 1)); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_record_field", 18), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstStr{field_name}}));
}
if (std::holds_alternative<ast::PatternWild>((*__match_subject))) {
const ast::PatternWild& patternWild = std::get<ast::PatternWild>((*__match_subject));
auto [__0] = patternWild; return mir_lower_bind_record_field_patterns(state, scrutinee, field_patterns, mlc::arith::checked_add(index, 1));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: unsupported record field pattern", 43)}};
std::abort();
}();
  }
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_match_arm_pattern(MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<ast::Pattern> pattern) noexcept{
  return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [name, __1] = patternIdent; return [&]() {
auto local_step = mir_lower_allocate_local(state, name, mir_lower_unknown_type());
auto bound_state = mir_lower_emit_assign(local_step.state, local_step.local_id, mir_types::MirRvalueUse{scrutinee});
(bound_state = mir_lower_bind_name(bound_state, name, local_step.local_id));
return ast::Ok<MirLowerState>{bound_state};
}();
}
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, sub_patterns, __2] = patternCtor; return mir_lower_bind_ctor_subpatterns(state, scrutinee, sub_patterns, 0);
}
if (std::holds_alternative<ast::PatternRecord>((*__match_subject))) {
const ast::PatternRecord& patternRecord = std::get<ast::PatternRecord>((*__match_subject));
auto [__0, field_patterns, __2] = patternRecord; return mir_lower_bind_record_field_patterns(state, scrutinee, field_patterns, 0);
}
return ast::Ok<MirLowerState>{state};
std::abort();
}();
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arm_after_bind(MirLowerState state, std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mir_types::BlockId guard_fail_block) noexcept{
  if ((!arm->has_guard))   {
    return mir_lower_return_expression(state, arm->body);
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [guard_step] = ok; return [&]() {
auto body_block_step = mir_lower_allocate_block_index(guard_step.state);
auto branch_state = mir_lower_finish_current_block(body_block_step.state, mir_types::MirCondJump{guard_step.operand, body_block_step.block_id, guard_fail_block});
(branch_state = mir_lower_start_block(branch_state, body_block_step.block_id, mlc::String("match_guard_body", 16)));
return mir_lower_return_expression(branch_state, arm->body);
}(); }
}, mir_lower_operand_from_expression(state, arm->when_condition));
  }
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arm_body_with_binding(MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<semantic_ir::SemanticMatchArm> arm, mir_types::BlockId guard_fail_block) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [bound_state] = ok; return mir_lower_match_arm_after_bind(bound_state, arm, guard_fail_block); }
}, mir_lower_bind_match_arm_pattern(state, scrutinee, arm->pattern));
}
MirLowerState mir_lower_finish_unreachable(MirLowerState state) noexcept{
  return mir_lower_finish_current_block(state, mir_types::MirUnreachable{});
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arms_return(MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int index) noexcept{
  if ((index >= arms.length()))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: empty match", 22)}};
  } else if ((index == mlc::arith::checked_sub(arms.length(), 1)))   {
    auto fail_block_step = mir_lower_allocate_block_index(state);
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [arm_state] = ok; return [&]() {
auto fail_state = mir_lower_start_block(arm_state, fail_block_step.block_id, mlc::String("match_guard_fail", 16));
return ast::Ok<MirLowerState>{mir_lower_finish_unreachable(fail_state)};
}(); }
}, mir_lower_match_arm_body_with_binding(fail_block_step.state, scrutinee, arms[index], fail_block_step.block_id));
  } else if ((!mir_lower_pattern_needs_conditional_test(arms[index]->pattern)))   {
    auto next_test_step = mir_lower_allocate_block_index(state);
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [arm_done_state] = ok; return mir_lower_match_arms_return(mir_lower_start_block(arm_done_state, next_test_step.block_id, mlc::String("match_test", 10)), scrutinee, arms, mlc::arith::checked_add(index, 1)); }
}, mir_lower_match_arm_body_with_binding(next_test_step.state, scrutinee, arms[index], next_test_step.block_id));
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() {
auto arm_block_step = mir_lower_allocate_block_index(condition_step.state);
auto next_test_step = mir_lower_allocate_block_index(arm_block_step.state);
auto branch_state = mir_lower_finish_current_block(next_test_step.state, mir_types::MirCondJump{condition_step.operand, arm_block_step.block_id, next_test_step.block_id});
(branch_state = mir_lower_start_block(branch_state, arm_block_step.block_id, mlc::String("match_arm", 9)));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [arm_done_state] = ok; return mir_lower_match_arms_return(mir_lower_start_block(arm_done_state, next_test_step.block_id, mlc::String("match_test", 10)), scrutinee, arms, mlc::arith::checked_add(index, 1)); }
}, mir_lower_match_arm_body_with_binding(branch_state, scrutinee, arms[index], next_test_step.block_id));
}(); }
}, mir_lower_pattern_matches_operand(state, scrutinee, arms[index]->pattern));
  }
}
std::shared_ptr<semantic_ir::SemanticMatchArm> mir_lower_match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept{
  return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{pattern, arm->has_guard, arm->when_condition, arm->body});
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_split_or_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> {
auto __match_subject = arm->pattern;
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; return alternatives.map([=](std::shared_ptr<ast::Pattern> alternative) mutable { return mir_lower_match_arm_with_pattern(arm, alternative); });
}
return mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{arm};
std::abort();
}();
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{
  return arms.flat_map([=](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return mir_lower_split_or_arm(arm); });
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_match_return(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [subject_step] = ok; return mir_lower_match_arms_return(subject_step.state, subject_step.operand, mir_lower_expand_or_arms(arms), 0); }
}, mir_lower_operand_from_expression(state, subject));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_lambda_parameters_from_index(MirLowerState state, mlc::Array<mlc::String> parameter_names, mlc::Array<mir_types::MirOperand> argument_operands, int index, std::shared_ptr<semantic_ir::SemanticExpression> body, mir_types::LocalId result_local_id) noexcept{
  if ((index >= parameter_names.length()))   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<MirLowerState>{mir_lower_emit_assign(rvalue_step.state, result_local_id, rvalue_step.rvalue)}; }
}, mir_lower_rvalue_from_expression(state, body));
  } else   {
    auto local_step = mir_lower_allocate_local(state, parameter_names[index], mir_lower_unknown_type());
    auto bound_state = mir_lower_emit_assign(local_step.state, local_step.local_id, mir_types::MirRvalueUse{argument_operands[index]});
    (bound_state = mir_lower_bind_name(bound_state, parameter_names[index], local_step.local_id));
    return mir_lower_bind_lambda_parameters_from_index(bound_state, parameter_names, argument_operands, mlc::arith::checked_add(index, 1), body, result_local_id);
  }
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_lambda_inline_body(MirLowerState state, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body, mlc::Array<mir_types::MirOperand> argument_operands, mir_types::LocalId result_local_id) noexcept{
  if ((parameter_names.length() != argument_operands.length()))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: lambda argument count mismatch", 41)}};
  } else   {
    return mir_lower_bind_lambda_parameters_from_index(state, parameter_names, argument_operands, 0, body, result_local_id);
  }
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_expression_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [elements, __1, __2] = semanticExpressionArray; return mir_lower_array_literal_to_local(state, elements);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord = std::get<semantic_ir::SemanticExpressionRecord>((*__match_subject));
auto [type_name, field_values, __2, __3] = semanticExpressionRecord; return mir_lower_record_literal_to_local(state, type_name, field_values);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [object, field_name, __2, __3] = semanticExpressionField; return mir_lower_record_field_to_local(state, object, field_name);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [inner, __1, __2] = semanticExpressionQuestion; return mir_lower_question_to_local(state, inner);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [object, index_expression, __2, __3] = semanticExpressionIndex; return mir_lower_index_to_local(state, object, index_expression);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [object, method_name, arguments, __3, __4, __5] = semanticExpressionMethod; return mir_lower_method_to_local(state, object, method_name, arguments);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3, __4] = semanticExpressionIf; return mir_lower_if_to_local(state, condition, then_expression, else_expression);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [function, arguments, __2, __3, __4] = semanticExpressionCall; return [&]() -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> {
auto __match_subject = function;
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [parameter_names, body, __2, __3] = semanticExpressionLambda; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerCallOperandsStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(operands_step.state, mlc::String("", 0), mir_lower_unknown_type());
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [done_state] = ok; return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{done_state, local_step.local_id}}; }
}, mir_lower_lambda_inline_body(local_step.state, parameter_names, body, operands_step.operands, local_step.local_id));
}(); }
}, mir_lower_operands_from_arguments(state, arguments));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [callee_name, __1, __2] = semanticExpressionIdent; return std::visit(overloaded{[&](const ast::Ok<MirLambdaBinding>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [binding] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerCallOperandsStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(operands_step.state, mlc::String("", 0), mir_lower_unknown_type());
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [done_state] = ok; return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{done_state, local_step.local_id}}; }
}, mir_lower_lambda_inline_body(local_step.state, binding.parameter_names, binding.body, operands_step.operands, local_step.local_id));
}(); }
}, mir_lower_operands_from_arguments(state, arguments)); },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [__0] = err; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerCallOperandsStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(operands_step.state, mlc::String("", 0), mir_lower_unknown_type());
return [&]() -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> {
  if (mir_lower_is_variant_ctor_name(callee_name))   {
    return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_variant_new", 17), mir_lower_variant_new_arguments(callee_name, operands_step.operands)), local_step.local_id}};
  } else   {
    return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, callee_name, operands_step.operands), local_step.local_id}};
  }
}();
}(); }
}, mir_lower_operands_from_arguments(state, arguments)); }
}, mir_lower_lookup_lambda(state, callee_name));
}
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerCallOperandsStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mlc::String>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [callee_name] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(operands_step.state, mlc::String("", 0), mir_lower_unknown_type());
return [&]() -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> {
  if (mir_lower_is_variant_ctor_name(callee_name))   {
    return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_variant_new", 17), mir_lower_variant_new_arguments(callee_name, operands_step.operands)), local_step.local_id}};
  } else   {
    return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, callee_name, operands_step.operands), local_step.local_id}};
  }
}();
}(); }
}, mir_lower_call_target_name(function)); }
}, mir_lower_operands_from_arguments(state, arguments));
std::abort();
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return std::visit(overloaded{[&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [__0] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(rvalue_step.state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_assign(local_step.state, local_step.local_id, rvalue_step.rvalue), local_step.local_id}};
}(); }
}, mir_lower_rvalue_from_expression(state, expression)); },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [__0] = err; return [&]() -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> {
  if (mir_lower_is_variant_ctor_name(name))   {
    return [&]() {
auto local_step = mir_lower_allocate_local(state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_variant_new", 17), mir_lower_variant_new_arguments(name, mlc::Array<mir_types::MirOperand>{})), local_step.local_id}};
}();
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("mir lower: unknown identifier ", 30) + mlc::to_string(name)) + mlc::String("", 0))}};
  }
}(); }
}, mir_lower_lookup_local(state, name));
}
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(rvalue_step.state, mlc::String("", 0), mir_lower_unknown_type());
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{mir_lower_emit_assign(local_step.state, local_step.local_id, rvalue_step.rvalue), local_step.local_id}};
}(); }
}, mir_lower_rvalue_from_expression(state, expression));
std::abort();
}();
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_question_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> inner) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [result_step] = ok; return [&]() {
auto is_error_step = mir_lower_allocate_local(result_step.state, mlc::String("", 0), mir_lower_unknown_type());
auto branch_state = mir_lower_emit_call_assign(is_error_step.state, is_error_step.local_id, mlc::String("__mir_result_is_err", 19), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{result_step.local_id}});
auto error_block_step = mir_lower_allocate_block_index(branch_state);
auto ok_block_step = mir_lower_allocate_block_index(error_block_step.state);
(branch_state = mir_lower_finish_current_block(ok_block_step.state, mir_types::MirCondJump{mir_types::MirOperandLocal{is_error_step.local_id}, error_block_step.block_id, ok_block_step.block_id}));
(branch_state = mir_lower_start_block(branch_state, error_block_step.block_id, mlc::String("question_err", 12)));
(branch_state = mir_lower_finish_return(branch_state, mir_types::MirOperandLocal{result_step.local_id}));
(branch_state = mir_lower_start_block(branch_state, ok_block_step.block_id, mlc::String("question_ok", 11)));
auto ok_value_step = mir_lower_allocate_local(branch_state, mlc::String("", 0), mir_lower_unknown_type());
(branch_state = mir_lower_emit_call_assign(ok_value_step.state, ok_value_step.local_id, mlc::String("__mir_result_ok_value", 21), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{result_step.local_id}}));
return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{branch_state, ok_value_step.local_id}};
}(); }
}, mir_lower_expression_to_local(state, inner));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_question_return(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> inner) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerState>{mir_lower_finish_return(local_step.state, mir_types::MirOperandLocal{local_step.local_id})}; }
}, mir_lower_question_to_local(state, inner));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_discard_expression(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [statement_state] = ok; return mir_lower_discard_expression(statement_state, result_expression); }
}, mir_lower_statements(state, statements));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return ast::Ok<MirLowerState>{state};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*__match_subject))) {
const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple = std::get<semantic_ir::SemanticExpressionTuple>((*__match_subject));
auto [elements, __1, __2] = semanticExpressionTuple; return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
  if ((elements.length() == 0))   {
    return ast::Ok<MirLowerState>{state};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: non-empty tuple discard not supported", 48)}};
  }
}();
}
return mir_lower_statement(state, std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr{expression, semantic_ir::sexpr_span(expression)}));
std::abort();
}();
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_expression_into_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression, mir_types::LocalId local_id) noexcept{
  return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [statement_state] = ok; return mir_lower_expression_into_local(statement_state, result_expression, local_id); }
}, mir_lower_statements(state, statements));
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3, __4] = semanticExpressionIf; return mir_lower_if_into_local(state, condition, then_expression, else_expression, local_id);
}
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<MirLowerState>{mir_lower_emit_assign(rvalue_step.state, local_id, rvalue_step.rvalue)}; }
}, mir_lower_rvalue_from_expression(state, expression));
std::abort();
}();
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_if_into_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, mir_types::LocalId result_local_id) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() {
auto then_block_step = mir_lower_allocate_block_index(condition_step.state);
auto else_block_step = mir_lower_allocate_block_index(then_block_step.state);
auto continue_block_step = mir_lower_allocate_block_index(else_block_step.state);
auto branch_state = mir_lower_finish_current_block(continue_block_step.state, mir_types::MirCondJump{condition_step.operand, then_block_step.block_id, else_block_step.block_id});
(branch_state = mir_lower_start_block(branch_state, then_block_step.block_id, mlc::String("if_then", 7)));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [then_state] = ok; return [&]() {
auto after_then = mir_lower_finish_current_block(then_state, mir_types::MirJump{continue_block_step.block_id});
(after_then = mir_lower_start_block(after_then, else_block_step.block_id, mlc::String("if_else", 7)));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [else_state] = ok; return [&]() {
auto after_else = mir_lower_finish_current_block(else_state, mir_types::MirJump{continue_block_step.block_id});
return ast::Ok<MirLowerState>{mir_lower_start_block(after_else, continue_block_step.block_id, mlc::String("if_continue", 11))};
}(); }
}, mir_lower_expression_into_local(after_then, else_expression, result_local_id));
}(); }
}, mir_lower_expression_into_local(branch_state, then_expression, result_local_id));
}(); }
}, mir_lower_operand_from_expression(state, condition));
}
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_if_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept{
  auto local_step = mir_lower_allocate_local(state, mlc::String("", 0), mir_lower_unknown_type());
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> { auto [done_state] = ok; return ast::Ok<MirLowerLocalStep>{MirLowerLocalStep{done_state, local_step.local_id}}; }
}, mir_lower_if_into_local(local_step.state, condition, then_expression, else_expression, local_step.local_id));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_if_statement(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() {
auto then_block_step = mir_lower_allocate_block_index(condition_step.state);
auto else_block_step = mir_lower_allocate_block_index(then_block_step.state);
auto continue_block_step = mir_lower_allocate_block_index(else_block_step.state);
auto branch_state = mir_lower_finish_current_block(continue_block_step.state, mir_types::MirCondJump{condition_step.operand, then_block_step.block_id, else_block_step.block_id});
(branch_state = mir_lower_start_block(branch_state, then_block_step.block_id, mlc::String("if_then", 7)));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [then_state] = ok; return [&]() {
auto after_then = mir_lower_finish_current_block(then_state, mir_types::MirJump{continue_block_step.block_id});
(after_then = mir_lower_start_block(after_then, else_block_step.block_id, mlc::String("if_else", 7)));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [else_state] = ok; return [&]() {
auto after_else = mir_lower_finish_current_block(else_state, mir_types::MirJump{continue_block_step.block_id});
return ast::Ok<MirLowerState>{mir_lower_start_block(after_else, continue_block_step.block_id, mlc::String("if_continue", 11))};
}(); }
}, mir_lower_discard_expression(after_then, else_expression));
}(); }
}, mir_lower_discard_expression(branch_state, then_expression));
}(); }
}, mir_lower_operand_from_expression(state, condition));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_while_statement(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements) noexcept{
  auto loop_header_step = mir_lower_allocate_block_index(state);
  auto body_block_step = mir_lower_allocate_block_index(loop_header_step.state);
  auto exit_block_step = mir_lower_allocate_block_index(body_block_step.state);
  auto loop_state = mir_lower_finish_current_block(loop_header_step.state, mir_types::MirJump{loop_header_step.block_id});
  (loop_state = mir_lower_start_block(loop_state, loop_header_step.block_id, mlc::String("loop", 4)));
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() {
(loop_state = mir_lower_finish_current_block(condition_step.state, mir_types::MirCondJump{condition_step.operand, body_block_step.block_id, exit_block_step.block_id}));
(loop_state = mir_lower_start_block(loop_state, body_block_step.block_id, mlc::String("loop_body", 9)));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [body_state] = ok; return [&]() {
auto exit_state = mir_lower_finish_current_block(body_state, mir_types::MirJump{loop_header_step.block_id});
return ast::Ok<MirLowerState>{mir_lower_start_block(exit_state, exit_block_step.block_id, mlc::String("loop_exit", 9))};
}(); }
}, mir_lower_statements(loop_state, body_statements));
}(); }
}, mir_lower_operand_from_expression(loop_state, condition));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_for_statement(MirLowerState state, mlc::String variable_name, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [iterator_step] = ok; return [&]() {
auto index_step = mir_lower_allocate_local(iterator_step.state, mlc::String("", 0), mir_lower_unknown_type());
auto prep_state = mir_lower_emit_assign(index_step.state, index_step.local_id, mir_types::MirRvalueUse{mir_types::MirOperandConstInt{0}});
auto loop_header_step = mir_lower_allocate_block_index(prep_state);
auto body_block_step = mir_lower_allocate_block_index(loop_header_step.state);
auto exit_block_step = mir_lower_allocate_block_index(body_block_step.state);
(prep_state = mir_lower_finish_current_block(loop_header_step.state, mir_types::MirJump{loop_header_step.block_id}));
(prep_state = mir_lower_start_block(prep_state, loop_header_step.block_id, mlc::String("for_loop", 8)));
auto length_step = mir_lower_allocate_local(prep_state, mlc::String("", 0), mir_lower_unknown_type());
(prep_state = mir_lower_emit_call_assign(length_step.state, length_step.local_id, mlc::String("__mir_length", 12), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{iterator_step.local_id}}));
auto less_op = mlc::String("<", 1);
auto compare_step = mir_lower_temp_operand_from_rvalue(prep_state, mir_types::MirRvalueBinary{less_op, mir_types::MirOperandLocal{index_step.local_id}, mir_types::MirOperandLocal{length_step.local_id}});
(prep_state = mir_lower_finish_current_block(compare_step.state, mir_types::MirCondJump{compare_step.operand, body_block_step.block_id, exit_block_step.block_id}));
(prep_state = mir_lower_start_block(prep_state, body_block_step.block_id, mlc::String("for_body", 8)));
auto element_step = mir_lower_allocate_local(prep_state, variable_name, mir_lower_unknown_type());
(prep_state = mir_lower_emit_call_assign(element_step.state, element_step.local_id, mlc::String("__mir_array_get", 15), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal{iterator_step.local_id}, mir_types::MirOperandLocal{index_step.local_id}}));
(prep_state = mir_lower_bind_name(prep_state, variable_name, element_step.local_id));
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [body_state] = ok; return [&]() {
auto add_op = mlc::String("+", 1);
auto next_index_step = mir_lower_temp_operand_from_rvalue(body_state, mir_types::MirRvalueBinary{add_op, mir_types::MirOperandLocal{index_step.local_id}, mir_types::MirOperandConstInt{1}});
auto exit_state = mir_lower_emit_assign(next_index_step.state, index_step.local_id, mir_types::MirRvalueUse{next_index_step.operand});
(exit_state = mir_lower_finish_current_block(exit_state, mir_types::MirJump{loop_header_step.block_id}));
return ast::Ok<MirLowerState>{mir_lower_start_block(exit_state, exit_block_step.block_id, mlc::String("for_exit", 8))};
}(); }
}, mir_lower_statements(prep_state, body_statements));
}(); }
}, mir_lower_expression_to_local(state, iterator));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_assign_identifier(MirLowerState state, mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> right) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [local_id] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<MirLowerState>{mir_lower_emit_assign(rvalue_step.state, local_id, rvalue_step.rvalue)}; }
}, mir_lower_rvalue_from_expression(state, right)); }
}, mir_lower_lookup_local(state, name));
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_lambda_statement(MirLowerState state, mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value) noexcept{
  return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [parameter_names, body, __2, __3] = semanticExpressionLambda; return ast::Ok<MirLowerState>{mir_lower_bind_lambda(state, name, parameter_names, body)};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: expected lambda binding", 34)}};
std::abort();
}();
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_statement(MirLowerState state, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept{
  return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = statement;
if (std::holds_alternative<semantic_ir::SemanticStatementLet>((*__match_subject))) {
const semantic_ir::SemanticStatementLet& semanticStatementLet = std::get<semantic_ir::SemanticStatementLet>((*__match_subject));
auto [name, __1, value, type_value, __4] = semanticStatementLet; return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionLambda; return mir_lower_bind_lambda_statement(state, name, value);
}
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(rvalue_step.state, name, type_value);
auto next_state = mir_lower_bind_name(local_step.state, name, local_step.local_id);
return ast::Ok<MirLowerState>{mir_lower_emit_assign(next_state, local_step.local_id, rvalue_step.rvalue)};
}(); }
}, mir_lower_rvalue_from_expression(state, value));
std::abort();
}();
}
if (std::holds_alternative<semantic_ir::SemanticStatementLetConst>((*__match_subject))) {
const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst = std::get<semantic_ir::SemanticStatementLetConst>((*__match_subject));
auto [name, value, type_value, __3] = semanticStatementLetConst; return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionLambda; return mir_lower_bind_lambda_statement(state, name, value);
}
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerRvalueStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return [&]() {
auto local_step = mir_lower_allocate_local(rvalue_step.state, name, type_value);
auto next_state = mir_lower_bind_name(local_step.state, name, local_step.local_id);
return ast::Ok<MirLowerState>{mir_lower_emit_assign(next_state, local_step.local_id, rvalue_step.rvalue)};
}(); }
}, mir_lower_rvalue_from_expression(state, value));
std::abort();
}();
}
if (std::holds_alternative<semantic_ir::SemanticStatementReturn>((*__match_subject))) {
const semantic_ir::SemanticStatementReturn& semanticStatementReturn = std::get<semantic_ir::SemanticStatementReturn>((*__match_subject));
auto [expression, __1] = semanticStatementReturn; return mir_lower_return_expression(state, expression);
}
if (std::holds_alternative<semantic_ir::SemanticStatementExpr>((*__match_subject))) {
const semantic_ir::SemanticStatementExpr& semanticStatementExpr = std::get<semantic_ir::SemanticStatementExpr>((*__match_subject));
auto [expression, __1] = semanticStatementExpr; return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*__match_subject))) {
const semantic_ir::SemanticExpressionBin& semanticExpressionBin = std::get<semantic_ir::SemanticExpressionBin>((*__match_subject));
auto [operation, left, right, __3, __4] = semanticExpressionBin; return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
  if (mir_lower_is_assign_operation(operation))   {
    return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = left;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return mir_lower_assign_identifier(state, name, right);
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: assignment target must be identifier", 47)}};
std::abort();
}();
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [operand_step] = ok; return ast::Ok<MirLowerState>{operand_step.state}; }
}, mir_lower_operand_from_expression(state, expression));
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*__match_subject))) {
const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile = std::get<semantic_ir::SemanticExpressionWhile>((*__match_subject));
auto [condition, body_statements, __2, __3] = semanticExpressionWhile; return mir_lower_while_statement(state, condition, body_statements);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*__match_subject))) {
const semantic_ir::SemanticExpressionFor& semanticExpressionFor = std::get<semantic_ir::SemanticExpressionFor>((*__match_subject));
auto [variable_name, iterator, body_statements, __3, __4] = semanticExpressionFor; return mir_lower_for_statement(state, variable_name, iterator, body_statements);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [object, method_name, arguments, __3, __4, __5] = semanticExpressionMethod; return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
  if ((method_name == mlc::String("pop", 3)))   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerState>{local_step.state}; }
}, mir_lower_method_to_local(state, object, method_name, arguments));
  } else   {
    return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
  if (mir_lower_is_mutating_method(method_name))   {
    return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
auto __match_subject = object;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [local_id] = ok; return mir_lower_mutating_method_statement(state, local_id, method_name, arguments); }
}, mir_lower_lookup_local(state, name));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: mutating method requires identifier receiver", 55)}};
std::abort();
}();
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerLocalStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<MirLowerState>{local_step.state}; }
}, mir_lower_method_to_local(state, object, method_name, arguments));
  }
}();
  }
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3, __4] = semanticExpressionIf; return mir_lower_if_statement(state, condition, then_expression, else_expression);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionBlock; return mir_lower_discard_expression(state, expression);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return ast::Ok<MirLowerState>{state};
}
if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*__match_subject))) {
const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple = std::get<semantic_ir::SemanticExpressionTuple>((*__match_subject));
auto [elements, __1, __2] = semanticExpressionTuple; return [&]() -> ast::Result<MirLowerState, mlc::Array<mlc::String>> {
  if ((elements.length() == 0))   {
    return ast::Ok<MirLowerState>{state};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: non-empty tuple statement not supported", 50)}};
  }
}();
}
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerOperandStep>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [operand_step] = ok; return ast::Ok<MirLowerState>{operand_step.state}; }
}, mir_lower_operand_from_expression(state, expression));
std::abort();
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: unsupported statement", 32)}};
std::abort();
}();
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_statements_from_index(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, int index) noexcept{
  if ((index >= statements.length()))   {
    return ast::Ok<MirLowerState>{state};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<MirLowerState, mlc::Array<mlc::String>> { auto [next_state] = ok; return mir_lower_statements_from_index(next_state, statements, mlc::arith::checked_add(index, 1)); }
}, mir_lower_statement(state, statements[index]));
  }
}
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_statements(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept{
  return mir_lower_statements_from_index(state, statements, 0);
}
mir_types::MirParamDefault mir_lower_param_default(std::shared_ptr<ast::Param> parameter) noexcept{
  if ((!parameter->has_default))   {
    return mir_types::MirParamDefaultNone{};
  } else   {
    return [&]() -> mir_types::MirParamDefault {
auto __match_subject = parameter->default_;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [value, __1] = exprInt; return mir_types::MirParamDefaultInt{value};
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [value, __1] = exprBool; return mir_types::MirParamDefaultBool{value};
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [value, __1] = exprStr; return mir_types::MirParamDefaultStr{value};
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return mir_types::MirParamDefaultUnit{};
}
return mir_types::MirParamDefaultNone{};
std::abort();
}();
  }
}
mlc::Array<mir_types::MirParam> mir_lower_build_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
  auto mir_params = mlc::Array<mir_types::MirParam>{};
  auto index = 0;
  while ((index < params.length()))   {
    mir_params.push_back(mir_types::MirParam{params[index]->name, mir_lower_unknown_type(), mir_lower_param_default(params[index])});
    (index = mlc::arith::checked_add(index, 1));
  }
  return mir_params;
}
ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> lower_semantic_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [name, __1, __2, params, return_type, body, __6, __7, __8] = semanticDeclarationFn; return [&]() {
auto state = mir_lower_bind_params(mir_lower_state_new(), params);
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<MirLowerState>& ok) -> ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> { auto [lowered_state] = ok; return ast::Ok<mir_types::MirFunction>{mir_types::MirFunction{name, mir_lower_build_params(params), lowered_state.locals, lowered_state.finished_blocks, return_type}}; }
}, mir_lower_return_expression(state, body));
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir lower: expected function declaration", 40)}};
std::abort();
}();
}

} // namespace lower_fn
