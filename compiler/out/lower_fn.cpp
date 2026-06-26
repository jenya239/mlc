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

struct MirLowerState {mlc::HashMap<mlc::String, mir_types::LocalId> name_environment;mlc::Array<mir_types::MirLocal> locals;mlc::Array<mir_types::MirBlock> finished_blocks;mir_types::BlockId current_block_id;mlc::String current_label;mlc::Array<mir_types::MirStmt> current_stmts;mir_ids::MirIdPool id_pool;};

struct MirLowerBlockStep {lower_fn::MirLowerState state;mir_types::BlockId block_id;};

struct MirLowerLocalStep {lower_fn::MirLowerState state;mir_types::LocalId local_id;};

struct MirLowerMethodOperandsStep {lower_fn::MirLowerState state;mlc::Array<mir_types::MirOperand> operands;};

struct MirLowerCallOperandsStep {lower_fn::MirLowerState state;mlc::Array<mir_types::MirOperand> operands;};

struct MirLowerOperandStep {lower_fn::MirLowerState state;mir_types::MirOperand operand;};

struct MirLowerRvalueStep {lower_fn::MirLowerState state;mir_types::MirRvalue rvalue;};

std::shared_ptr<registry::Type> mir_lower_unknown_type() noexcept;

bool mir_lower_is_assign_operation(mlc::String operation) noexcept;

lower_fn::MirLowerState mir_lower_state_new() noexcept;

lower_fn::MirLowerBlockStep mir_lower_allocate_block_index(lower_fn::MirLowerState state) noexcept;

lower_fn::MirLowerState mir_lower_finish_current_block(lower_fn::MirLowerState state, mir_types::MirTerminator terminator) noexcept;

lower_fn::MirLowerState mir_lower_start_block(lower_fn::MirLowerState state, mir_types::BlockId block_id, mlc::String label) noexcept;

