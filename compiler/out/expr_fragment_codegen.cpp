#include "expr_fragment_codegen.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "mut_actual_argument.hpp"
#include "method_gen.hpp"
#include "match_gen.hpp"
#include "record_gen.hpp"
#include "type_gen.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace expr_fragment_codegen {

using namespace registry;
using namespace semantic_ir;
using namespace registry;
using namespace semantic_type_structure;
using namespace context;
using namespace literals;
using namespace cpp_naming;
using namespace expression_support;
using namespace mut_actual_argument;
using namespace method_gen;
using namespace match_gen;
using namespace record_gen;
using namespace type_gen;
using namespace statement_context;
using namespace expr;

mlc::String gen_binary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_unary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_call_via_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_method_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_if_else_code(std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SExpr> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_if_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_block_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_match_via_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_extern_via_visitor(std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String argument_list_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_record_via_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_record_update_via_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_array_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_tuple_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_question_via_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String cpp_lambda_header_from_semantic_function_type(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda, context::CodegenContext context) noexcept;

mlc::String gen_lambda_via_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_with_via_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_binary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String method = semantic_type_structure::operator_method_for(operation);
std::shared_ptr<registry::Type> left_type = semantic_ir::sexpr_type(left_expression);
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*left_type))) { auto _v_tnamed = std::get<registry::TNamed>((*left_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return method != mlc::String("") && type_name != mlc::String("") ? type_name + mlc::String("_") + method + mlc::String("(") + evaluate_expression(left_expression, context, gen_stmts) + mlc::String(", ") + evaluate_expression(right_expression, context, gen_stmts) + mlc::String(")") : expr::parenthesized_binary(evaluate_expression(left_expression, context, gen_stmts), operation, evaluate_expression(right_expression, context, gen_stmts));
}

mlc::String gen_unary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::parenthesized_unary(operation, evaluate_expression(inner_expression, context, gen_stmts));}

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*function_expression)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*function_expression)._); auto [name, _w0, _w1] = _v_sexprident; return context::qualify_function_callee(cpp_naming::map_builtin(name), context); } return evaluate_expression(function_expression, context, gen_stmts); }();}

mlc::String gen_call_via_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String function_code = gen_call_function_code(function_expression, context, gen_stmts, evaluate_expression);
mut_actual_argument::MutActualArgumentMaterializationOutcome materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_maybe_holding_mut_references(arguments, call_parameter_mutability_flags, context, gen_stmts, evaluate_expression);
mlc::String argument_list = materialization_outcome.materialized_argument_fragments.join(mlc::String(", "));
mlc::String invoke_expression = expression_support::is_constructor_call(function_expression) ? expr::constructor_call_braces(function_code, argument_list) : expr::function_call_parentheses(function_code, argument_list);
return mut_actual_argument::wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(materialization_outcome.prelude_block, invoke_expression);
}

mlc::String gen_method_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return method_gen::gen_method_expr(object, method_name, arguments, receiver_and_parameter_mutability_flags, context, gen_stmts, evaluate_expression);}

mlc::String gen_if_else_code(std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SExpr> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprArray>((*else_expression)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*else_expression)._); auto [elements, _w0, array_span] = _v_sexprarray; return elements.size() > 0 ? evaluate_expression(else_expression, context, gen_stmts) : [&]() -> mlc::String { 
  std::shared_ptr<registry::Type> inner_preferred = semantic_type_structure::array_element_type_from_array_type(if_semantic_type);
  std::shared_ptr<registry::Type> inner_from_then_branch = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(then_expression));
  std::shared_ptr<registry::Type> inner_for_empty_array = !semantic_type_structure::type_is_unknown(inner_preferred) ? inner_preferred : inner_from_then_branch;
  return semantic_type_structure::type_is_unknown(inner_for_empty_array) ? evaluate_expression(else_expression, context, gen_stmts) : evaluate_expression(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(elements, std::make_shared<registry::Type>(registry::TArray(inner_for_empty_array)), array_span)), context, gen_stmts);
 }(); } return evaluate_expression(else_expression, context, gen_stmts); }();}

mlc::String gen_if_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String condition_code = evaluate_expression(condition, context, gen_stmts);
mlc::String then_code = evaluate_expression(then_expression, context, gen_stmts);
mlc::String else_code = gen_if_else_code(else_expression, if_semantic_type, then_expression, context, gen_stmts, evaluate_expression);
return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*if_semantic_type))) { auto _v_tnamed = std::get<registry::TNamed>((*if_semantic_type)); auto [type_name] = _v_tnamed; return [&]() -> mlc::String { 
  mlc::String type_cpp = context::CodegenContext_resolve(context, type_name);
  return type_cpp == mlc::String("auto") ? expr::ternary_conditional(condition_code, then_code, else_code) : mlc::String("[&]() -> ") + type_cpp + mlc::String(" {\nif (") + condition_code + mlc::String(") {\nreturn ") + then_code + mlc::String(";\n} else {\nreturn ") + else_code + mlc::String(";\n}\n}()");
 }(); } return expr::ternary_conditional(condition_code, then_code, else_code); }();
}

