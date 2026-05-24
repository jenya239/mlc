#include "expr_eval.hpp"

#include "semantic_ir.hpp"
#include "context.hpp"
#include "expr_visitor_string.hpp"

namespace expr_eval {

using namespace semantic_ir;
using namespace context;
using namespace expr_visitor_string;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expressions.map([context, gen_stmts](std::shared_ptr<semantic_ir::SExpr> expression) mutable { return eval_expr(expression, context, gen_stmts); }).join(mlc::String(", "));}

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> mlc::String { auto [_w0, _w1, _w2] = sexprint; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprStr& sexprstr) -> mlc::String { auto [_w0, _w1, _w2] = sexprstr; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprBool& sexprbool) -> mlc::String { auto [_w0, _w1, _w2] = sexprbool; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprUnit& sexprunit) -> mlc::String { auto [_w0, _w1] = sexprunit; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprFloat& sexprfloat) -> mlc::String { auto [_w0, _w1, _w2] = sexprfloat; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprI64& sexpri64) -> mlc::String { auto [_w0, _w1, _w2] = sexpri64; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprU8& sexpru8) -> mlc::String { auto [_w0, _w1, _w2] = sexpru8; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprUsize& sexprusize) -> mlc::String { auto [_w0, _w1, _w2] = sexprusize; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprChar& sexprchar) -> mlc::String { auto [_w0, _w1, _w2] = sexprchar; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprExtern& sexprextern) -> mlc::String { auto [semantic_type, _w0] = sexprextern; return expr_visitor_string::gen_extern_via_visitor(semantic_type); },
  [&](const SExprIdent& sexprident) -> mlc::String { auto [_w0, _w1, _w2] = sexprident; return expr_visitor_string::gen_expr_via_string_visitor(expr, context); },
  [&](const SExprBin& sexprbin) -> mlc::String { auto [operation, left_expr, right_expr, _w0, _w1] = sexprbin; return expr_visitor_string::gen_binary_via_visitor(operation, left_expr, right_expr, context, gen_stmts, eval_expr); },
  [&](const SExprUn& sexprun) -> mlc::String { auto [operation, inner_expr, _w0, _w1] = sexprun; return expr_visitor_string::gen_unary_via_visitor(operation, inner_expr, context, gen_stmts, eval_expr); },
  [&](const SExprCall& sexprcall) -> mlc::String { auto [function_expression, arguments, call_parameter_mutability_flags, _w0, _w1] = sexprcall; return expr_visitor_string::gen_call_via_visitor(function_expression, arguments, call_parameter_mutability_flags, context, gen_stmts, eval_expr); },
  [&](const SExprMethod& sexprmethod) -> mlc::String { auto [method_object, method_name, arguments, receiver_and_parameter_mut_flags, _w0, _w1] = sexprmethod; return expr_visitor_string::gen_method_via_visitor(method_object, method_name, arguments, receiver_and_parameter_mut_flags, context, gen_stmts, eval_expr); },
  [&](const SExprField& sexprfield) -> mlc::String { auto [object, field_name, _w0, _w1] = sexprfield; return expr_visitor_string::gen_field_via_visitor(object, field_name, context, gen_stmts, eval_expr); },
  [&](const SExprIndex& sexprindex) -> mlc::String { auto [object, index_expr, _w0, _w1] = sexprindex; return expr_visitor_string::gen_index_via_visitor(object, index_expr, context, gen_stmts, eval_expr); },
  [&](const SExprIf& sexprif) -> mlc::String { auto [condition, then_expr, else_expr, if_semantic_type, _w0] = sexprif; return expr_visitor_string::gen_if_via_visitor(condition, then_expr, else_expr, if_semantic_type, context, gen_stmts, eval_expr); },
  [&](const SExprBlock& sexprblock) -> mlc::String { auto [statements, result_expr, _w0, _w1] = sexprblock; return expr_visitor_string::gen_block_via_visitor(statements, result_expr, context, gen_stmts, eval_expr); },
  [&](const SExprWhile& sexprwhile) -> mlc::String { auto [condition, statements, _w0, _w1] = sexprwhile; return expr_visitor_string::gen_while_via_visitor(condition, statements, context, gen_stmts, eval_expr); },
  [&](const SExprFor& sexprfor) -> mlc::String { auto [variable, iterator, statements, _w0, _w1] = sexprfor; return expr_visitor_string::gen_for_via_visitor(variable, iterator, statements, context, gen_stmts, eval_expr); },
  [&](const SExprMatch& sexprmatch) -> mlc::String { auto [subject, arms, match_semantic_type, _w0] = sexprmatch; return expr_visitor_string::gen_match_via_visitor(subject, arms, match_semantic_type, context, gen_stmts, eval_expr); },
  [&](const SExprRecord& sexprrecord) -> mlc::String { auto [type_name, field_values, expr_type, _w0] = sexprrecord; return expr_visitor_string::gen_record_via_visitor(type_name, field_values, expr_type, context, gen_stmts, eval_expr); },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> mlc::String { auto [type_name, base_expr, overrides, _w0, _w1] = sexprrecordupdate; return expr_visitor_string::gen_record_update_via_visitor(type_name, base_expr, overrides, context, gen_stmts, eval_expr); },
  [&](const SExprArray& sexprarray) -> mlc::String { auto [elements, container_semantic_type, _w0] = sexprarray; return expr_visitor_string::gen_array_via_visitor(elements, container_semantic_type, context, gen_stmts, eval_expr); },
  [&](const SExprTuple& sexprtuple) -> mlc::String { auto [elements, _w0, _w1] = sexprtuple; return expr_visitor_string::gen_tuple_via_visitor(elements, context, gen_stmts, eval_expr); },
  [&](const SExprQuestion& sexprquestion) -> mlc::String { auto [inner_expr, _w0, _w1] = sexprquestion; return expr_visitor_string::gen_question_via_visitor(inner_expr, context, gen_stmts, eval_expr); },
  [&](const SExprLambda& sexprlambda) -> mlc::String { auto [parameters, body_expr, lambda_annotation_type, _w0] = sexprlambda; return expr_visitor_string::gen_lambda_via_visitor(parameters, body_expr, lambda_annotation_type, context, gen_stmts, eval_expr); },
  [&](const SExprWith& sexprwith) -> mlc::String { auto [resource, binder, stmts, _w0, _w1] = sexprwith; return expr_visitor_string::gen_with_via_visitor(resource, binder, stmts, context, gen_stmts, eval_expr); }
}, (*expr)._);}

} // namespace expr_eval
