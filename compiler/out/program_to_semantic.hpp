#ifndef PROGRAM_TO_SEMANTIC_HPP
#define PROGRAM_TO_SEMANTIC_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "load_item.hpp"
#include "registry.hpp"
#include "transform_decl.hpp"
#include "trait_param_expand.hpp"

namespace program_to_semantic {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool module_path_is_safe(mlc::String module_path) noexcept;

ast::Result<semantic_ir::SemanticLoadItem, mlc::Array<mlc::String>> program_to_semantic_load_item(ast::Program program, mlc::String module_path) noexcept;

} // namespace program_to_semantic

#endif // PROGRAM_TO_SEMANTIC_HPP
