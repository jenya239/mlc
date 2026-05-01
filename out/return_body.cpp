#define main mlc_user_main
#include "return_body.hpp"

namespace return_body {

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprIf& sExprIf) { auto [condition, then_expr, else_expr, __3, __4] = sExprIf; return [&]() {
auto output = expr::if_brace_block(eval::gen_expr(condition, context), gen_return_body(then_expr, context));
auto else_string = std::visit(overloaded{[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return mlc::String("", 0); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [__0, __1, __2, __3, __4] = sExprIf; return expr::else_fragment_raw(gen_return_if_stmt(else_expr, context)); },
[&](const auto& __v) { return expr::else_brace_block(gen_return_body(else_expr, context)); }
}, (*else_expr));
return expr::fragment_with_newline(expr::prefix_with_optional_suffix(output, else_string));
}(); },
[&](const auto& __v) { return expr::if_always_true_block(gen_return_body(expr, context)); }
}, (*expr));
}
mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expr, __2, __3] = sExprBlock; return [&]() {
auto final_context = statement_context::stmts_final_ctx(statements, context);
auto statements_code = eval::gen_stmts_str(statements, context);
return std::visit(overloaded{[&](const semantic_ir::SExprQuestion& sExprQuestion) { auto [inner_expr, __1, __2] = sExprQuestion; return [&]() {
auto try_identifier = mlc::String("__try_ret", 9);
return (statements_code + eval::gen_try_unwrap(inner_expr, final_context, try_identifier, expr::return_try_ok_field0_statement(try_identifier)));
}(); },
[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return statements_code; },
[&](const semantic_ir::SExprIf& sExprIf) { auto [__0, __1, __2, __3, __4] = sExprIf; return (statements_code + gen_return_body(result_expr, final_context)); },
[&](const semantic_ir::SExprMatch& sExprMatch) { auto [__0, __1, __2, __3] = sExprMatch; return (statements_code + gen_return_body(result_expr, final_context)); },
[&](const semantic_ir::SExprBlock& sExprBlock) { auto [__0, __1, __2, __3] = sExprBlock; return (statements_code + gen_return_body(result_expr, final_context)); },
[&](const auto& __v) { return (statements_code + expr::return_line(eval::gen_expr(result_expr, final_context))); }
}, (*result_expr));
}(); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [condition, then_expr, else_expr, __3, __4] = sExprIf; return [&]() {
auto output = expr::if_brace_block(eval::gen_expr(condition, context), gen_return_body(then_expr, context));
auto else_string = std::visit(overloaded{[&](const semantic_ir::SExprUnit& sExprUnit) { auto [__0, __1] = sExprUnit; return mlc::String("", 0); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [__0, __1, __2, __3, __4] = sExprIf; return expr::else_fragment_raw(gen_return_if_stmt(else_expr, context)); },
[&](const auto& __v) { return expr::else_brace_block(gen_return_body(else_expr, context)); }
}, (*else_expr));
return expr::fragment_with_newline(expr::prefix_with_optional_suffix(output, else_string));
}(); },
[&](const semantic_ir::SExprQuestion& sExprQuestion) { auto [inner_expr, __1, __2] = sExprQuestion; return [&]() {
auto try_identifier = mlc::String("__try_ret", 9);
return eval::gen_try_unwrap(inner_expr, context, try_identifier, expr::return_try_ok_field0_statement(try_identifier));
}(); },
[&](const auto& __v) { return expr::return_line(eval::gen_expr(expr, context)); }
}, (*expr));
}
mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept{
return gen_return_body(body_expr, context);
}

} // namespace return_body
