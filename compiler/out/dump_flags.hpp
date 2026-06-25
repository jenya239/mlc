#ifndef DUMP_FLAGS_HPP
#define DUMP_FLAGS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "ast_printer.hpp"
#include "semantic_ir_dump.hpp"

namespace dump_flags {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool dump_label_is_safe(mlc::String label) noexcept;

void emit_dump_ast(ast::Program program, mlc::String label) noexcept;

void emit_dump_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, mlc::String label) noexcept;

} // namespace dump_flags

#endif // DUMP_FLAGS_HPP
