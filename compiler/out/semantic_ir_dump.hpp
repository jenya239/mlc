#ifndef SEMANTIC_IR_DUMP_HPP
#define SEMANTIC_IR_DUMP_HPP

#include "mlc.hpp"
#include <variant>
#include "semantic_ir.hpp"
namespace semantic_ir_dump {

mlc::String semantic_declaration_label(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::String print_semantic_load_item(semantic_ir::SemanticLoadItem item) noexcept;
mlc::String print_semantic_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

} // namespace semantic_ir_dump

#endif // SEMANTIC_IR_DUMP_HPP
