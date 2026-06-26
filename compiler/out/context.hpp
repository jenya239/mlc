#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_naming.hpp"
#include "decl_index.hpp"
#include "decl_index.hpp"
#include "load_item.hpp"
#include "type_index.hpp"
#include "ctor_info.hpp"
#include "param_analysis.hpp"

namespace context {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct StructUsingEntry {mlc::String alias;mlc::String type_cpp;};

struct StructUsingData {mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>> entries;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines;};

struct CodegenContext {mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> field_order_index;mlc::String namespace_prefix;mlc::HashMap<mlc::String, mlc::String> qualified;mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes;mlc::String self_type;mlc::HashMap<mlc::String, mlc::String> method_owners;mlc::Array<mlc::String> shared_params;mlc::Array<mlc::String> shared_array_params;mlc::HashMap<mlc::String, mlc::String> array_elem_types;mlc::Array<mlc::String> shared_map_params;mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos;mlc::HashMap<mlc::String, std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_info_index;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::Array<mlc::String> value_params;mlc::Array<mlc::String> match_deref_params;mlc::Array<mlc::String> generic_variants;mlc::HashMap<mlc::String, mlc::Array<context::StructUsingEntry>> struct_using_entries;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines;mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>> type_alias_annotations;int temp_name_counter;std::shared_ptr<registry::Type> enclosing_function_return_type;};

struct TempNameResult {mlc::String name;context::CodegenContext codegen_context;};

struct GenStmtsResult {mlc::Array<mlc::String> parts;int next_try;};

struct GenStmtsWithContext {context::GenStmtsResult statements_parsed;context::CodegenContext codegen_context;};

struct GenStmtResult {mlc::String output;int next_try;context::CodegenContext codegen_context;};

struct GenModuleOut {mlc::String header;mlc::String source;};

struct PrecomputedCtx {mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>> item_index;mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos;mlc::Array<mlc::String> generic_variants;mlc::HashMap<mlc::String, mlc::String> method_owners;mlc::HashMap<mlc::String, mlc::Array<mlc::String>> field_order_index;mlc::HashMap<mlc::String, std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_info_index;mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>> type_alias_annotations;};

int new_temp_name_counter() noexcept;

context::TempNameResult take_temp_name(context::CodegenContext context) noexcept;

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept;

mlc::String qualify_function_callee(mlc::String name, context::CodegenContext context) noexcept;

mlc::String struct_using_line_from_entry(context::StructUsingEntry entry) noexcept;

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context) noexcept;

void mutate_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext& context) noexcept;

mlc::Array<mlc::String> lookup_fields_for_context(context::CodegenContext context, mlc::String name) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info_for_context(context::CodegenContext context, mlc::String constructor_name) noexcept;

context::CodegenContext create_codegen_context(ast::Program program) noexcept;

context::CodegenContext context_with_struct_using_data(context::CodegenContext base, context::StructUsingData data) noexcept;

context::CodegenContext context_with_struct_using_lines(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept;

context::CodegenContext context_with_namespace_alias_prefixes(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept;

context::GenStmtsResult GenStmtsResult_append_stmt(context::GenStmtsResult self, context::GenStmtResult statement_parsed) noexcept;

mlc::String GenStmtsResult_joined_code(context::GenStmtsResult self) noexcept;

mlc::String CodegenContext_resolve(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept;

context::CodegenContext CodegenContext_add_shared(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_shared_array(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_value(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_add_match_deref(context::CodegenContext self, mlc::String name) noexcept;

context::CodegenContext CodegenContext_with_struct_using_data(context::CodegenContext self, context::StructUsingData data) noexcept;

context::CodegenContext CodegenContext_with_struct_using_lines(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept;

context::CodegenContext CodegenContext_with_namespace_alias_prefixes(context::CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept;

context::CodegenContext CodegenContext_with_enclosing_function_return_type(context::CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept;

context::CodegenContext CodegenContext_update_from_statement(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;

} // namespace context

#endif // CONTEXT_HPP
