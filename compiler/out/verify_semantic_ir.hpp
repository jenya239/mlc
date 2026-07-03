#ifndef VERIFY_SEMANTIC_IR_HPP
#define VERIFY_SEMANTIC_IR_HPP

#include "mlc.hpp"
#include <variant>
#include "registry.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
namespace verify_semantic_ir {

mlc::Array<mlc::String> verify_semantic_append_errors(mlc::Array<mlc::String> accumulator, mlc::Array<mlc::String> more) noexcept;
bool semantic_type_is_resolved(std::shared_ptr<registry::Type> type_value) noexcept;
mlc::Array<mlc::String> verify_semantic_empty_name_errors(mlc::String name, mlc::String message) noexcept;
mlc::Array<mlc::String> verify_semantic_errors_if(bool condition, mlc::Array<mlc::String> errors) noexcept;
mlc::Array<mlc::String> verify_semantic_span_errors(mlc::String label, ast::Span span) noexcept;
mlc::Array<mlc::String> verify_semantic_type_errors(mlc::String label, std::shared_ptr<registry::Type> type_value) noexcept;
mlc::Array<mlc::String> verify_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
mlc::Array<mlc::String> verify_semantic_statement(std::shared_ptr<semantic_ir::SemanticStatement> semantic_statement) noexcept;
mlc::Array<mlc::String> verify_semantic_declaration_members(mlc::Array<mlc::String> errors, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> members) noexcept;
mlc::Array<mlc::String> verify_semantic_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::Array<mlc::String> verify_semantic_ir_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

} // namespace verify_semantic_ir

#endif // VERIFY_SEMANTIC_IR_HPP
