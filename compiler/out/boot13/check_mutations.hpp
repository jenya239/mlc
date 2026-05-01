#ifndef CHECK_MUTATIONS_HPP
#define CHECK_MUTATIONS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "names.hpp"

namespace check_mutations {

bool scope_has(mlc::Array<mlc::String> scope, mlc::String name) noexcept;
mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_mutation_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_mutation_stmts_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept;

} // namespace check_mutations

#endif // CHECK_MUTATIONS_HPP
