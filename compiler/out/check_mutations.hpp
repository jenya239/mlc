#ifndef CHECK_MUTATIONS_HPP
#define CHECK_MUTATIONS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace check_mutations {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct MutationCheckResult {mlc::Array<ast::Diagnostic> diagnostics;mlc::Array<mlc::String> mutable_locals;};

check_mutations::MutationCheckResult check_mutation_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_semantic_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept;

} // namespace check_mutations

#endif // CHECK_MUTATIONS_HPP
