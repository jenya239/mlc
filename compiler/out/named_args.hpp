#ifndef NAMED_ARGS_HPP
#define NAMED_ARGS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace named_args {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct ResolvedNamedArguments {mlc::Array<std::shared_ptr<ast::Expr>> expressions;mlc::Array<ast::Diagnostic> errors;};

bool call_arguments_contain_named_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

int find_parameter_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> strip_named_argument_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

named_args::ResolvedNamedArguments resolve_named_call_arguments_for_infer(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> resolve_named_call_arguments_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept;

} // namespace named_args

#endif // NAMED_ARGS_HPP