lower_fn::MirLowerState mir_lower_bind_params(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

lower_fn::MirLowerLocalStep mir_lower_allocate_local(lower_fn::MirLowerState state, mlc::String name, std::shared_ptr<registry::Type> type_value) noexcept;

lower_fn::MirLowerState mir_lower_bind_name(lower_fn::MirLowerState state, mlc::String name, mir_types::LocalId local_id) noexcept;

ast::Result<mir_types::LocalId, mlc::Array<mlc::String>> mir_lower_lookup_local(lower_fn::MirLowerState state, mlc::String name) noexcept;

lower_fn::MirLowerState mir_lower_emit_assign(lower_fn::MirLowerState state, mir_types::LocalId local_id, mir_types::MirRvalue rvalue) noexcept;

lower_fn::MirLowerState mir_lower_emit_call_assign(lower_fn::MirLowerState state, mir_types::LocalId local_id, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_call_target_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_method_native_name(mlc::String method_name) noexcept;

bool mir_lower_is_mutating_method(mlc::String method_name) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_push_elements(lower_fn::MirLowerState state, mir_types::LocalId array_local_id, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, int index) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_literal_to_local(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_index_to_local(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_map_new_local(lower_fn::MirLowerState state) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_to_local(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_call_on_operand(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;

ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_method_operands(lower_fn::MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept;

ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_method_operands_with_receiver(lower_fn::MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_mutating_method_statement(lower_fn::MirLowerState state, mir_types::LocalId local_id, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;

ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_operands(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept;

ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_operands_from_arguments(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;

lower_fn::MirLowerOperandStep mir_lower_temp_operand_from_rvalue(lower_fn::MirLowerState state, mir_types::MirRvalue rvalue) noexcept;

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_int(lower_fn::MirLowerState state, mir_types::MirOperand left_operand, int right_value) noexcept;

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_bool(lower_fn::MirLowerState state, mir_types::MirOperand left_operand, bool right_value) noexcept;

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_from_expression(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> mir_lower_rvalue_from_expression(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

lower_fn::MirLowerState mir_lower_finish_return(lower_fn::MirLowerState state, mir_types::MirOperand operand) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_return_expression(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_emit_call_to_local(lower_fn::MirLowerState state, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept;

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_and(lower_fn::MirLowerState state, mir_types::MirOperand left_operand, mir_types::MirOperand right_operand) noexcept;

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_variant_is(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::String tag) noexcept;

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_ctor_subpattern_tests(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mir_types::MirOperand accumulated, int index) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpattern(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, int field_index, std::shared_ptr<ast::Pattern> pattern) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpatterns(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, int index) noexcept;

bool mir_lower_pattern_needs_conditional_test(std::shared_ptr<ast::Pattern> pattern) noexcept;

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_pattern_matches_operand(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<ast::Pattern> pattern) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arm_body_with_binding(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arms_return(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int index) noexcept;

std::shared_ptr<semantic_ir::SemanticMatchArm> mir_lower_match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_split_or_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_match_return(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_lambda_parameters_from_index(lower_fn::MirLowerState state, mlc::Array<mlc::String> parameter_names, mlc::Array<mir_types::MirOperand> argument_operands, int index, std::shared_ptr<semantic_ir::SemanticExpression> body, mir_types::LocalId result_local_id) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_lambda_inline_body(lower_fn::MirLowerState state, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body, mlc::Array<mir_types::MirOperand> argument_operands, mir_types::LocalId result_local_id) noexcept;

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_expression_to_local(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_question_return(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> inner) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_while_statement(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_assign_identifier(lower_fn::MirLowerState state, mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> right) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_statement(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_statements_from_index(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, int index) noexcept;

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_statements(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;

mlc::Array<mir_types::MirParam> mir_lower_build_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> lower_semantic_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

std::shared_ptr<registry::Type> mir_lower_unknown_type() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

bool mir_lower_is_assign_operation(mlc::String operation) noexcept{return operation == mlc::String("=");}

lower_fn::MirLowerState mir_lower_state_new() noexcept{return lower_fn::MirLowerState{mlc::HashMap<mlc::String, mir_types::LocalId>(), {}, {}, mir_types::mir_block_id(0), mlc::String("entry"), {}, mir_ids::mir_id_pool_new()};}

lower_fn::MirLowerBlockStep mir_lower_allocate_block_index(lower_fn::MirLowerState state) noexcept{
mir_ids::MirIdBlockStep block_step = mir_ids::mir_id_pool_allocate_block(state.id_pool);
return lower_fn::MirLowerBlockStep{lower_fn::MirLowerState{state.name_environment, state.locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, block_step.pool}, block_step.block_id};
}

lower_fn::MirLowerState mir_lower_finish_current_block(lower_fn::MirLowerState state, mir_types::MirTerminator terminator) noexcept{
mlc::Array<mir_types::MirBlock> finished_blocks = state.finished_blocks;
finished_blocks.push_back(mir_types::MirBlock{state.current_block_id, state.current_label, state.current_stmts, terminator});
return lower_fn::MirLowerState{state.name_environment, state.locals, finished_blocks, state.current_block_id, state.current_label, {}, state.id_pool};
}

lower_fn::MirLowerState mir_lower_start_block(lower_fn::MirLowerState state, mir_types::BlockId block_id, mlc::String label) noexcept{return lower_fn::MirLowerState{state.name_environment, state.locals, state.finished_blocks, block_id, label, {}, state.id_pool};}

lower_fn::MirLowerState mir_lower_bind_params(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::HashMap<mlc::String, mir_types::LocalId> environment = state.name_environment;
int index = 0;
while (index < params.size()){
{
environment.set(params[index]->name, mir_types::mir_local_id(index));
index = index + 1;
}
}
return lower_fn::MirLowerState{environment, state.locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, mir_ids::mir_id_pool_locals_reserved(state.id_pool, index)};
}

lower_fn::MirLowerLocalStep mir_lower_allocate_local(lower_fn::MirLowerState state, mlc::String name, std::shared_ptr<registry::Type> type_value) noexcept{
mir_ids::MirIdLocalStep local_step = mir_ids::mir_id_pool_allocate_local(state.id_pool);
mlc::Array<mir_types::MirLocal> locals = state.locals;
locals.push_back(mir_types::MirLocal{local_step.local_id, name, type_value});
return lower_fn::MirLowerLocalStep{lower_fn::MirLowerState{state.name_environment, locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, local_step.pool}, local_step.local_id};
}

lower_fn::MirLowerState mir_lower_bind_name(lower_fn::MirLowerState state, mlc::String name, mir_types::LocalId local_id) noexcept{
mlc::HashMap<mlc::String, mir_types::LocalId> environment = state.name_environment;
environment.set(name, local_id);
return lower_fn::MirLowerState{environment, state.locals, state.finished_blocks, state.current_block_id, state.current_label, state.current_stmts, state.id_pool};
}

ast::Result<mir_types::LocalId, mlc::Array<mlc::String>> mir_lower_lookup_local(lower_fn::MirLowerState state, mlc::String name) noexcept{
return state.name_environment.has(name) ? ast::Result<mir_types::LocalId, mlc::Array<mlc::String>>(ast::Ok<mir_types::LocalId>(state.name_environment.get(name))) : ast::Result<mir_types::LocalId, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unknown local ") + name}));
}

lower_fn::MirLowerState mir_lower_emit_assign(lower_fn::MirLowerState state, mir_types::LocalId local_id, mir_types::MirRvalue rvalue) noexcept{
mlc::Array<mir_types::MirStmt> statements = state.current_stmts;
statements.push_back(mir_types::MirAssign(local_id, rvalue));
return lower_fn::MirLowerState{state.name_environment, state.locals, state.finished_blocks, state.current_block_id, state.current_label, statements, state.id_pool};
}

lower_fn::MirLowerState mir_lower_emit_call_assign(lower_fn::MirLowerState state, mir_types::LocalId local_id, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept{
mlc::Array<mir_types::MirStmt> statements = state.current_stmts;
statements.push_back(mir_types::MirCallAssign(local_id, callee_name, arguments));
return lower_fn::MirLowerState{state.name_environment, state.locals, state.finished_blocks, state.current_block_id, state.current_label, statements, state.id_pool};
}

ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_call_target_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return ast::Ok<mlc::String>(name); } if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*expression)._)) { auto _v_semanticexpressionfield = std::get<semantic_ir::SemanticExpressionField>((*expression)._); auto [receiver, field_name, _w0, _w1] = _v_semanticexpressionfield; return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*receiver)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*receiver)._); auto [type_name, _w0, _w1] = _v_semanticexpressionident; return type_name == mlc::String("Map") && field_name == mlc::String("new") ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(mlc::String("__mir_map_empty"))) : ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unsupported static field call")})); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: call target must be identifier")}); }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: call target must be identifier")}); }();}

ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_method_native_name(mlc::String method_name) noexcept{return method_name == mlc::String("push") ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(mlc::String("__mir_array_push"))) : ast::Result<mlc::String, mlc::Array<mlc::String>>(method_name == mlc::String("get") ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(mlc::String("__mir_map_get"))) : ast::Result<mlc::String, mlc::Array<mlc::String>>(method_name == mlc::String("set") ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(mlc::String("__mir_map_set"))) : ast::Result<mlc::String, mlc::Array<mlc::String>>(method_name == mlc::String("length") ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(mlc::String("__mir_length"))) : ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unsupported method ") + method_name})))));}

bool mir_lower_is_mutating_method(mlc::String method_name) noexcept{return method_name == mlc::String("push") || method_name == mlc::String("set");}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_push_elements(lower_fn::MirLowerState state, mir_types::LocalId array_local_id, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, int index) noexcept{return index >= elements.size() ? ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>>(ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{state, array_local_id})) : ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [element_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep push_result_step = mir_lower_allocate_local(element_step.state, mlc::String(""), mir_lower_unknown_type());
  lower_fn::MirLowerState push_state = mir_lower_emit_call_assign(push_result_step.state, push_result_step.local_id, mlc::String("__mir_array_push"), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal(array_local_id), element_step.operand});
  return mir_lower_array_push_elements(push_state, push_result_step.local_id, elements, index + 1);
 }(); }
}, mir_lower_operand_from_expression(state, elements[index])));}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_literal_to_local(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements) noexcept{
lower_fn::MirLowerLocalStep empty_step = mir_lower_allocate_local(state, mlc::String(""), mir_lower_unknown_type());
lower_fn::MirLowerState empty_state = mir_lower_emit_call_assign(empty_step.state, empty_step.local_id, mlc::String("__mir_array_empty"), {});
return mir_lower_array_push_elements(empty_state, empty_step.local_id, elements, 0);
}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_index_to_local(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [object_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [index_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(index_step.state, mlc::String(""), mir_lower_unknown_type());
  return ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_array_get"), mlc::Array<mir_types::MirOperand>{object_step.operand, index_step.operand}), local_step.local_id});
 }(); }
}, mir_lower_operand_from_expression(object_step.state, index_expression)); }
}, mir_lower_operand_from_expression(state, object));}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_map_new_local(lower_fn::MirLowerState state) noexcept{
lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(state, mlc::String(""), mir_lower_unknown_type());
return ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, mlc::String("__mir_map_empty"), {}), local_step.local_id});
}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_to_local(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*object)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*object)._); auto [type_name, _w0, _w1] = _v_semanticexpressionident; return type_name == mlc::String("Map") && method_name == mlc::String("new") && arguments.size() == 0 ? ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>>(mir_lower_map_new_local(state)) : ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>>(mir_lower_method_call_on_operand(state, object, method_name, arguments)); } return mir_lower_method_call_on_operand(state, object, method_name, arguments); }();}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_call_on_operand(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mlc::String>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [native_name] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [object_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerMethodOperandsStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(operands_step.state, mlc::String(""), mir_lower_unknown_type());
  return ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, native_name, operands_step.operands), local_step.local_id});
 }(); }
}, mir_lower_method_operands_with_receiver(object_step.state, object_step.operand, arguments)); }
}, mir_lower_operand_from_expression(state, object)); }
}, mir_lower_method_native_name(method_name));}

ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_method_operands(lower_fn::MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept{return index >= arguments.size() ? ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>>(ast::Ok<lower_fn::MirLowerMethodOperandsStep>(lower_fn::MirLowerMethodOperandsStep{state, operands})) : ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>> { auto [argument_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>> { 
  mlc::Array<mir_types::MirOperand> next_operands = operands;
  next_operands.push_back(argument_step.operand);
  return mir_lower_collect_method_operands(argument_step.state, object_operand, arguments, index + 1, next_operands);
 }(); }
}, mir_lower_operand_from_expression(state, arguments[index])));}

