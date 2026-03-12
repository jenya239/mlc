#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace codegen {

struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;mlc::Array<mlc::String> imports;};
struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};
struct CodegenContext {mlc::Array<std::shared_ptr<FieldOrder>> field_orders;mlc::String ns_prefix;mlc::HashMap<mlc::String, mlc::String> qualified;mlc::String self_type;mlc::HashMap<mlc::String, mlc::String> method_owners;mlc::Array<mlc::String> shared_params;mlc::HashMap<mlc::String, mlc::String> variant_types;};
struct GenModuleOut {mlc::String h;mlc::String c;};
mlc::String path_to_module_base(mlc::String path) noexcept;
mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept;
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> q, mlc::String imp_path, mlc::Array<LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<LoadItem> all_items) noexcept;
mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;
mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> m) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_method_owners_for(LoadItem item, mlc::Array<LoadItem> all_items) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
CodegenContext create_codegen_context(ast::Program prog) noexcept;
mlc::String ctx_resolve(CodegenContext ctx, mlc::String name) noexcept;
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(ast::Program prog) noexcept;
bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept;
bool cpp_keyword(mlc::String name) noexcept;
mlc::String cpp_safe(mlc::String name) noexcept;
mlc::String lower_first(mlc::String name) noexcept;
mlc::String type_to_cpp(CodegenContext ctx, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;
mlc::String escape_str(mlc::String input) noexcept;
mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, CodegenContext ctx) noexcept;
mlc::String map_method(mlc::String method_name) noexcept;
mlc::String map_builtin(mlc::String name) noexcept;
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext ctx) noexcept;
mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept;
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, CodegenContext ctx) noexcept;
mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept;
std::shared_ptr<ast::Expr> find_field_val(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::String name) noexcept;
bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, CodegenContext ctx) noexcept;
mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, CodegenContext ctx) noexcept;
mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, CodegenContext ctx) noexcept;
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept;
mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, CodegenContext ctx) noexcept;
mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, CodegenContext ctx) noexcept;
mlc::String gen_params(CodegenContext ctx, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::String gen_type_decl_fwd(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_single_variant(CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_adt_fwd(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_adt_defs(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_variant_struct(CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;
mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, CodegenContext ctx) noexcept;
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, CodegenContext ctx) noexcept;
mlc::String gen_type_decl(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, CodegenContext ctx) noexcept;
mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, CodegenContext ctx) noexcept;
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, CodegenContext ctx, int phase) noexcept;
mlc::String gen_program(ast::Program prog) noexcept;
mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;
GenModuleOut gen_module(LoadItem item, mlc::Array<LoadItem> all_items, ast::Program full_prog) noexcept;

} // namespace codegen

#endif // CODEGEN_HPP
