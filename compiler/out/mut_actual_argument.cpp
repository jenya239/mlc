#include "mut_actual_argument.hpp"

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "type_gen.hpp"

namespace mut_actual_argument {

using namespace semantic_ir;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace type_gen;

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SemanticExpression> actual_argument) noexcept;

std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_invoked_lambda() noexcept;

mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> wrap_invoke_with_mut_prelude_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements, std::shared_ptr<cpp_ast::CppExpression> invoke_expression) noexcept;

mut_actual_argument::MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp materialize_positional_actual_arguments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SemanticExpression> actual_argument) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*actual_argument)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*actual_argument)._); auto [_w0, _w1, _w2] = _v_semanticexpressionident; return true; } if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*actual_argument)._)) { auto _v_semanticexpressionfield = std::get<semantic_ir::SemanticExpressionField>((*actual_argument)._); auto [inner, _w0, _w1, _w2] = _v_semanticexpressionfield; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*actual_argument)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*actual_argument)._); auto [inner, _w0, _w1, _w2] = _v_semanticexpressionindex; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner); } return false; }();}

std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_invoked_lambda() noexcept{return std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String(""), true});}

mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept{return prelude_block.length() == 0 ? invoke_expression : mlc::String("[&]() {\n") + prelude_block + mlc::String("\nreturn ") + invoke_expression + mlc::String(";\n}()");}

std::shared_ptr<cpp_ast::CppExpression> wrap_invoke_with_mut_prelude_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements, std::shared_ptr<cpp_ast::CppExpression> invoke_expression) noexcept{return prelude_statements.size() == 0 ? invoke_expression : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements = prelude_statements;
  body_statements.push_back(emit_helpers::make_return_cpp_statement(invoke_expression));
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppLambda(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{reference_capture_for_invoked_lambda()}, {}, std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("auto"))), body_statements)), {}));
 }();}

mut_actual_argument::MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return mut_parameter_flags.size() != positional_argument_expressions.size() ? mut_actual_argument::MutActualArgumentMaterializationOutcome{mlc::String(""), positional_argument_expressions.map([evaluate_expression, context, gen_stmts](std::shared_ptr<semantic_ir::SemanticExpression> expression) mutable { return evaluate_expression(expression, context, gen_stmts); })} : positional_argument_expressions.fold(mut_actual_argument::MutActualArgumentMaterializationOutcome{mlc::String(""), {}}, [evaluate_expression, context, gen_stmts, mut_parameter_flags](mut_actual_argument::MutActualArgumentMaterializationOutcome accumulator, std::shared_ptr<semantic_ir::SemanticExpression> positional_argument_expression) mutable { return [&]() -> mut_actual_argument::MutActualArgumentMaterializationOutcome { 
  int position_index = accumulator.materialized_argument_fragments.size();
  mlc::String evaluated_fragment = evaluate_expression(positional_argument_expression, context, gen_stmts);
  return mut_parameter_flags[position_index] != 0 && !sexpr_is_stable_reference_for_cpp_mut_parameter(positional_argument_expression) ? [&]() -> mut_actual_argument::MutActualArgumentMaterializationOutcome { 
  mlc::String holder_variable_name = mlc::String("__mut_actual_argument_holder_") + mlc::to_string(position_index);
  mlc::String holder_type_spelling = type_gen::sem_type_to_cpp(context, semantic_ir::sexpr_type(positional_argument_expression));
  return mut_actual_argument::MutActualArgumentMaterializationOutcome{accumulator.prelude_block + holder_type_spelling + mlc::String(" ") + holder_variable_name + mlc::String(" = ") + evaluated_fragment + mlc::String(";\n"), accumulator.materialized_argument_fragments.concat(mlc::Array<mlc::String>{holder_variable_name})};
 }() : mut_actual_argument::MutActualArgumentMaterializationOutcome{accumulator.prelude_block, accumulator.materialized_argument_fragments.concat(mlc::Array<mlc::String>{evaluated_fragment})};
 }(); });}

mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp materialize_positional_actual_arguments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return mut_parameter_flags.size() != positional_argument_expressions.size() ? mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp{{}, positional_argument_expressions.map([evaluate_expression, context, gen_stmts](std::shared_ptr<semantic_ir::SemanticExpression> expression) mutable { return evaluate_expression(expression, context, gen_stmts); })} : positional_argument_expressions.fold(mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp{{}, {}}, [evaluate_expression, context, gen_stmts, mut_parameter_flags](mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp accumulator, std::shared_ptr<semantic_ir::SemanticExpression> positional_argument_expression) mutable { return [&]() -> mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp { 
  int position_index = accumulator.materialized_argument_expressions.size();
  std::shared_ptr<cpp_ast::CppExpression> evaluated_expression = evaluate_expression(positional_argument_expression, context, gen_stmts);
  return mut_parameter_flags[position_index] != 0 && !sexpr_is_stable_reference_for_cpp_mut_parameter(positional_argument_expression) ? [&]() -> mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp { 
  mlc::String holder_variable_name = mlc::String("__mut_actual_argument_holder_") + mlc::to_string(position_index);
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements = accumulator.prelude_statements;
  prelude_statements.push_back(emit_helpers::make_auto_cpp_statement(holder_variable_name, evaluated_expression));
  return mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp{prelude_statements, accumulator.materialized_argument_expressions.concat(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{emit_helpers::make_identifier_cpp_expression(holder_variable_name)})};
 }() : mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp{accumulator.prelude_statements, accumulator.materialized_argument_expressions.concat(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{evaluated_expression})};
 }(); });}

} // namespace mut_actual_argument