ast::Result<lower_fn::MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_method_operands_with_receiver(lower_fn::MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{
mlc::Array<mir_types::MirOperand> operands = mlc::Array<mir_types::MirOperand>{object_operand};
return mir_lower_collect_method_operands(state, object_operand, arguments, 0, operands);
}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_mutating_method_statement(lower_fn::MirLowerState state, mir_types::LocalId local_id, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mlc::String>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [native_name] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerMethodOperandsStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep result_step = mir_lower_allocate_local(operands_step.state, mlc::String(""), mir_lower_unknown_type());
  lower_fn::MirLowerState call_state = mir_lower_emit_call_assign(result_step.state, result_step.local_id, native_name, operands_step.operands);
  return ast::Ok<lower_fn::MirLowerState>(mir_lower_emit_assign(call_state, local_id, mir_types::MirRvalueUse(mir_types::MirOperandLocal(result_step.local_id))));
 }(); }
}, mir_lower_method_operands_with_receiver(state, mir_types::MirOperandLocal(local_id), arguments)); }
}, mir_lower_method_native_name(method_name));}

ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_operands(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept{return index >= arguments.size() ? ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>>(ast::Ok<lower_fn::MirLowerCallOperandsStep>(lower_fn::MirLowerCallOperandsStep{state, operands})) : ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>> { auto [step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>> { 
  mlc::Array<mir_types::MirOperand> next_operands = operands;
  next_operands.push_back(step.operand);
  return mir_lower_collect_operands(step.state, arguments, index + 1, next_operands);
 }(); }
}, mir_lower_operand_from_expression(state, arguments[index])));}

