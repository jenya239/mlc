#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace context {

struct Expr;
struct Stmt;

struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;mlc::Array<mlc::String> imports;};

struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};

struct CtorTypeInfo {mlc::String name;mlc::Array<int> shared_pos;mlc::Array<int> shared_arr_pos;};

struct CodegenContext {mlc::Array<std::shared_ptr<context::FieldOrder>> field_orders;mlc::String namespace_prefix;mlc::HashMap<mlc::String, mlc::String> qualified;mlc::String self_type;mlc::HashMap<mlc::String, mlc::String> method_owners;mlc::Array<mlc::String> shared_params;mlc::Array<mlc::String> shared_array_params;mlc::HashMap<mlc::String, mlc::String> array_elem_types;mlc::Array<mlc::String> shared_map_params;mlc::Array<std::shared_ptr<context::CtorTypeInfo>> ctor_type_infos;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::Array<mlc::String> value_params;mlc::Array<mlc::String> match_deref_params;mlc::Array<mlc::String> generic_variants;};

struct GenStmtsResult {mlc::String code;int next_try;};

struct GenStmtResult {mlc::String output;int next_try;};

struct GenModuleOut {mlc::String h;mlc::String c;};

struct PrecomputedCtx {mlc::Array<std::shared_ptr<context::FieldOrder>> field_orders;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::HashMap<mlc::String, std::shared_ptr<context::LoadItem>> item_index;mlc::Array<std::shared_ptr<context::CtorTypeInfo>> ctor_type_infos;mlc::Array<mlc::String> generic_variants;};

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<context::LoadItem> all_items) noexcept;

bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<context::FieldOrder>> build_field_orders(ast::Program prog) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<context::FieldOrder>> orders, mlc::String name) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<context::LoadItem>> build_item_index(mlc::Array<context::LoadItem> all_items) noexcept;

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<std::shared_ptr<context::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

std::shared_ptr<context::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<context::CtorTypeInfo>> infos, mlc::String cti_name) noexcept;

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept;

bool expr_returns_shared_sum_visit_ptr(std::shared_ptr<ast::Expr> e) noexcept;

context::CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context) noexcept;

context::CodegenContext create_codegen_context(ast::Program prog) noexcept;

} // namespace context

#endif // CONTEXT_HPP
