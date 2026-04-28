#ifndef AST_BUILDERS_HPP
#define AST_BUILDERS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace ast_builders {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

ast::Program empty_program() noexcept;

std::shared_ptr<ast::TypeExpr> integer_type() noexcept;

std::shared_ptr<ast::TypeExpr> string_type() noexcept;

std::shared_ptr<ast::TypeExpr> boolean_type() noexcept;

std::shared_ptr<ast::TypeExpr> unit_type() noexcept;

std::shared_ptr<ast::TypeExpr> named_type(mlc::String type_name) noexcept;

std::shared_ptr<ast::TypeExpr> array_type(std::shared_ptr<ast::TypeExpr> element_type) noexcept;

std::shared_ptr<ast::TypeExpr> shared_type(std::shared_ptr<ast::TypeExpr> inner_type) noexcept;

std::shared_ptr<ast::TypeExpr> generic_type(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept;

std::shared_ptr<ast::TypeExpr> function_type(mlc::Array<std::shared_ptr<ast::TypeExpr>> parameter_types, std::shared_ptr<ast::TypeExpr> return_type) noexcept;

std::shared_ptr<ast::Expr> integer_expr(int value) noexcept;

std::shared_ptr<ast::Expr> string_expr(mlc::String value) noexcept;

std::shared_ptr<ast::Expr> boolean_expr(bool value) noexcept;

std::shared_ptr<ast::Expr> unit_expr() noexcept;

std::shared_ptr<ast::Expr> identifier_expr(mlc::String name) noexcept;

std::shared_ptr<ast::Expr> binary_expr(mlc::String operation, std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;

std::shared_ptr<ast::Expr> unary_expr(mlc::String operation, std::shared_ptr<ast::Expr> inner_expr) noexcept;

std::shared_ptr<ast::Expr> call_expr(mlc::String function_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

std::shared_ptr<ast::Expr> method_call_expr(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

std::shared_ptr<ast::Expr> field_expr(std::shared_ptr<ast::Expr> object, mlc::String field_name) noexcept;

std::shared_ptr<ast::Expr> index_expr(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index) noexcept;

std::shared_ptr<ast::Expr> if_expr(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expr, std::shared_ptr<ast::Expr> else_expr) noexcept;

std::shared_ptr<ast::Expr> block_expr(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expr) noexcept;

std::shared_ptr<ast::Expr> while_expr(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;

std::shared_ptr<ast::Expr> array_expr(mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;

std::shared_ptr<ast::Expr> record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;

std::shared_ptr<ast::Expr> question_expr(std::shared_ptr<ast::Expr> inner_expr) noexcept;

std::shared_ptr<ast::Expr> lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<ast::Expr> body_expr) noexcept;

std::shared_ptr<ast::FieldVal> field_value(mlc::String field_name, std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Pat> wildcard_pattern() noexcept;

std::shared_ptr<ast::Pat> integer_pattern(int value) noexcept;

std::shared_ptr<ast::Pat> identifier_pattern(mlc::String name) noexcept;

std::shared_ptr<ast::Pat> ctor_pattern(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept;

std::shared_ptr<ast::MatchArm> match_arm(std::shared_ptr<ast::Pat> pattern, std::shared_ptr<ast::Expr> body_expr) noexcept;

std::shared_ptr<ast::Stmt> let_statement(mlc::String binding_name, std::shared_ptr<ast::TypeExpr> type_expr, std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Stmt> let_mut_statement(mlc::String binding_name, std::shared_ptr<ast::TypeExpr> type_expr, std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Stmt> expression_statement(std::shared_ptr<ast::Expr> expr) noexcept;

std::shared_ptr<ast::Stmt> return_statement(std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Param> value_param(mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> parameter_type) noexcept;

std::shared_ptr<ast::Decl> function_decl(mlc::String function_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body_expr) noexcept;

std::shared_ptr<ast::Decl> type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

std::shared_ptr<ast::Decl> exported_decl(std::shared_ptr<ast::Decl> inner_decl) noexcept;

std::shared_ptr<ast::TypeVariant> record_variant(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<ast::TypeVariant> tuple_variant(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept;

std::shared_ptr<ast::TypeVariant> unit_variant(mlc::String variant_name) noexcept;

std::shared_ptr<ast::FieldDef> field_definition(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;

} // namespace ast_builders

#endif // AST_BUILDERS_HPP