ast::Result<lower_fn::MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_operands_from_arguments(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept{return mir_lower_collect_operands(state, arguments, 0, {});}

lower_fn::MirLowerOperandStep mir_lower_temp_operand_from_rvalue(lower_fn::MirLowerState state, mir_types::MirRvalue rvalue) noexcept{
lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(state, mlc::String(""), mir_lower_unknown_type());
lower_fn::MirLowerState assigned_state = mir_lower_emit_assign(local_step.state, local_step.local_id, rvalue);
return lower_fn::MirLowerOperandStep{assigned_state, mir_types::MirOperandLocal(local_step.local_id)};
}

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_int(lower_fn::MirLowerState state, mir_types::MirOperand left_operand, int right_value) noexcept{return ast::Ok<lower_fn::MirLowerOperandStep>(mir_lower_temp_operand_from_rvalue(state, mir_types::MirRvalueBinary(mlc::String("=="), left_operand, mir_types::MirOperandConstInt(right_value))));}

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_bool(lower_fn::MirLowerState state, mir_types::MirOperand left_operand, bool right_value) noexcept{return ast::Ok<lower_fn::MirLowerOperandStep>(mir_lower_temp_operand_from_rvalue(state, mir_types::MirRvalueBinary(mlc::String("=="), left_operand, mir_types::MirOperandConstBool(right_value))));}

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_from_expression(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
return [&]() -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*expression)._)) { auto _v_semanticexpressionint = std::get<semantic_ir::SemanticExpressionInt>((*expression)._); auto [value, _w0, _w1] = _v_semanticexpressionint; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{state, mir_types::MirOperandConstInt(value)}); } if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*expression)._)) { auto _v_semanticexpressionbool = std::get<semantic_ir::SemanticExpressionBool>((*expression)._); auto [value, _w0, _w1] = _v_semanticexpressionbool; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{state, mir_types::MirOperandConstBool(value)}); } if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*expression)._)) { auto _v_semanticexpressionstr = std::get<semantic_ir::SemanticExpressionStr>((*expression)._); auto [value, _w0, _w1] = _v_semanticexpressionstr; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{state, mir_types::MirOperandConstStr(value)}); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{state, (mir_types::MirOperandUnit{})}); } if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return std::visit(overloaded{
  [&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_id] = ok; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{state, mir_types::MirOperandLocal(local_id)}); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); }
}, mir_lower_lookup_local(state, name)); } if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, _w0, _w1, _w2, _w3] = _v_semanticexpressionbin; return mir_lower_is_assign_operation(operation) ? ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: assignment expression is not an operand")})) : ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerRvalueStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<lower_fn::MirLowerOperandStep>(mir_lower_temp_operand_from_rvalue(rvalue_step.state, rvalue_step.rvalue)); }
}, mir_lower_rvalue_from_expression(state, expression))); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [function, arguments, _w0, _w1, _w2] = _v_semanticexpressioncall; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal(local_step.local_id)}); }
}, mir_lower_expression_to_local(state, expression)); } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [_w0, _w1, _w2] = _v_semanticexpressionarray; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal(local_step.local_id)}); }
}, mir_lower_expression_to_local(state, expression)); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [_w0, _w1, _w2, _w3] = _v_semanticexpressionindex; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal(local_step.local_id)}); }
}, mir_lower_expression_to_local(state, expression)); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [_w0, _w1, _w2, _w3, _w4, _w5] = _v_semanticexpressionmethod; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal(local_step.local_id)}); }
}, mir_lower_expression_to_local(state, expression)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unsupported expression in operand context")}); }();
}

ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> mir_lower_rvalue_from_expression(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
return [&]() -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*expression)._)) { auto _v_semanticexpressionint = std::get<semantic_ir::SemanticExpressionInt>((*expression)._); auto [value, _w0, _w1] = _v_semanticexpressionint; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{state, mir_types::MirRvalueUse(mir_types::MirOperandConstInt(value))}); } if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*expression)._)) { auto _v_semanticexpressionbool = std::get<semantic_ir::SemanticExpressionBool>((*expression)._); auto [value, _w0, _w1] = _v_semanticexpressionbool; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{state, mir_types::MirRvalueUse(mir_types::MirOperandConstBool(value))}); } if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*expression)._)) { auto _v_semanticexpressionstr = std::get<semantic_ir::SemanticExpressionStr>((*expression)._); auto [value, _w0, _w1] = _v_semanticexpressionstr; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{state, mir_types::MirRvalueUse(mir_types::MirOperandConstStr(value))}); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{state, mir_types::MirRvalueUse((mir_types::MirOperandUnit{}))}); } if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return std::visit(overloaded{
  [&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_id] = ok; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{state, mir_types::MirRvalueUse(mir_types::MirOperandLocal(local_id))}); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); }
}, mir_lower_lookup_local(state, name)); } if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, left, right, _w0, _w1] = _v_semanticexpressionbin; return mir_lower_is_assign_operation(operation) ? ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: assignment is not an rvalue")})) : ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [left_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [right_step] = ok; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{right_step.state, mir_types::MirRvalueBinary(operation, left_step.operand, right_step.operand)}); }
}, mir_lower_operand_from_expression(left_step.state, right)); }
}, mir_lower_operand_from_expression(state, left))); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [function, arguments, _w0, _w1, _w2] = _v_semanticexpressioncall; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse(mir_types::MirOperandLocal(local_step.local_id))}); }
}, mir_lower_expression_to_local(state, expression)); } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [_w0, _w1, _w2] = _v_semanticexpressionarray; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse(mir_types::MirOperandLocal(local_step.local_id))}); }
}, mir_lower_expression_to_local(state, expression)); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [_w0, _w1, _w2, _w3] = _v_semanticexpressionindex; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse(mir_types::MirOperandLocal(local_step.local_id))}); }
}, mir_lower_expression_to_local(state, expression)); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [_w0, _w1, _w2, _w3, _w4, _w5] = _v_semanticexpressionmethod; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerRvalueStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerRvalueStep>(lower_fn::MirLowerRvalueStep{local_step.state, mir_types::MirRvalueUse(mir_types::MirOperandLocal(local_step.local_id))}); }
}, mir_lower_expression_to_local(state, expression)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unsupported expression in rvalue context")}); }();
}

