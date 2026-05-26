#ifndef RETURN_BODY_HPP
#define RETURN_BODY_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "eval.hpp"
#include "stmt_cpp.hpp"

namespace return_body {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> gen_return_body_cpp(std::shared_ptr<semantic_ir::SExpr> body_expression, context::CodegenContext context) noexcept;

mlc::String gen_return_body_cpp_as_string(std::shared_ptr<semantic_ir::SExpr> body_expression, context::CodegenContext context) noexcept;

mlc::String gen_fn_body_cpp(std::shared_ptr<semantic_ir::SExpr> body_expression, context::CodegenContext context) noexcept;

} // namespace return_body

#endif // RETURN_BODY_HPP
