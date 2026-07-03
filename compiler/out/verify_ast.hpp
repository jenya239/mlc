#ifndef VERIFY_AST_HPP
#define VERIFY_AST_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
namespace verify_ast {

mlc::Array<mlc::String> verify_ast_empty_name_errors(mlc::String name, mlc::String message) noexcept;
mlc::Array<mlc::String> verify_ast_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept;
mlc::Array<mlc::String> verify_ast_span_errors(mlc::String label, ast::Span span) noexcept;
mlc::Array<mlc::String> verify_ast_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept;
mlc::Array<mlc::String> verify_ast_pattern(std::shared_ptr<ast::Pattern> pattern) noexcept;
mlc::Array<mlc::String> verify_ast_record_lit_part(mlc::Array<mlc::String> errors, ast::RecordLitPart part) noexcept;
mlc::Array<mlc::String> verify_ast_expression(std::shared_ptr<ast::Expr> expression) noexcept;
mlc::Array<mlc::String> verify_ast_statement(std::shared_ptr<ast::Stmt> statement) noexcept;
mlc::Array<mlc::String> verify_ast_parameter(std::shared_ptr<ast::Param> parameter) noexcept;
mlc::Array<mlc::String> verify_ast_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<ast::Decl>> members) noexcept;
mlc::Array<mlc::String> verify_ast_declaration(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<mlc::String> verify_ast_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> verify_ast_program(ast::Program program) noexcept;

} // namespace verify_ast

#endif // VERIFY_AST_HPP
