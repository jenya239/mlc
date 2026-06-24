#ifndef HOVER_HPP
#define HOVER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "registry.hpp"
#include "param_destructure_expand.hpp"
#include "partial_application_desugar.hpp"
#include "check_context.hpp"
#include "infer.hpp"
#include "infer_expr_ident.hpp"
#include "semantic_type_structure.hpp"
#include "symbols.hpp"

namespace hover {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String resolve_hover_type_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept;

} // namespace hover

#endif // HOVER_HPP