mlc::String gen_block_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return statements.size() == 0 ? evaluate_expression(result_expression, context, gen_stmts) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts(statements, context) + expr::block_immediate_function_return_fragment(evaluate_expression(result_expression, statement_context::stmts_final_ctx(statements, context), gen_stmts));
  return expr::block_as_immediate_invoked_function_expression(body);
 }();}

mlc::String gen_match_via_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return match_gen::gen_match(subject, arms, match_semantic_type, context, gen_stmts, evaluate_expression);}

mlc::String gen_extern_via_visitor(std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_extern_placeholder();}

mlc::String argument_list_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expressions.map([evaluate_expression, context, gen_stmts](std::shared_ptr<semantic_ir::SExpr> expression) mutable { return evaluate_expression(expression, context, gen_stmts); }).join(mlc::String(", "));}

mlc::String gen_record_via_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return record_gen::gen_record_expr(type_name, field_values, expression_type, context, gen_stmts, evaluate_expression);}

mlc::String gen_record_update_via_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return record_gen::gen_record_update_expr(type_name, base_expression, overrides, context, gen_stmts, evaluate_expression);}

mlc::String gen_array_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*element_container_semantic_type))) { auto _v_tarray = std::get<registry::TArray>((*element_container_semantic_type)); auto [inner_type] = _v_tarray; return [&]() -> mlc::String { 
  mlc::String element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
  return elements.size() == 0 ? expr::typed_array_empty_or_untyped_empty(element_cpp) : element_cpp == mlc::String("auto") ? expr::array_literal(argument_list_via_visitor(elements, context, gen_stmts, evaluate_expression)) : expr::typed_array_braced_initializer(element_cpp, argument_list_via_visitor(elements, context, gen_stmts, evaluate_expression));
 }(); } return expr::array_literal(argument_list_via_visitor(elements, context, gen_stmts, evaluate_expression)); }();}

mlc::String gen_tuple_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return mlc::String("std::make_tuple(") + elements.map([evaluate_expression, context, gen_stmts](std::shared_ptr<semantic_ir::SExpr> element) mutable { return evaluate_expression(element, context, gen_stmts); }).join(mlc::String(", ")) + mlc::String(")");}

mlc::String gen_question_via_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::question_try_result(evaluate_expression(inner_expression, context, gen_stmts));}

mlc::String cpp_lambda_header_from_semantic_function_type(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda, context::CodegenContext context) noexcept{
mlc::String capture_clause = parameter_binding_names.size() == 0 ? mlc::String("[]") : mlc::String("[&]");
return [&]() -> mlc::String { if (std::holds_alternative<registry::TFn>((*semantic_function_type_for_lambda))) { auto _v_tfn = std::get<registry::TFn>((*semantic_function_type_for_lambda)); auto [lambda_parameter_semantic_types, _w0] = _v_tfn; return lambda_parameter_semantic_types.size() != parameter_binding_names.size() ? expression_support::cpp_lambda_header_prefix(parameter_binding_names) : [&]() -> mlc::String { 
  mlc::Array<mlc::String> formatted_parameter_declarations = {};
  int lambda_parameter_index = 0;
  while (lambda_parameter_index < parameter_binding_names.size()){
{
mlc::String raw_param_type = type_gen::sem_type_to_cpp(context, lambda_parameter_semantic_types[lambda_parameter_index]);
mlc::String param_type_cpp = raw_param_type == mlc::String("void") ? mlc::String("std::tuple<>") : raw_param_type;
formatted_parameter_declarations.push_back(expr::parameter_declaration_item(param_type_cpp, cpp_naming::cpp_safe(parameter_binding_names[lambda_parameter_index])));
lambda_parameter_index = lambda_parameter_index + 1;
}
}
  return capture_clause + mlc::String("(") + formatted_parameter_declarations.join(mlc::String(", ")) + mlc::String(") mutable");
 }(); } return expression_support::cpp_lambda_header_prefix(parameter_binding_names); }();
}

mlc::String gen_lambda_via_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::lambda_with_return(cpp_lambda_header_from_semantic_function_type(parameter_binding_names, semantic_function_type_for_lambda_expression, context), evaluate_expression(body_expression_under_lambda, context, gen_stmts));}

mlc::String gen_with_via_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String resource_code = evaluate_expression(resource, context, gen_stmts);
mlc::String body_code = gen_stmts(statements, context);
return expr::with_block_statement(resource_code, binder, body_code);
}

} // namespace expr_fragment_codegen