lower_fn::MirLowerState mir_lower_finish_return(lower_fn::MirLowerState state, mir_types::MirOperand operand) noexcept{return mir_lower_finish_current_block(state, mir_types::MirReturn(operand));}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_return_expression(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [statement_state] = ok; return mir_lower_return_expression(statement_state, result_expression); }
}, mir_lower_statements(state, statements)); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, _w0, _w1] = _v_semanticexpressionif; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerBlockStep then_block_step = mir_lower_allocate_block_index(condition_step.state);
  lower_fn::MirLowerBlockStep else_block_step = mir_lower_allocate_block_index(then_block_step.state);
  lower_fn::MirLowerState branch_state = mir_lower_finish_current_block(then_block_step.state, mir_types::MirCondJump(condition_step.operand, then_block_step.block_id, else_block_step.block_id));
  branch_state = mir_lower_start_block(branch_state, then_block_step.block_id, mlc::String("then"));
  return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [then_state] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerState else_state = mir_lower_start_block(then_state, else_block_step.block_id, mlc::String("else"));
  return mir_lower_return_expression(else_state, else_expression);
 }(); }
}, mir_lower_return_expression(branch_state, then_expression));
 }(); }
}, mir_lower_operand_from_expression(state, condition)); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return ast::Ok<lower_fn::MirLowerState>(mir_lower_finish_return(state, (mir_types::MirOperandUnit{}))); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*expression)._); auto [subject, arms, _w0, _w1] = _v_semanticexpressionmatch; return mir_lower_match_return(state, subject, arms); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner, _w0, _w1] = _v_semanticexpressionquestion; return mir_lower_question_return(state, inner); } return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [operand_step] = ok; return ast::Ok<lower_fn::MirLowerState>(mir_lower_finish_return(operand_step.state, operand_step.operand)); }
}, mir_lower_operand_from_expression(state, expression)); }();
}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_emit_call_to_local(lower_fn::MirLowerState state, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept{
lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(state, mlc::String(""), mir_lower_unknown_type());
return ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, callee_name, arguments), local_step.local_id});
}

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_and(lower_fn::MirLowerState state, mir_types::MirOperand left_operand, mir_types::MirOperand right_operand) noexcept{return ast::Ok<lower_fn::MirLowerOperandStep>(mir_lower_temp_operand_from_rvalue(state, mir_types::MirRvalueBinary(mlc::String("&&"), left_operand, right_operand)));}

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_variant_is(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::String tag) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{local_step.state, mir_types::MirOperandLocal(local_step.local_id)}); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_is"), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstStr(tag)}));}

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_ctor_subpattern_tests(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mir_types::MirOperand accumulated, int index) noexcept{return index >= sub_patterns.size() ? ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>>(ast::Ok<lower_fn::MirLowerOperandStep>(lower_fn::MirLowerOperandStep{state, accumulated})) : ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>>([&]() -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { if (std::holds_alternative<ast::PatternInt>((*sub_patterns[index]))) { auto _v_patternint = std::get<ast::PatternInt>((*sub_patterns[index])); auto [value, _w0] = _v_patternint; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [field_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [eq_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [and_step] = ok; return mir_lower_ctor_subpattern_tests(and_step.state, scrutinee, sub_patterns, and_step.operand, index + 1); }
}, mir_lower_operand_and(eq_step.state, accumulated, eq_step.operand)); }
}, mir_lower_operand_equals_const_int(field_step.state, mir_types::MirOperandLocal(field_step.local_id), value)); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field"), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt(index)})); } if (std::holds_alternative<ast::PatternBool>((*sub_patterns[index]))) { auto _v_patternbool = std::get<ast::PatternBool>((*sub_patterns[index])); auto [value, _w0] = _v_patternbool; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [field_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [eq_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [and_step] = ok; return mir_lower_ctor_subpattern_tests(and_step.state, scrutinee, sub_patterns, and_step.operand, index + 1); }
}, mir_lower_operand_and(eq_step.state, accumulated, eq_step.operand)); }
}, mir_lower_operand_equals_const_bool(field_step.state, mir_types::MirOperandLocal(field_step.local_id), value)); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field"), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt(index)})); } if (std::holds_alternative<ast::PatternWild>((*sub_patterns[index]))) { auto _v_patternwild = std::get<ast::PatternWild>((*sub_patterns[index])); auto [_w0] = _v_patternwild; return mir_lower_ctor_subpattern_tests(state, scrutinee, sub_patterns, accumulated, index + 1); } else if (std::holds_alternative<ast::PatternIdent>((*sub_patterns[index]))) { auto _v_patternident = std::get<ast::PatternIdent>((*sub_patterns[index])); return mir_lower_ctor_subpattern_tests(state, scrutinee, sub_patterns, accumulated, index + 1); } else if (std::holds_alternative<ast::PatternUnit>((*sub_patterns[index]))) { auto _v_patternunit = std::get<ast::PatternUnit>((*sub_patterns[index])); return mir_lower_ctor_subpattern_tests(state, scrutinee, sub_patterns, accumulated, index + 1); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unsupported variant subpattern in match test")}); }());}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpattern(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, int field_index, std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { if (std::holds_alternative<ast::PatternWild>((*pattern))) { auto _v_patternwild = std::get<ast::PatternWild>((*pattern)); auto [_w0] = _v_patternwild; return ast::Ok<lower_fn::MirLowerState>(state); } else if (std::holds_alternative<ast::PatternUnit>((*pattern))) { auto _v_patternunit = std::get<ast::PatternUnit>((*pattern)); return ast::Ok<lower_fn::MirLowerState>(state); } else if (std::holds_alternative<ast::PatternInt>((*pattern))) { auto _v_patternint = std::get<ast::PatternInt>((*pattern)); return ast::Ok<lower_fn::MirLowerState>(state); } else if (std::holds_alternative<ast::PatternBool>((*pattern))) { auto _v_patternbool = std::get<ast::PatternBool>((*pattern)); return ast::Ok<lower_fn::MirLowerState>(state); } if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [name, _w0] = _v_patternident; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [field_step] = ok; return ast::Ok<lower_fn::MirLowerState>(mir_lower_bind_name(field_step.state, name, field_step.local_id)); }
}, mir_lower_emit_call_to_local(state, mlc::String("__mir_variant_field"), mlc::Array<mir_types::MirOperand>{scrutinee, mir_types::MirOperandConstInt(field_index)})); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unsupported variant subpattern binding")}); }();}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpatterns(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, int index) noexcept{return index >= sub_patterns.size() ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(ast::Ok<lower_fn::MirLowerState>(state)) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [bound_state] = ok; return mir_lower_bind_ctor_subpatterns(bound_state, scrutinee, sub_patterns, index + 1); }
}, mir_lower_bind_ctor_subpattern(state, scrutinee, index, sub_patterns[index])));}

