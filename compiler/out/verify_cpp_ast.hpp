#ifndef VERIFY_CPP_AST_HPP
#define VERIFY_CPP_AST_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"

namespace verify_cpp_ast {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool cpp_identifier_is_safe(mlc::String name) noexcept;

bool cpp_include_path_is_safe(mlc::String path) noexcept;

mlc::Array<mlc::String> verify_cpp_program(cpp_ast::CppProgram program) noexcept;

} // namespace verify_cpp_ast

#endif // VERIFY_CPP_AST_HPP
