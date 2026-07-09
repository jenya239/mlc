#ifndef LOWER_FN_HPP
#define LOWER_FN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "mir_types.hpp"
#include "mir_ids.hpp"

namespace lower_fn {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> lower_semantic_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

} // namespace lower_fn

#endif // LOWER_FN_HPP
