#ifndef RETURN_BODY_HPP
#define RETURN_BODY_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "eval.hpp"
#include "stmt_cpp.hpp"
#include "expr_visitor_cpp.hpp"

namespace return_body {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;

} // namespace return_body

#endif // RETURN_BODY_HPP
