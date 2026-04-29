#include "expr_eval.hpp"

#include "semantic_ir.hpp"
#include "semantic_ir.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "literals.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "expression_support.hpp"
#include "match_gen.hpp"
#include "record_gen.hpp"
#include "method_gen.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace expr_eval {

using namespace semantic_ir;
using namespace semantic_ir;
using namespace context;
using namespace cpp_naming;
using namespace literals;
using namespace literals;
using namespace identifiers;
using namespace expression_support;
using namespace match_gen;
using namespace record_gen;
using namespace method_gen;
using namespace statement_context;
using namespace expr;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < expressions.size()){
{
parts.push_back(eval_expr(expressions[index], context, gen_stmts));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::parenthesized_binary(eval_expr(left_expr, context, gen_stmts), operation, eval_expr(right_expr, context, gen_stmts));}

mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::parenthesized_unary(operation, eval_expr(inner_expr, context, gen_stmts));}

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*function_expr)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*function_expr)._); auto [name, _w0, _w1] = _v_sexprident; return context::context_resolve(context, cpp_naming::map_builtin(name)); } return eval_expr(function_expr, context, gen_stmts); }();}

mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String function_code = gen_call_function_code(function_expr, context, gen_stmts);
mlc::String argument_list = eval_argument_list(arguments, context, gen_stmts);
return expression_support::is_constructor_call(function_expr) ? expr::constructor_call_braces(function_code, argument_list) : expr::function_call_parentheses(function_code, argument_list);
}

mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String object_code = eval_expr(object, context, gen_stmts);
mlc::String operator_ = expression_support::field_access_operator(object, context);
return expr::field_access(object_code, operator_, cpp_naming::cpp_safe(field_name));
}

mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::index_subscript(eval_expr(object, context, gen_stmts), eval_expr(index_expr, context, gen_stmts));}

mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::ternary_conditional(eval_expr(condition, context, gen_stmts), eval_expr(then_expr, context, gen_stmts), eval_expr(else_expr, context, gen_stmts));}

mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return statements.size() == 0 ? eval_expr(result_expr, context, gen_stmts) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts(statements, context) + expr::block_immediate_function_return_fragment(eval_expr(result_expr, statement_context::stmts_final_ctx(statements, context), gen_stmts));
  return expr::block_as_immediate_invoked_function_expression(body);
 }();}

mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::while_loop_immediate_invoked_function_expression(eval_expr(condition, context, gen_stmts), gen_stmts(statements, context));}

mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::for_loop_immediate_invoked_function_expression(cpp_naming::cpp_safe(variable), eval_expr(iterator, context, gen_stmts), gen_stmts(statements, context));}

mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::array_literal(eval_argument_list(elements, context, gen_stmts));}

mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String inner_code = eval_expr(inner_expr, context, gen_stmts);
return expr::question_try_result(inner_code);
}

mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr::lambda_with_return(expression_support::cpp_lambda_header_prefix(parameters), eval_expr(body_expr, context, gen_stmts));}

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> mlc::String { auto [integer_value, _w0, _w1] = sexprint; return literals::gen_integer_literal(integer_value); },
  [&](const SExprStr& sexprstr) -> mlc::String { auto [string_value, _w0, _w1] = sexprstr; return literals::gen_string_literal(string_value); },
  [&](const SExprBool& sexprbool) -> mlc::String { auto [boolean_value, _w0, _w1] = sexprbool; return literals::gen_boolean_literal(boolean_value); },
  [&](const SExprUnit& sexprunit) -> mlc::String { auto [_w0, _w1] = sexprunit; return literals::gen_unit_literal(); },
  [&](const SExprExtern& sexprextern) -> mlc::String { auto [_w0, _w1] = sexprextern; return literals::gen_extern_placeholder(); },
  [&](const SExprFloat& sexprfloat) -> mlc::String { auto [v, _w0, _w1] = sexprfloat; return literals::gen_float_literal(v); },
  [&](const SExprI64& sexpri64) -> mlc::String { auto [v, _w0, _w1] = sexpri64; return literals::gen_i64_literal(v); },
  [&](const SExprU8& sexpru8) -> mlc::String { auto [v, _w0, _w1] = sexpru8; return literals::gen_u8_literal(v); },
  [&](const SExprUsize& sexprusize) -> mlc::String { auto [v, _w0, _w1] = sexprusize; return literals::gen_usize_literal(v); },
  [&](const SExprChar& sexprchar) -> mlc::String { auto [v, _w0, _w1] = sexprchar; return literals::gen_char_literal(v); },
  [&](const SExprIdent& sexprident) -> mlc::String { auto [name, _w0, _w1] = sexprident; return identifiers::gen_identifier(name, context); },
  [&](const SExprBin& sexprbin) -> mlc::String { auto [operation, left_expr, right_expr, _w0, _w1] = sexprbin; return gen_binary_expr(operation, left_expr, right_expr, context, gen_stmts); },
  [&](const SExprUn& sexprun) -> mlc::String { auto [operation, inner_expr, _w0, _w1] = sexprun; return gen_unary_expr(operation, inner_expr, context, gen_stmts); },
  [&](const SExprCall& sexprcall) -> mlc::String { auto [function_expr, arguments, _w0, _w1] = sexprcall; return gen_call_expr(function_expr, arguments, context, gen_stmts); },
  [&](const SExprMethod& sexprmethod) -> mlc::String { auto [object, method_name, arguments, _w0, _w1] = sexprmethod; return method_gen::gen_method_expr(object, method_name, arguments, context, gen_stmts, eval_expr); },
  [&](const SExprField& sexprfield) -> mlc::String { auto [object, field_name, _w0, _w1] = sexprfield; return gen_field_expr(object, field_name, context, gen_stmts); },
  [&](const SExprIndex& sexprindex) -> mlc::String { auto [object, index_expr, _w0, _w1] = sexprindex; return gen_index_expr(object, index_expr, context, gen_stmts); },
  [&](const SExprIf& sexprif) -> mlc::String { auto [condition, then_expr, else_expr, _w0, _w1] = sexprif; return gen_if_expr(condition, then_expr, else_expr, context, gen_stmts); },
  [&](const SExprBlock& sexprblock) -> mlc::String { auto [statements, result_expr, _w0, _w1] = sexprblock; return gen_block_expr(statements, result_expr, context, gen_stmts); },
  [&](const SExprWhile& sexprwhile) -> mlc::String { auto [condition, statements, _w0, _w1] = sexprwhile; return gen_while_expr(condition, statements, context, gen_stmts); },
  [&](const SExprFor& sexprfor) -> mlc::String { auto [variable, iterator, statements, _w0, _w1] = sexprfor; return gen_for_expr(variable, iterator, statements, context, gen_stmts); },
  [&](const SExprMatch& sexprmatch) -> mlc::String { auto [subject, arms, _w0, _w1] = sexprmatch; return match_gen::gen_match(subject, arms, context, gen_stmts, eval_expr); },
  [&](const SExprRecord& sexprrecord) -> mlc::String { auto [type_name, field_values, expr_type, _w0] = sexprrecord; return record_gen::gen_record_expr(type_name, field_values, expr_type, context, gen_stmts, eval_expr); },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> mlc::String { auto [type_name, base_expr, overrides, _w0, _w1] = sexprrecordupdate; return record_gen::gen_record_update_expr(type_name, base_expr, overrides, context, gen_stmts, eval_expr); },
  [&](const SExprArray& sexprarray) -> mlc::String { auto [elements, _w0, _w1] = sexprarray; return gen_array_expr(elements, context, gen_stmts); },
  [&](const SExprTuple& sexprtuple) -> mlc::String { auto [elements, _w0, _w1] = sexprtuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int ki = 0;
  while (ki < elements.size()){
{
parts.push_back(eval_expr(elements[ki], context, gen_stmts));
ki = ki + 1;
}
}
  return mlc::String("std::make_tuple(") + parts.join(mlc::String(", ")) + mlc::String(")");
 }(); },
  [&](const SExprQuestion& sexprquestion) -> mlc::String { auto [inner_expr, _w0, _w1] = sexprquestion; return gen_question_expr(inner_expr, context, gen_stmts); },
  [&](const SExprLambda& sexprlambda) -> mlc::String { auto [parameters, body_expr, _w0, _w1] = sexprlambda; return gen_lambda_expr(parameters, body_expr, context, gen_stmts); },
  [&](const SExprWith& sexprwith) -> mlc::String { auto [resource, binder, stmts, _w0, _w1] = sexprwith; return [&]() -> mlc::String { 
  mlc::String resource_code = eval_expr(resource, context, gen_stmts);
  mlc::String body_code = gen_stmts(stmts, context);
  return expr::with_block_statement(resource_code, binder, body_code);
 }(); }
}, (*expr)._);}

} // namespace expr_eval
