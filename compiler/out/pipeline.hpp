#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "check.hpp"
#include "registry.hpp"
#include "trait_param_expand.hpp"
#include "param_destructure_expand.hpp"
#include "transform_decl.hpp"
#include "semantic_ir.hpp"
#include "decl_index.hpp"
#include "module.hpp"
#include "cpp_naming.hpp"
#include "profile.hpp"

namespace pipeline {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct ModularCompileInput {mlc::Array<decl_index::LoadItem> load_items;ast::Program full_program;ast::Program entry_program;mlc::String output_directory;bool profile_enabled;};

struct CheckedCompileState {mlc::Array<decl_index::LoadItem> load_items;ast::Program full_program;registry::TypeRegistry registry;mlc::String output_directory;bool profile_enabled;};

struct TransformedCompileState {mlc::Array<decl_index::LoadItem> load_items;mlc::Array<semantic_ir::SLoadItem> transformed_items;ast::Program expanded_program;context::PrecomputedCtx precomputed;mlc::String output_directory;bool profile_enabled;};

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState state) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(pipeline::TransformedCompileState state) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(pipeline::ModularCompileInput input) noexcept;

} // namespace pipeline

#endif // PIPELINE_HPP
