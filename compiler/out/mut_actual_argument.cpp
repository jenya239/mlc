#include "mut_actual_argument.hpp"

#include "semantic_ir.hpp"
#include "context.hpp"
#include "type_gen.hpp"

namespace mut_actual_argument {

using namespace semantic_ir;
using namespace context;
using namespace type_gen;

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SExpr> actual_argument) noexcept;

mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept;

mut_actual_argument::MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SExpr> actual_argument) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SExprIdent>((*actual_argument)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*actual_argument)._); auto [_w0, _w1, _w2] = _v_sexprident; return true; } if (std::holds_alternative<semantic_ir::SExprField>((*actual_argument)._)) { auto _v_sexprfield = std::get<semantic_ir::SExprField>((*actual_argument)._); auto [inner, _w0, _w1, _w2] = _v_sexprfield; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner); } if (std::holds_alternative<semantic_ir::SExprIndex>((*actual_argument)._)) { auto _v_sexprindex = std::get<semantic_ir::SExprIndex>((*actual_argument)._); auto [inner, _w0, _w1, _w2] = _v_sexprindex; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner); } return false; }();}

mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept{return prelude_block.length() == 0 ? invoke_expression : mlc::String("[&]() {\n") + prelude_block + mlc::String("\nreturn ") + invoke_expression + mlc::String(";\n}()");}

mut_actual_argument::MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return mut_parameter_flags.size() != positional_argument_expressions.size() ? mut_actual_argument::MutActualArgumentMaterializationOutcome{mlc::String(""), positional_argument_expressions.map([evaluate_expression, context, gen_stmts](std::shared_ptr<semantic_ir::SExpr> expression) mutable { return evaluate_expression(expression, context, gen_stmts); })} : positional_argument_expressions.fold(mut_actual_argument::MutActualArgumentMaterializationOutcome{mlc::String(""), {}}, [evaluate_expression, context, gen_stmts, mut_parameter_flags](mut_actual_argument::MutActualArgumentMaterializationOutcome accumulator, std::shared_ptr<semantic_ir::SExpr> positional_argument_expression) mutable { return [&]() -> mut_actual_argument::MutActualArgumentMaterializationOutcome { 
  int position_index = accumulator.materialized_argument_fragments.size();
  mlc::String evaluated_fragment = evaluate_expression(positional_argument_expression, context, gen_stmts);
  return mut_parameter_flags[position_index] != 0 && !sexpr_is_stable_reference_for_cpp_mut_parameter(positional_argument_expression) ? [&]() -> mut_actual_argument::MutActualArgumentMaterializationOutcome { 
  mlc::String holder_variable_name = mlc::String("__mut_actual_argument_holder_") + mlc::to_string(position_index);
  mlc::String holder_type_spelling = type_gen::sem_type_to_cpp(context, semantic_ir::sexpr_type(positional_argument_expression));
  return mut_actual_argument::MutActualArgumentMaterializationOutcome{accumulator.prelude_block + holder_type_spelling + mlc::String(" ") + holder_variable_name + mlc::String(" = ") + evaluated_fragment + mlc::String(";\n"), accumulator.materialized_argument_fragments.concat(mlc::Array<mlc::String>{holder_variable_name})};
 }() : mut_actual_argument::MutActualArgumentMaterializationOutcome{accumulator.prelude_block, accumulator.materialized_argument_fragments.concat(mlc::Array<mlc::String>{evaluated_fragment})};
 }(); });}

} // namespace mut_actual_argument
