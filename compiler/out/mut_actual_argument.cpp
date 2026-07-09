#define main mlc_user_main
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

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SemanticExpression> actual_argument) noexcept{
  return [&]() -> bool {
auto __match_subject = actual_argument;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [__0, __1, __2] = semanticExpressionIdent; return true;
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [inner, __1, __2, __3] = semanticExpressionField; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [inner, __1, __2, __3] = semanticExpressionIndex; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner);
}
return false;
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_invoked_lambda() noexcept{
  return std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String("", 0), true});
}
mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept{
  if ((prelude_block.length() == 0))   {
    return invoke_expression;
  } else   {
    return ((((mlc::String("[&]() {\n", 8) + mlc::to_string(prelude_block)) + mlc::String("\nreturn ", 8)) + mlc::to_string(invoke_expression)) + mlc::String(";\n}()", 5));
  }
}
std::shared_ptr<cpp_ast::CppExpression> wrap_invoke_with_mut_prelude_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements, std::shared_ptr<cpp_ast::CppExpression> invoke_expression) noexcept{
  if ((prelude_statements.length() == 0))   {
    return invoke_expression;
  } else   {
    auto body_statements = prelude_statements;
    body_statements.push_back(emit_helpers::make_return_cpp_statement(invoke_expression));
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppLambda{mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{reference_capture_for_invoked_lambda()}, {}, std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mlc::String("auto", 4)}), body_statements}), {}});
  }
}
MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
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
MutActualArgumentMaterializationOutcomeCpp materialize_positional_actual_arguments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
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
