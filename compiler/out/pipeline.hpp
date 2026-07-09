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
#include "load_item.hpp"
#include "module.hpp"
#include "cpp_naming.hpp"
#include "profile.hpp"
#include "compile_commands.hpp"
#include "verify_ast.hpp"
#include "verify_semantic_ir.hpp"
#include "preserved_analyses.hpp"
#include "pass_manager.hpp"
#include "dump_flags.hpp"
#include "interpreter.hpp"
namespace pipeline {

struct ModularCompileInput {
  mlc::Array<load_item::LoadItem> load_items;
  ast::Program full_program;
  ast::Program entry_program;
  mlc::String output_directory;
  bool profile_enabled;
  bool check_only;
  bool emit_compile_commands;
  bool verify_each_pass;
  bool dump_ast;
  bool dump_sem;
  bool dump_mir;
  bool mir_bootstrap_report;
  bool time_passes;
  bool run_interpreter;
  bool trace_vm;
};
struct CheckedCompileState {
  mlc::Array<load_item::LoadItem> load_items;
  ast::Program full_program;
  registry::TypeRegistry registry;
  mlc::String output_directory;
  bool profile_enabled;
  bool verify_each_pass;
};
struct TransformedCompileState {
  mlc::Array<load_item::LoadItem> load_items;
  mlc::Array<semantic_ir::SemanticLoadItem> transformed_items;
  ast::Program expanded_program;
  context::PrecomputedCtx precomputed;
  mlc::String output_directory;
  bool profile_enabled;
};
struct PipelineContext {
  ModularCompileInput modular_input;
  bool has_checked;
  CheckedCompileState checked_state;
  bool has_transformed;
  TransformedCompileState transformed_state;
  mlc::Array<mlc::String> produced_keys;
  preserved_analyses::PreservedAnalyses preserved;
};
void write_text_if_changed(mlc::String path, mlc::String content) noexcept;
bool pipeline_wants_timing(ModularCompileInput input) noexcept;
mlc::String modular_input_entry_label(ModularCompileInput input) noexcept;
context::PrecomputedCtx empty_precomputed_context() noexcept;
PipelineContext pipeline_context_new(ModularCompileInput input) noexcept;
void emit_checker_warnings(mlc::Array<mlc::String> warning_lines) noexcept;
ast::Result<CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(ModularCompileInput input) noexcept;
ast::Result<TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(CheckedCompileState checked_state) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(TransformedCompileState transformed_state, bool emit_compile_commands) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(ModularCompileInput input) noexcept;
void maybe_emit_dump_semantic(ModularCompileInput input, PipelineContext context) noexcept;
void maybe_emit_dump_mir(ModularCompileInput input, PipelineContext context) noexcept;
void maybe_emit_mir_bootstrap_report(ModularCompileInput input, PipelineContext context) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> maybe_run_interpreter(ModularCompileInput input, PipelineContext context) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline_with_manager(pass_manager::PassManager manager, ModularCompileInput input) noexcept;
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_pass_manager_descriptors(pass_manager::PassManager manager, PipelineContext context, int pass_index) noexcept;
ast::Result<PipelineContext, mlc::Array<mlc::String>> advance_pipeline_context(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor, PipelineContext context) noexcept;
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_pipeline_pass(pass_manager::PassDescriptor descriptor, PipelineContext context) noexcept;
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_verify_ast_pass(PipelineContext context) noexcept;
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_checker_pipeline_pass(PipelineContext context) noexcept;
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_transform_pipeline_pass(PipelineContext context) noexcept;
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_codegen_pipeline_pass(PipelineContext context) noexcept;

} // namespace pipeline

#endif // PIPELINE_HPP
