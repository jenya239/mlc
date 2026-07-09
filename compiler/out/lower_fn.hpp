#ifndef LOWER_FN_HPP
#define LOWER_FN_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "mir_types.hpp"
#include "mir_ids.hpp"
namespace lower_fn {

struct MirLowerState {
  mlc::HashMap<mlc::String, mir_types::LocalId> name_environment;
  mlc::Array<mir_types::MirLocal> locals;
  mlc::Array<mir_types::MirBlock> finished_blocks;
  mir_types::BlockId current_block_id;
  mlc::String current_label;
  mlc::Array<mir_types::MirStmt> current_stmts;
  mir_ids::MirIdPool id_pool;
};
struct MirLowerBlockStep {
  MirLowerState state;
  mir_types::BlockId block_id;
};
struct MirLowerLocalStep {
  MirLowerState state;
  mir_types::LocalId local_id;
};
struct MirLowerMethodOperandsStep {
  MirLowerState state;
  mlc::Array<mir_types::MirOperand> operands;
};
struct MirLowerCallOperandsStep {
  MirLowerState state;
  mlc::Array<mir_types::MirOperand> operands;
};
struct MirLowerOperandStep {
  MirLowerState state;
  mir_types::MirOperand operand;
};
struct MirLowerRvalueStep {
  MirLowerState state;
  mir_types::MirRvalue rvalue;
};
std::shared_ptr<registry::Type> mir_lower_unknown_type() noexcept;
bool mir_lower_is_assign_operation(mlc::String operation) noexcept;
MirLowerState mir_lower_state_new() noexcept;
MirLowerBlockStep mir_lower_allocate_block_index(MirLowerState state) noexcept;
MirLowerState mir_lower_finish_current_block(MirLowerState state, mir_types::MirTerminator terminator) noexcept;
MirLowerState mir_lower_start_block(MirLowerState state, mir_types::BlockId block_id, mlc::String label) noexcept;
MirLowerState mir_lower_bind_params(MirLowerState state, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
MirLowerLocalStep mir_lower_allocate_local(MirLowerState state, mlc::String name, std::shared_ptr<registry::Type> type_value) noexcept;
MirLowerState mir_lower_bind_name(MirLowerState state, mlc::String name, mir_types::LocalId local_id) noexcept;
ast::Result<mir_types::LocalId, mlc::Array<mlc::String>> mir_lower_lookup_local(MirLowerState state, mlc::String name) noexcept;
MirLowerState mir_lower_emit_assign(MirLowerState state, mir_types::LocalId local_id, mir_types::MirRvalue rvalue) noexcept;
MirLowerState mir_lower_emit_call_assign(MirLowerState state, mir_types::LocalId local_id, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_call_target_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> mir_lower_method_native_name(mlc::String method_name) noexcept;
bool mir_lower_is_mutating_method(mlc::String method_name) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_push_elements(MirLowerState state, mir_types::LocalId array_local_id, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, int index) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_array_literal_to_local(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_index_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_map_new_local(MirLowerState state) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_method_call_on_operand(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;
ast::Result<MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_method_operands(MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept;
ast::Result<MirLowerMethodOperandsStep, mlc::Array<mlc::String>> mir_lower_method_operands_with_receiver(MirLowerState state, mir_types::MirOperand object_operand, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_mutating_method_statement(MirLowerState state, mir_types::LocalId local_id, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;
ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_collect_operands(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, int index, mlc::Array<mir_types::MirOperand> operands) noexcept;
ast::Result<MirLowerCallOperandsStep, mlc::Array<mlc::String>> mir_lower_operands_from_arguments(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments) noexcept;
MirLowerOperandStep mir_lower_temp_operand_from_rvalue(MirLowerState state, mir_types::MirRvalue rvalue) noexcept;
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_int(MirLowerState state, mir_types::MirOperand left_operand, int right_value) noexcept;
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_equals_const_bool(MirLowerState state, mir_types::MirOperand left_operand, bool right_value) noexcept;
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_from_expression(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
ast::Result<MirLowerRvalueStep, mlc::Array<mlc::String>> mir_lower_rvalue_from_expression(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
MirLowerState mir_lower_finish_return(MirLowerState state, mir_types::MirOperand operand) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_return_expression(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_emit_call_to_local(MirLowerState state, mlc::String callee_name, mlc::Array<mir_types::MirOperand> arguments) noexcept;
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_and(MirLowerState state, mir_types::MirOperand left_operand, mir_types::MirOperand right_operand) noexcept;
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_operand_variant_is(MirLowerState state, mir_types::MirOperand scrutinee, mlc::String tag) noexcept;
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_ctor_subpattern_tests(MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mir_types::MirOperand accumulated, int index) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpattern(MirLowerState state, mir_types::MirOperand scrutinee, int field_index, std::shared_ptr<ast::Pattern> pattern) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_ctor_subpatterns(MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, int index) noexcept;
bool mir_lower_pattern_needs_conditional_test(std::shared_ptr<ast::Pattern> pattern) noexcept;
ast::Result<MirLowerOperandStep, mlc::Array<mlc::String>> mir_lower_pattern_matches_operand(MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<ast::Pattern> pattern) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arm_body_with_binding(MirLowerState state, mir_types::MirOperand scrutinee, std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_match_arms_return(MirLowerState state, mir_types::MirOperand scrutinee, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, int index) noexcept;
std::shared_ptr<semantic_ir::SemanticMatchArm> mir_lower_match_arm_with_pattern(std::shared_ptr<semantic_ir::SemanticMatchArm> arm, std::shared_ptr<ast::Pattern> pattern) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_split_or_arm(std::shared_ptr<semantic_ir::SemanticMatchArm> arm) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> mir_lower_expand_or_arms(mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_match_return(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_bind_lambda_parameters_from_index(MirLowerState state, mlc::Array<mlc::String> parameter_names, mlc::Array<mir_types::MirOperand> argument_operands, int index, std::shared_ptr<semantic_ir::SemanticExpression> body, mir_types::LocalId result_local_id) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_lambda_inline_body(MirLowerState state, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body, mlc::Array<mir_types::MirOperand> argument_operands, mir_types::LocalId result_local_id) noexcept;
ast::Result<MirLowerLocalStep, mlc::Array<mlc::String>> mir_lower_expression_to_local(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_question_return(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> inner) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_while_statement(MirLowerState state, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_assign_identifier(MirLowerState state, mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> right) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_statement(MirLowerState state, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_statements_from_index(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, int index) noexcept;
ast::Result<MirLowerState, mlc::Array<mlc::String>> mir_lower_statements(MirLowerState state, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;
mlc::Array<mir_types::MirParam> mir_lower_build_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> lower_semantic_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

} // namespace lower_fn

#endif // LOWER_FN_HPP
