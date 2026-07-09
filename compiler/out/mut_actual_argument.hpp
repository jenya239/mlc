#ifndef MUT_ACTUAL_ARGUMENT_HPP
#define MUT_ACTUAL_ARGUMENT_HPP

#include "mlc.hpp"
#include <variant>
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "type_gen.hpp"
namespace mut_actual_argument {

struct MutActualArgumentMaterializationOutcome {
  mlc::String prelude_block;
  mlc::Array<mlc::String> materialized_argument_fragments;
};
struct MutActualArgumentMaterializationOutcomeCpp {
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements;
  mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> materialized_argument_expressions;
};
bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SemanticExpression> actual_argument) noexcept;
std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_invoked_lambda() noexcept;
mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept;
std::shared_ptr<cpp_ast::CppExpression> wrap_invoke_with_mut_prelude_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements, std::shared_ptr<cpp_ast::CppExpression> invoke_expression) noexcept;
template<typename __F4>
MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 evaluate_expression) noexcept;
template<typename __F4>
MutActualArgumentMaterializationOutcomeCpp materialize_positional_actual_arguments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 evaluate_expression) noexcept;
template<typename __F4>
MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 evaluate_expression) noexcept{
  if ((mut_parameter_flags.length() != positional_argument_expressions.length()))   {
    return MutActualArgumentMaterializationOutcome{mlc::String("", 0), positional_argument_expressions.map([=](std::shared_ptr<semantic_ir::SemanticExpression> expression) mutable { return evaluate_expression(expression, context, gen_stmts); })};
  } else   {
    return positional_argument_expressions.fold(MutActualArgumentMaterializationOutcome{mlc::String("", 0), {}}, [=](MutActualArgumentMaterializationOutcome accumulator, std::shared_ptr<semantic_ir::SemanticExpression> positional_argument_expression) mutable { return [&]() {
auto position_index = accumulator.materialized_argument_fragments.length();
auto evaluated_fragment = evaluate_expression(positional_argument_expression, context, gen_stmts);
return [&]() -> MutActualArgumentMaterializationOutcome {
  if (((mut_parameter_flags[position_index] != 0) && (!sexpr_is_stable_reference_for_cpp_mut_parameter(positional_argument_expression))))   {
    return [&]() {
auto holder_variable_name = ((mlc::String("__mut_actual_argument_holder_", 29) + mlc::to_string(mlc::to_string(position_index))) + mlc::String("", 0));
auto holder_type_spelling = type_gen::sem_type_to_cpp(context, semantic_ir::sexpr_type(positional_argument_expression));
return MutActualArgumentMaterializationOutcome{((((((((mlc::String("", 0) + mlc::to_string(accumulator.prelude_block)) + mlc::String("", 0)) + mlc::to_string(holder_type_spelling)) + mlc::String(" ", 1)) + mlc::to_string(holder_variable_name)) + mlc::String(" = ", 3)) + mlc::to_string(evaluated_fragment)) + mlc::String(";\n", 2)), accumulator.materialized_argument_fragments.concat(mlc::Array<mlc::String>{holder_variable_name})};
}();
  } else   {
    return MutActualArgumentMaterializationOutcome{accumulator.prelude_block, accumulator.materialized_argument_fragments.concat(mlc::Array<mlc::String>{evaluated_fragment})};
  }
}();
}(); });
  }
}
template<typename __F4>
MutActualArgumentMaterializationOutcomeCpp materialize_positional_actual_arguments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 evaluate_expression) noexcept{
  if ((mut_parameter_flags.length() != positional_argument_expressions.length()))   {
    return MutActualArgumentMaterializationOutcomeCpp{{}, positional_argument_expressions.map([=](std::shared_ptr<semantic_ir::SemanticExpression> expression) mutable { return evaluate_expression(expression, context, gen_stmts); })};
  } else   {
    return positional_argument_expressions.fold(MutActualArgumentMaterializationOutcomeCpp{{}, {}}, [=](MutActualArgumentMaterializationOutcomeCpp accumulator, std::shared_ptr<semantic_ir::SemanticExpression> positional_argument_expression) mutable { return [&]() {
auto position_index = accumulator.materialized_argument_expressions.length();
auto evaluated_expression = evaluate_expression(positional_argument_expression, context, gen_stmts);
return [&]() -> MutActualArgumentMaterializationOutcomeCpp {
  if (((mut_parameter_flags[position_index] != 0) && (!sexpr_is_stable_reference_for_cpp_mut_parameter(positional_argument_expression))))   {
    return [&]() {
auto holder_variable_name = ((mlc::String("__mut_actual_argument_holder_", 29) + mlc::to_string(mlc::to_string(position_index))) + mlc::String("", 0));
auto prelude_statements = accumulator.prelude_statements;
prelude_statements.push_back(emit_helpers::make_auto_cpp_statement(holder_variable_name, evaluated_expression));
return MutActualArgumentMaterializationOutcomeCpp{prelude_statements, accumulator.materialized_argument_expressions.concat(mlc::Array{emit_helpers::make_identifier_cpp_expression(holder_variable_name)})};
}();
  } else   {
    return MutActualArgumentMaterializationOutcomeCpp{accumulator.prelude_statements, accumulator.materialized_argument_expressions.concat(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{evaluated_expression})};
  }
}();
}(); });
  }
}

} // namespace mut_actual_argument

#endif // MUT_ACTUAL_ARGUMENT_HPP
