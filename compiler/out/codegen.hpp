#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace codegen {

struct Expr;
struct Stmt;

struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;mlc::Array<mlc::String> imports;};

struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};

struct CodegenContext {mlc::Array<std::shared_ptr<codegen::FieldOrder>> field_orders;mlc::String ns_prefix;mlc::HashMap<mlc::String, mlc::String> qualified;mlc::String self_type;mlc::HashMap<mlc::String, mlc::String> method_owners;mlc::Array<mlc::String> shared_params;mlc::HashMap<mlc::String, mlc::String> variant_types;};

struct GenModuleOut {mlc::String h;mlc::String c;};

struct PrecomputedCtx {mlc::Array<std::shared_ptr<codegen::FieldOrder>> field_orders;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> item_index;};

codegen::CodegenContext create_codegen_context(ast::Program prog) noexcept;

mlc::String type_to_cpp(codegen::CodegenContext ctx, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_program(ast::Program prog) noexcept;

codegen::PrecomputedCtx precompute(ast::Program prog, mlc::Array<codegen::LoadItem> all_items) noexcept;

codegen::GenModuleOut gen_module(codegen::LoadItem item, mlc::Array<codegen::LoadItem> all_items, ast::Program full_prog, codegen::PrecomputedCtx precomp) noexcept;

} // namespace codegen

#endif // CODEGEN_HPP
