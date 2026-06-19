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
#include "compile_commands.hpp"

namespace pipeline {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct ModularCompileInput {mlc::Array<decl_index::LoadItem> load_items;ast::Program full_program;ast::Program entry_program;mlc::String output_directory;bool profile_enabled;bool check_only;bool emit_compile_commands;};

struct CheckedCompileState {mlc::Array<decl_index::LoadItem> load_items;ast::Program full_program;registry::TypeRegistry registry;mlc::String output_directory;bool profile_enabled;};

struct TransformedCompileState {mlc::Array<decl_index::LoadItem> load_items;mlc::Array<semantic_ir::SemanticLoadItem> transformed_items;ast::Program expanded_program;context::PrecomputedCtx precomputed;mlc::String output_directory;bool profile_enabled;};

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState checked_state) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(pipeline::TransformedCompileState transformed_state, bool emit_compile_commands) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(pipeline::ModularCompileInput input) noexcept;

} // namespace pipeline

#endif // PIPELINE_HPP
