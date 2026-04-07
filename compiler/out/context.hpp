#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_naming.hpp"
#include "decl_index.hpp"
#include "ctor_info.hpp"
#include "param_analysis.hpp"

namespace context {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct CodegenContext {mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders;mlc::String namespace_prefix;mlc::HashMap<mlc::String, mlc::String> qualified;mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes;mlc::String self_type;mlc::HashMap<mlc::String, mlc::String> method_owners;mlc::Array<mlc::String> shared_params;mlc::Array<mlc::String> shared_array_params;mlc::HashMap<mlc::String, mlc::String> array_elem_types;mlc::Array<mlc::String> shared_map_params;mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::Array<mlc::String> value_params;mlc::Array<mlc::String> match_deref_params;mlc::Array<mlc::String> generic_variants;};

struct GenStmtsResult {mlc::String code;int next_try;};

struct GenStmtResult {mlc::String output;int next_try;};

struct GenModuleOut {mlc::String h;mlc::String c;};

struct PrecomputedCtx {mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders;mlc::HashMap<mlc::String, mlc::String> variant_types;mlc::HashMap<mlc::String, std::shared_ptr<decl_index::LoadItem>> item_index;mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> ctor_type_infos;mlc::Array<mlc::String> generic_variants;};

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context) noexcept;

context::CodegenContext create_codegen_context(ast::Program prog) noexcept;

context::CodegenContext context_with_namespace_alias_prefixes(context::CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept;

} // namespace context

#endif // CONTEXT_HPP