bool mir_lower_pattern_needs_conditional_test(std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatternInt>((*pattern))) { auto _v_patternint = std::get<ast::PatternInt>((*pattern)); auto [_w0, _w1] = _v_patternint; return true; } if (std::holds_alternative<ast::PatternBool>((*pattern))) { auto _v_patternbool = std::get<ast::PatternBool>((*pattern)); auto [_w0, _w1] = _v_patternbool; return true; } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [_w0, _w1, _w2] = _v_patternctor; return true; } return false; }();}

ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_pattern_matches_operand(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<ast::Pattern> pattern) noexcept{return [&]() -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { if (std::holds_alternative<ast::PatternInt>((*pattern))) { auto _v_patternint = std::get<ast::PatternInt>((*pattern)); auto [value, _w0] = _v_patternint; return mir_lower_operand_equals_const_int(state, scrutinee, value); } if (std::holds_alternative<ast::PatternBool>((*pattern))) { auto _v_patternbool = std::get<ast::PatternBool>((*pattern)); auto [value, _w0] = _v_patternbool; return mir_lower_operand_equals_const_bool(state, scrutinee, value); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [tag, sub_patterns, _w0] = _v_patternctor; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerOperandStep, mlc::Array<mlc::String>> { auto [is_step] = ok; return mir_lower_ctor_subpattern_tests(is_step.state, scrutinee, sub_patterns, is_step.operand, 0); }
}, mir_lower_operand_variant_is(state, scrutinee, tag)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: staged match supports int, bool, and variant ctor patterns")}); }();}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arm_body_with_binding(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept{return arm->has_guard ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: match guards not supported in staged lowering")})) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>([&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { if (std::holds_alternative<ast::PatternIdent>((*arm->pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*arm->pattern)); auto [name, _w0] = _v_patternident; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(state, name, mir_lower_unknown_type());
  lower_fn::MirLowerState bound_state = mir_lower_emit_assign(local_step.state, local_step.local_id, mir_types::MirRvalueUse(scrutinee));
  bound_state = mir_lower_bind_name(bound_state, name, local_step.local_id);
  return mir_lower_return_expression(bound_state, arm->body);
 }(); } if (std::holds_alternative<ast::PatternCtor>((*arm->pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*arm->pattern)); auto [_w0, sub_patterns, _w1] = _v_patternctor; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [bound_state] = ok; return mir_lower_return_expression(bound_state, arm->body); }
}, mir_lower_bind_ctor_subpatterns(state, scrutinee, sub_patterns, 0)); } return mir_lower_return_expression(state, arm->body); }());}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arms_return(lower_fn::MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int index) noexcept{return index >= arms.size() ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: empty match")})) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(index == arms.size() - 1 ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(mir_lower_match_arm_body_with_binding(state, scrutinee, arms[index])) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(!mir_lower_pattern_needs_conditional_test(arms[index]->pattern) ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(mir_lower_match_arm_body_with_binding(state, scrutinee, arms[index])) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerBlockStep arm_block_step = mir_lower_allocate_block_index(condition_step.state);
  lower_fn::MirLowerBlockStep next_test_step = mir_lower_allocate_block_index(arm_block_step.state);
  lower_fn::MirLowerState branch_state = mir_lower_finish_current_block(next_test_step.state, mir_types::MirCondJump(condition_step.operand, arm_block_step.block_id, next_test_step.block_id));
  branch_state = mir_lower_start_block(branch_state, arm_block_step.block_id, mlc::String("match_arm"));
  return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [arm_done_state] = ok; return mir_lower_match_arms_return(mir_lower_start_block(arm_done_state, next_test_step.block_id, mlc::String("match_test")), scrutinee, arms, index + 1); }
}, mir_lower_match_arm_body_with_binding(branch_state, scrutinee, arms[index]));
 }(); }
}, mir_lower_pattern_matches_operand(state, scrutinee, arms[index]->pattern)))));}

