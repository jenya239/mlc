#include "eval.hpp"

#include "semantic_ir.hpp"
#include "context.hpp"
#include "cpp_ast.hpp"
#include "stmt_eval.hpp"
#include "expr_visitor_cpp.hpp"
#include "stmt_cpp.hpp"

namespace eval {

using namespace semantic_ir;
using namespace context;
using namespace cpp_ast;
using namespace stmt_eval;
using namespace expr_visitor_cpp;
using namespace stmt_cpp;

mlc::String eval_expr_gen_stmts_callback(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext codegen_context) noexcept;

mlc::String CodegenContext_gen_stmts_str(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;

mlc::String CodegenContext_gen_expr(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

mlc::String CodegenContext_gen_argument_list(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> expressions) noexcept;

mlc::String CodegenContext_gen_try_unwrap(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, mlc::String try_identifier, mlc::String success_line) noexcept;

context::GenStmtResult CodegenContext_gen_stmt_with_try(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement, int try_counter) noexcept;

context::GenStmtsWithContext CodegenContext_gen_stmts_str_with_try(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, int try_counter) noexcept;

mlc::String gen_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> expressions, context::CodegenContext context) noexcept;

mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept;

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept;

context::GenStmtsWithContext gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String eval_expr_gen_stmts_callback(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext codegen_context) noexcept{return gen_stmts_str(statements, codegen_context);}

mlc::String CodegenContext_gen_stmts_str(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept{return stmt_cpp::print_cpp_statements(stmt_cpp::gen_stmts_cpp(statements, self));}

mlc::String CodegenContext_gen_expr(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return ast_printer::print_expr(expr_visitor_cpp::eval_expr_cpp(expression, self, eval_expr_gen_stmts_callback));}

mlc::String CodegenContext_gen_argument_list(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> expressions) noexcept{return expressions.map([self](std::shared_ptr<semantic_ir::SemanticExpression> expression) mutable { return gen_expr(expression, self); }).join(mlc::String(", "));}

mlc::String CodegenContext_gen_try_unwrap(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, mlc::String try_identifier, mlc::String success_line) noexcept{return stmt_eval::eval_try_unwrap(inner_expression, self, try_identifier, success_line, [](std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext codegen_context) mutable { return gen_expr(expression, codegen_context); });}

context::GenStmtResult CodegenContext_gen_stmt_with_try(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement, int try_counter) noexcept{return stmt_eval::eval_stmt_with_try(statement, self, try_counter, [](std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext codegen_context) mutable { return gen_expr(expression, codegen_context); });}

context::GenStmtsWithContext CodegenContext_gen_stmts_str_with_try(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, int try_counter) noexcept{return stmt_eval::eval_stmts_str_with_try(statements, self, try_counter, [](std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext codegen_context) mutable { return gen_expr(expression, codegen_context); });}

mlc::String gen_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return CodegenContext_gen_expr(context, expression);}

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{return CodegenContext_gen_stmts_str(context, statements);}

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> expressions, context::CodegenContext context) noexcept{return CodegenContext_gen_argument_list(context, expressions);}

mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept{return CodegenContext_gen_try_unwrap(context, inner_expression, try_identifier, success_line);}

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept{return CodegenContext_gen_stmt_with_try(context, statement, try_counter);}

context::GenStmtsWithContext gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept{return CodegenContext_gen_stmts_str_with_try(context, statements, try_counter);}

} // namespace eval
