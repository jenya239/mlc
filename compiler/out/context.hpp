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

struct StructUsingEntry {
  mlc::String alias;
  mlc::String type_cpp;
};
struct StructUsingData {
  mlc::HashMap<mlc::String, mlc::Array<StructUsingEntry>> entries;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines;
};
struct CodegenContext {
  mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> field_order_index;
  mlc::String namespace_prefix;
  mlc::HashMap<mlc::String, mlc::String> qualified;
  mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes;
  mlc::String self_type;
  mlc::HashMap<mlc::String, mlc::String> method_owners;
  mlc::Array<mlc::String> shared_params;
  mlc::Array<mlc::String> shared_array_params;
  mlc::HashMap<mlc::String, mlc::String> array_elem_types;
  mlc::Array<mlc::String> shared_map_params;
  mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos;
  mlc::HashMap<mlc::String, std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_info_index;
  mlc::HashMap<mlc::String, mlc::String> variant_types;
  mlc::Array<mlc::String> value_params;
  mlc::Array<mlc::String> match_deref_params;
  mlc::Array<mlc::String> generic_variants;
  mlc::HashMap<mlc::String, mlc::Array<StructUsingEntry>> struct_using_entries;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines;
  mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>> type_alias_annotations;
  int temp_name_counter;
  std::shared_ptr<registry::Type> enclosing_function_return_type;
};
struct TempNameResult {
  mlc::String name;
  CodegenContext codegen_context;
};
struct GenStmtsResult {
  mlc::Array<mlc::String> parts;
  int next_try;
};
struct GenStmtsWithContext {
  GenStmtsResult statements_parsed;
  CodegenContext codegen_context;
};
struct GenStmtResult {
  mlc::String output;
  int next_try;
  CodegenContext codegen_context;
};
struct GenModuleOut {
  mlc::String header;
  mlc::String source;
};
struct PrecomputedCtx {
  mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders;
  mlc::HashMap<mlc::String, mlc::String> variant_types;
  mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>> item_index;
  mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos;
  mlc::Array<mlc::String> generic_variants;
  mlc::HashMap<mlc::String, mlc::String> method_owners;
  mlc::HashMap<mlc::String, mlc::Array<mlc::String>> field_order_index;
  mlc::HashMap<mlc::String, std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_info_index;
  mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>> type_alias_annotations;
};
int new_temp_name_counter() noexcept;
TempNameResult take_temp_name(CodegenContext context) noexcept;
GenStmtsResult GenStmtsResult_append_stmt(GenStmtsResult self, GenStmtResult statement_parsed) noexcept;
mlc::String GenStmtsResult_joined_code(GenStmtsResult self) noexcept;
mlc::String context_resolve(CodegenContext context, mlc::String name) noexcept;
mlc::String type_method_namespace_prefix(mlc::String mangled_name, CodegenContext context) noexcept;
mlc::String qualify_function_callee(mlc::String name, CodegenContext context) noexcept;
mlc::Array<mlc::String> collect_pattern_bind_names_for_context(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String>& accumulator) noexcept;
mlc::String struct_using_line_from_entry(StructUsingEntry entry) noexcept;
mlc::String CodegenContext_resolve(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_for_type_body(CodegenContext self, mlc::String type_name) noexcept;
CodegenContext CodegenContext_add_shared(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_shared_array(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_value(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_match_deref(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_with_struct_using_data(CodegenContext self, StructUsingData data) noexcept;
CodegenContext CodegenContext_with_struct_using_lines(CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept;
CodegenContext CodegenContext_with_namespace_alias_prefixes(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept;
CodegenContext CodegenContext_with_enclosing_function_return_type(CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept;
CodegenContext CodegenContext_update_from_statement(CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;
CodegenContext make_body_context(CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;
CodegenContext context_add_shared(CodegenContext context, mlc::String name) noexcept;
CodegenContext context_add_shared_array(CodegenContext context, mlc::String name) noexcept;
CodegenContext context_add_value(CodegenContext context, mlc::String name) noexcept;
CodegenContext context_add_match_deref(CodegenContext context, mlc::String name) noexcept;
CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, CodegenContext context) noexcept;
void push_pattern_bind_names_to_value_params(std::shared_ptr<ast::Pattern> pattern, CodegenContext& context) noexcept;
void mutate_context_for_let_const(mlc::String binding_name, CodegenContext& context) noexcept;
void mutate_context_for_let(mlc::String binding_name, std::shared_ptr<registry::Type> value_type, CodegenContext& context) noexcept;
void mutate_context_noop() noexcept;
void mutate_context_from_statement(std::shared_ptr<semantic_ir::SemanticStatement> statement, CodegenContext& context) noexcept;
mlc::Array<mlc::String> lookup_fields_for_context(CodegenContext context, mlc::String name) noexcept;
std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info_for_context(CodegenContext context, mlc::String constructor_name) noexcept;
CodegenContext create_codegen_context(ast::Program program) noexcept;
CodegenContext context_with_struct_using_data(CodegenContext base, StructUsingData data) noexcept;
CodegenContext context_with_struct_using_lines(CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept;
CodegenContext context_with_namespace_alias_prefixes(CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept;
GenStmtsResult GenStmtsResult_append_stmt(GenStmtsResult self, GenStmtResult statement_parsed) noexcept;
mlc::String GenStmtsResult_joined_code(GenStmtsResult self) noexcept;
mlc::String CodegenContext_resolve(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_for_type_body(CodegenContext self, mlc::String type_name) noexcept;
CodegenContext CodegenContext_add_shared(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_shared_array(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_value(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_add_match_deref(CodegenContext self, mlc::String name) noexcept;
CodegenContext CodegenContext_with_struct_using_data(CodegenContext self, StructUsingData data) noexcept;
CodegenContext CodegenContext_with_struct_using_lines(CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept;
CodegenContext CodegenContext_with_namespace_alias_prefixes(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept;
CodegenContext CodegenContext_with_enclosing_function_return_type(CodegenContext self, std::shared_ptr<registry::Type> return_type) noexcept;
CodegenContext CodegenContext_update_from_statement(CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement) noexcept;

} // namespace context

#endif // CONTEXT_HPP