std::shared_ptr<semantic_ir::SemanticMatchArm> mir_lower_match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept{return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{pattern, arm->has_guard, arm->when_condition, arm->body});}

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_split_or_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept{return [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> { if (std::holds_alternative<ast::PatternOr>((*arm->pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*arm->pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.map([arm](std::shared_ptr<ast::Pattern> alternative) mutable { return mir_lower_match_arm_with_pattern(arm, alternative); }); } return mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{arm}; }();}

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{return mlc::collections::flat_map(arms, [](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return mir_lower_split_or_arm(arm); });}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_match_return(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [subject_step] = ok; return mir_lower_match_arms_return(subject_step.state, subject_step.operand, mir_lower_expand_or_arms(arms), 0); }
}, mir_lower_operand_from_expression(state, subject));}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_lambda_parameters_from_index(lower_fn::MirLowerState state, mlc::Array<mlc::String> parameter_names, mlc::Array<mir_types::MirOperand> argument_operands, int index, std::shared_ptr<semantic_ir::SemanticExpression> body, mir_types::LocalId result_local_id) noexcept{return index >= parameter_names.size() ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerRvalueStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<lower_fn::MirLowerState>(mir_lower_emit_assign(rvalue_step.state, result_local_id, rvalue_step.rvalue)); }
}, mir_lower_rvalue_from_expression(state, body))) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>([&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(state, parameter_names[index], mir_lower_unknown_type());
  lower_fn::MirLowerState bound_state = mir_lower_emit_assign(local_step.state, local_step.local_id, mir_types::MirRvalueUse(argument_operands[index]));
  bound_state = mir_lower_bind_name(bound_state, parameter_names[index], local_step.local_id);
  return mir_lower_bind_lambda_parameters_from_index(bound_state, parameter_names, argument_operands, index + 1, body, result_local_id);
 }());}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_lambda_inline_body(lower_fn::MirLowerState state, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body, mlc::Array<mir_types::MirOperand> argument_operands, mir_types::LocalId result_local_id) noexcept{return parameter_names.size() != argument_operands.size() ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: lambda argument count mismatch")})) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(mir_lower_bind_lambda_parameters_from_index(state, parameter_names, argument_operands, 0, body, result_local_id));}

ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_expression_to_local(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [elements, _w0, _w1] = _v_semanticexpressionarray; return mir_lower_array_literal_to_local(state, elements); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [object, index_expression, _w0, _w1] = _v_semanticexpressionindex; return mir_lower_index_to_local(state, object, index_expression); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [object, method_name, arguments, _w0, _w1, _w2] = _v_semanticexpressionmethod; return mir_lower_method_to_local(state, object, method_name, arguments); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [function, arguments, _w0, _w1, _w2] = _v_semanticexpressioncall; return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*function)._)) { auto _v_semanticexpressionlambda = std::get<semantic_ir::SemanticExpressionLambda>((*function)._); auto [parameter_names, body, _w0, _w1] = _v_semanticexpressionlambda; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerCallOperandsStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(operands_step.state, mlc::String(""), mir_lower_unknown_type());
  return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [done_state] = ok; return ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{done_state, local_step.local_id}); }
}, mir_lower_lambda_inline_body(local_step.state, parameter_names, body, operands_step.operands, local_step.local_id));
 }(); }
}, mir_lower_operands_from_arguments(state, arguments)); } return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerCallOperandsStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [operands_step] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mlc::String>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [callee_name] = ok; return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(operands_step.state, mlc::String(""), mir_lower_unknown_type());
  return ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{mir_lower_emit_call_assign(local_step.state, local_step.local_id, callee_name, operands_step.operands), local_step.local_id});
 }(); }
}, mir_lower_call_target_name(function)); }
}, mir_lower_operands_from_arguments(state, arguments)); }(); } return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerRvalueStep>& ok) -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerLocalStep, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(rvalue_step.state, mlc::String(""), mir_lower_unknown_type());
  return ast::Ok<lower_fn::MirLowerLocalStep>(lower_fn::MirLowerLocalStep{mir_lower_emit_assign(local_step.state, local_step.local_id, rvalue_step.rvalue), local_step.local_id});
 }(); }
}, mir_lower_rvalue_from_expression(state, expression)); }();}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_question_return(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> inner) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [result_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep is_error_step = mir_lower_allocate_local(result_step.state, mlc::String(""), mir_lower_unknown_type());
  lower_fn::MirLowerState branch_state = mir_lower_emit_call_assign(is_error_step.state, is_error_step.local_id, mlc::String("__mir_result_is_err"), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal(result_step.local_id)});
  lower_fn::MirLowerBlockStep error_block_step = mir_lower_allocate_block_index(branch_state);
  lower_fn::MirLowerBlockStep ok_block_step = mir_lower_allocate_block_index(error_block_step.state);
  branch_state = mir_lower_finish_current_block(ok_block_step.state, mir_types::MirCondJump(mir_types::MirOperandLocal(is_error_step.local_id), error_block_step.block_id, ok_block_step.block_id));
  branch_state = mir_lower_start_block(branch_state, error_block_step.block_id, mlc::String("question_err"));
  lower_fn::MirLowerLocalStep error_value_step = mir_lower_allocate_local(branch_state, mlc::String(""), mir_lower_unknown_type());
  branch_state = mir_lower_emit_call_assign(error_value_step.state, error_value_step.local_id, mlc::String("__mir_result_err_value"), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal(result_step.local_id)});
  branch_state = mir_lower_finish_return(branch_state, mir_types::MirOperandLocal(error_value_step.local_id));
  branch_state = mir_lower_start_block(branch_state, ok_block_step.block_id, mlc::String("question_ok"));
  lower_fn::MirLowerLocalStep ok_value_step = mir_lower_allocate_local(branch_state, mlc::String(""), mir_lower_unknown_type());
  branch_state = mir_lower_emit_call_assign(ok_value_step.state, ok_value_step.local_id, mlc::String("__mir_result_ok_value"), mlc::Array<mir_types::MirOperand>{mir_types::MirOperandLocal(result_step.local_id)});
  return ast::Ok<lower_fn::MirLowerState>(mir_lower_finish_return(branch_state, mir_types::MirOperandLocal(ok_value_step.local_id)));
 }(); }
}, mir_lower_expression_to_local(state, inner));}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_while_statement(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements) noexcept{
lower_fn::MirLowerBlockStep loop_header_step = mir_lower_allocate_block_index(state);
lower_fn::MirLowerBlockStep body_block_step = mir_lower_allocate_block_index(loop_header_step.state);
lower_fn::MirLowerBlockStep exit_block_step = mir_lower_allocate_block_index(body_block_step.state);
lower_fn::MirLowerState loop_state = mir_lower_finish_current_block(loop_header_step.state, mir_types::MirJump(loop_header_step.block_id));
loop_state = mir_lower_start_block(loop_state, loop_header_step.block_id, mlc::String("loop"));
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [condition_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  loop_state = mir_lower_finish_current_block(condition_step.state, mir_types::MirCondJump(condition_step.operand, body_block_step.block_id, exit_block_step.block_id));
  loop_state = mir_lower_start_block(loop_state, body_block_step.block_id, mlc::String("loop_body"));
  return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [body_state] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerState exit_state = mir_lower_finish_current_block(body_state, mir_types::MirJump(loop_header_step.block_id));
  return ast::Ok<lower_fn::MirLowerState>(mir_lower_start_block(exit_state, exit_block_step.block_id, mlc::String("loop_exit")));
 }(); }
}, mir_lower_statements(loop_state, body_statements));
 }(); }
}, mir_lower_operand_from_expression(loop_state, condition));
}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_assign_identifier(lower_fn::MirLowerState state, mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> right) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [local_id] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerRvalueStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return ast::Ok<lower_fn::MirLowerState>(mir_lower_emit_assign(rvalue_step.state, local_id, rvalue_step.rvalue)); }
}, mir_lower_rvalue_from_expression(state, right)); }
}, mir_lower_lookup_local(state, name));}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_statement(lower_fn::MirLowerState state, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept{
return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticStatementLet>((*statement)._)) { auto _v_semanticstatementlet = std::get<semantic_ir::SemanticStatementLet>((*statement)._); auto [name, _w0, value, type_value, _w1] = _v_semanticstatementlet; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerRvalueStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(rvalue_step.state, name, type_value);
  lower_fn::MirLowerState next_state = mir_lower_bind_name(local_step.state, name, local_step.local_id);
  return ast::Ok<lower_fn::MirLowerState>(mir_lower_emit_assign(next_state, local_step.local_id, rvalue_step.rvalue));
 }(); }
}, mir_lower_rvalue_from_expression(state, value)); } if (std::holds_alternative<semantic_ir::SemanticStatementLetConst>((*statement)._)) { auto _v_semanticstatementletconst = std::get<semantic_ir::SemanticStatementLetConst>((*statement)._); auto [name, value, type_value, _w0] = _v_semanticstatementletconst; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerRvalueStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [rvalue_step] = ok; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerLocalStep local_step = mir_lower_allocate_local(rvalue_step.state, name, type_value);
  lower_fn::MirLowerState next_state = mir_lower_bind_name(local_step.state, name, local_step.local_id);
  return ast::Ok<lower_fn::MirLowerState>(mir_lower_emit_assign(next_state, local_step.local_id, rvalue_step.rvalue));
 }(); }
}, mir_lower_rvalue_from_expression(state, value)); } if (std::holds_alternative<semantic_ir::SemanticStatementReturn>((*statement)._)) { auto _v_semanticstatementreturn = std::get<semantic_ir::SemanticStatementReturn>((*statement)._); auto [expression, _w0] = _v_semanticstatementreturn; return mir_lower_return_expression(state, expression); } if (std::holds_alternative<semantic_ir::SemanticStatementExpr>((*statement)._)) { auto _v_semanticstatementexpr = std::get<semantic_ir::SemanticStatementExpr>((*statement)._); auto [expression, _w0] = _v_semanticstatementexpr; return [&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, left, right, _w0, _w1] = _v_semanticexpressionbin; return mir_lower_is_assign_operation(operation) ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>([&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*left)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*left)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return mir_lower_assign_identifier(state, name, right); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: assignment target must be identifier")}); }()) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [operand_step] = ok; return ast::Ok<lower_fn::MirLowerState>(operand_step.state); }
}, mir_lower_operand_from_expression(state, expression))); } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, body_statements, _w0, _w1] = _v_semanticexpressionwhile; return mir_lower_while_statement(state, condition, body_statements); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [object, method_name, arguments, _w0, _w1, _w2] = _v_semanticexpressionmethod; return mir_lower_is_mutating_method(method_name) ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>([&]() -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*object)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*object)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mir_types::LocalId>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [local_id] = ok; return mir_lower_mutating_method_statement(state, local_id, method_name, arguments); }
}, mir_lower_lookup_local(state, name)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: mutating method requires identifier receiver")}); }()) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerLocalStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [local_step] = ok; return ast::Ok<lower_fn::MirLowerState>(local_step.state); }
}, mir_lower_method_to_local(state, object, method_name, arguments))); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: if expression statement requires return context")}); } return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerOperandStep>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [operand_step] = ok; return ast::Ok<lower_fn::MirLowerState>(operand_step.state); }
}, mir_lower_operand_from_expression(state, expression)); }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: unsupported statement")}); }();
}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_statements_from_index(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, int index) noexcept{return index >= statements.size() ? ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(ast::Ok<lower_fn::MirLowerState>(state)) : ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> { auto [next_state] = ok; return mir_lower_statements_from_index(next_state, statements, index + 1); }
}, mir_lower_statement(state, statements[index])));}

ast::Result<lower_fn::MirLowerState, mlc::Array<mlc::String>> mir_lower_statements(lower_fn::MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept{return mir_lower_statements_from_index(state, statements, 0);}

mlc::Array<mir_types::MirParam> mir_lower_build_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mir_types::MirParam> mir_params = {};
int index = 0;
while (index < params.size()){
{
mir_params.push_back(mir_types::MirParam{params[index]->name, mir_lower_unknown_type()});
index = index + 1;
}
}
return mir_params;
}

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> lower_semantic_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
return [&]() -> ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*declaration))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*declaration)); auto [name, _w0, _w1, params, return_type, body, _w2, _w3] = _v_semanticdeclarationfn; return [&]() -> ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> { 
  lower_fn::MirLowerState state = mir_lower_bind_params(mir_lower_state_new(), params);
  return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<lower_fn::MirLowerState>& ok) -> ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> { auto [lowered_state] = ok; return ast::Ok<mir_types::MirFunction>(mir_types::MirFunction{name, mir_lower_build_params(params), lowered_state.locals, lowered_state.finished_blocks, return_type}); }
}, mir_lower_return_expression(state, body));
 }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir lower: expected function declaration")}); }();
}

} // namespace lower_fn
