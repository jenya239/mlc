#ifndef EXPR_EVAL_HPP
#define EXPR_EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
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

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String eval_expression_else_branch_with_conditional_empty_array_coercion(std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

} // namespace expr_eval

#endif // EXPR_EVAL_HPP
