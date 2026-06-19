#ifndef TYPE_INDEX_HPP
#define TYPE_INDEX_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "decl_index.hpp"

namespace type_index {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program program) noexcept;

} // namespace type_index

#endif // TYPE_INDEX_HPP
