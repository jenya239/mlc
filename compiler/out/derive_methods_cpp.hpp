#ifndef DERIVE_METHODS_CPP_HPP
#define DERIVE_METHODS_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "hash_stmt_helpers.hpp"

namespace derive_methods_cpp {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_display_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_eq_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_ord_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_hash_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_methods_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;

} // namespace derive_methods_cpp

#endif // DERIVE_METHODS_CPP_HPP
