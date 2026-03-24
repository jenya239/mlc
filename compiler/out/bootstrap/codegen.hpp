#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace codegen {

struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;mlc::Array<mlc::String> imports;};
struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};
struct CtorTypeInfo {mlc::String name;mlc::Array<int> shared_pos;mlc::Array<int> shared_arr_pos;};
struct CodegenContext {mlc::Array<std::shared_ptr<FieldOrder>> field_orders;mlc::String namespace_prefix;mlc::HashMap<mlc::String, mlc::String> qualified;mlc::String self_type;mlc::HashMap<mlc::String, mlc::String> method_owners;mlc::Array<mlc::String> shared_params;mlc::Array<mlc::String> shared_array_params;mlc::HashMap<mlc::String, mlc::String> array_elem_types;mlc::Array<mlc::String> shared_map_params;mlc::Array<std::shared_ptr<CtorTypeInfo>> ctor_type_infos;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::Array<mlc::String> value_params;};
struct GenStmtsResult {mlc::String code;int next_try;};
struct GenStmtResult {mlc::String output;int next_try;};
struct GenModuleOut {mlc::String h;mlc::String c;};
struct PrecomputedCtx {mlc::Array<std::shared_ptr<FieldOrder>> field_orders;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::HashMap<mlc::String, std::shared_ptr<LoadItem>> item_index;mlc::Array<std::shared_ptr<CtorTypeInfo>> ctor_type_infos;};
mlc::String path_to_module_base(mlc::String path) noexcept;
mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept;
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<LoadItem> all_items) noexcept;
mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;
mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<LoadItem>> build_item_index(mlc::Array<LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
CodegenContext create_codegen_context(ast::Program prog) noexcept;
mlc::String context_resolve(CodegenContext context, mlc::String name) noexcept;
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(ast::Program prog) noexcept;
bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept;
bool cpp_keyword(mlc::String name) noexcept;
mlc::String cpp_safe(mlc::String name) noexcept;
mlc::String type_name_to_cpp(CodegenContext context, mlc::String type_name) noexcept;
mlc::String lower_first(mlc::String name) noexcept;
mlc::String type_to_cpp(CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;
mlc::String escape_str(mlc::String input) noexcept;
mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, CodegenContext context) noexcept;
mlc::String map_method(mlc::String method_name) noexcept;
mlc::String map_builtin(mlc::String name) noexcept;
GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext context, int try_counter) noexcept;
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext context) noexcept;
CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext context) noexcept;
mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept;
mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept;
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, CodegenContext context) noexcept;
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, CodegenContext context) noexcept;
mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, CodegenContext context) noexcept;
std::shared_ptr<ast::Expr> find_field_val(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::String name) noexcept;
bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, CodegenContext context) noexcept;
bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, CodegenContext context) noexcept;
mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, CodegenContext context) noexcept;
mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, CodegenContext context) noexcept;
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept;
mlc::String gen_try_unwrap(std::shared_ptr<ast::Expr> inner, CodegenContext context, mlc::String try_id, mlc::String success_line) noexcept;
GenStmtResult gen_stmt_with_try(std::shared_ptr<ast::Stmt> stmt, CodegenContext context, int try_counter) noexcept;
mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, CodegenContext context) noexcept;
mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept;
mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept;
mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept;
mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, CodegenContext context) noexcept;
mlc::String gen_params(CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::String gen_type_decl_fwd(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_single_variant(CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_adt_fwd(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_adt_defs(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_variant_struct(CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;
bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept;
bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept;
mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept;
mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
std::shared_ptr<CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;
std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String cti_name) noexcept;
CodegenContext make_body_context(CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params) noexcept;
CodegenContext context_add_shared(CodegenContext context, mlc::String name) noexcept;
CodegenContext context_add_shared_array(CodegenContext context, mlc::String name) noexcept;
CodegenContext context_add_value(CodegenContext context, mlc::String name) noexcept;
CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, CodegenContext context) noexcept;
mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;
mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;
mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, CodegenContext context) noexcept;
mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, CodegenContext context) noexcept;
mlc::String gen_trait_decl(CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;
mlc::String gen_type_decl(CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, CodegenContext context) noexcept;
mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, CodegenContext context) noexcept;
mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, CodegenContext context) noexcept;
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, CodegenContext context, int phase) noexcept;
mlc::String gen_program(ast::Program prog) noexcept;
mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;
PrecomputedCtx precompute(ast::Program prog, mlc::Array<LoadItem> all_items) noexcept;
GenModuleOut gen_module(LoadItem item, mlc::Array<LoadItem> all_items, ast::Program full_prog, PrecomputedCtx precomp) noexcept;

} // namespace codegen

#endif // CODEGEN_HPP
