#ifndef LOWER_PROGRAM_HPP
#define LOWER_PROGRAM_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "mir_types.hpp"
#include "lower_fn.hpp"
#include "mir_passes.hpp"
namespace lower_program {

struct MirLowerAccum {
  mlc::Array<mir_types::MirFunction> functions;
  mlc::Array<mlc::String> errors;
};
mlc::Array<mlc::String> mir_lower_append_errors(mlc::Array<mlc::String> accumulated, mlc::Array<mlc::String> errors) noexcept;
MirLowerAccum mir_lower_append_function(MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
MirLowerAccum mir_lower_append_declaration(MirLowerAccum accum, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
MirLowerAccum mir_lower_declarations(MirLowerAccum accum, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;
MirLowerAccum mir_lower_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;
mir_types::MirProgram build_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;
ast::Result<mir_types::MirProgram, mlc::Array<mlc::String>> build_mir_program_from_semantic_items_checked(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

} // namespace lower_program

#endif // LOWER_PROGRAM_HPP
