#ifndef DUMP_FLAGS_HPP
#define DUMP_FLAGS_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "ast_printer.hpp"
#include "semantic_ir_dump.hpp"
#include "mir_dump.hpp"
#include "lower_program.hpp"
#include "mir_types.hpp"
#include "mir_bootstrap_report.hpp"
namespace dump_flags {

bool dump_label_character_is_safe(mlc::String character) noexcept;
bool dump_label_is_safe(mlc::String label) noexcept;
void emit_dump_ast(ast::Program program, mlc::String label) noexcept;
void emit_dump_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, mlc::String label) noexcept;
void emit_dump_mir_program(mir_types::MirProgram program, mlc::String label) noexcept;
void emit_dump_mir_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, mlc::String label) noexcept;
void emit_mir_bootstrap_report_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, mlc::String label) noexcept;

} // namespace dump_flags

#endif // DUMP_FLAGS_HPP
