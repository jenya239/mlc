#define main mlc_user_main
#include "mut_actual_argument.hpp"

namespace mut_actual_argument {

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SExpr> actual_argument) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [__0, __1, __2] = sExprIdent; return true; },
[&](const semantic_ir::SExprField& sExprField) { auto [inner, __1, __2, __3] = sExprField; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner); },
[&](const semantic_ir::SExprIndex& sExprIndex) { auto [inner, __1, __2, __3] = sExprIndex; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner); },
[&](const auto& __v) { return false; }
}, (*actual_argument));
}
mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept{
if ((prelude_block.length() == 0)) {
return invoke_expression;
} else {
((((mlc::String("[&]() {\n", 8) + mlc::to_string(prelude_block)) + mlc::String("\nreturn ", 8)) + mlc::to_string(invoke_expression)) + mlc::String(";\n}()", 5));
export_;
/* unit */;
materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<bool>{((Shared{} < semantic_ir::SExpr{}) > /* unit */), mut_parameter_flags}[i32], context::CodegenContext{}, mlc::Array<bool>{((Shared{} < semantic_ir::SStmt{}) > /* unit */), context::CodegenContext{}}, string, ((Shared{} < semantic_ir::SExpr{}) > /* unit */), mlc::Array<bool>{((Shared{} < semantic_ir::SStmt{}) > /* unit */), context::CodegenContext{}}, string);
/* unit */;
string;
/* unit */;
/* unit */;
MutActualArgumentMaterializationOutcome{} = [&]() -> MutActualArgumentMaterializationOutcome {
if ((mut_parameter_flags.length() != positional_argument_expressions.length())) {
return MutActualArgumentMaterializationOutcome{mlc::String("", 0), positional_argument_expressions.map([=](auto expression) mutable { return evaluate_expression(expression, context, gen_stmts); })};
} else {
return positional_argument_expressions.fold(MutActualArgumentMaterializationOutcome{mlc::String("", 0), {}}, [=](auto accumulator, auto positional_argument_expression) mutable { return [&]() {
auto position_index = accumulator.materialized_argument_fragments.length();
auto evaluated_fragment = evaluate_expression(positional_argument_expression, context, gen_stmts);
return [&]() -> MutActualArgumentMaterializationOutcome {
if (((mut_parameter_flags[position_index] != 0) && (!sexpr_is_stable_reference_for_cpp_mut_parameter(positional_argument_expression)))) {
return [&]() {
auto holder_variable_name = ((mlc::String("__mut_actual_argument_holder_", 29) + mlc::to_string(mlc::to_string(position_index))) + mlc::String("", 0));
auto holder_type_spelling = type_gen::sem_type_to_cpp(context, semantic_ir::sexpr_type(positional_argument_expression));
return MutActualArgumentMaterializationOutcome{((((((((mlc::String("", 0) + mlc::to_string(accumulator.prelude_block)) + mlc::String("", 0)) + mlc::to_string(holder_type_spelling)) + mlc::String(" ", 1)) + mlc::to_string(holder_variable_name)) + mlc::String(" = ", 3)) + mlc::to_string(evaluated_fragment)) + mlc::String(";\n", 2)), accumulator.materialized_argument_fragments.concat(mlc::Array<mlc::String>{holder_variable_name})};
}();
} else {
return MutActualArgumentMaterializationOutcome{accumulator.prelude_block, accumulator.materialized_argument_fragments.concat(mlc::Array{evaluated_fragment})};
}
}();
}(); });
}
}();
}
}

} // namespace mut_actual_argument
