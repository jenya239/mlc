#ifndef RECORD_LIT_MERGE_HPP
#define RECORD_LIT_MERGE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"

namespace record_lit_merge {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> collect_explicit_field_values_flat(mlc::Array<ast::RecordLitPart> parts) noexcept;

bool record_literal_contains_spread(mlc::Array<ast::RecordLitPart> parts) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_record_literal_parts_to_field_values(mlc::String type_name, mlc::Array<ast::RecordLitPart> parts, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_missing_fields_for_ordered_record(mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept;

} // namespace record_lit_merge

#endif // RECORD_LIT_MERGE_HPP
