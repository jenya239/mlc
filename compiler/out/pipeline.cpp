#include "pipeline.hpp"

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

using namespace ast;
using namespace check;
using namespace registry;
using namespace trait_param_expand;
using namespace param_destructure_expand;
using namespace transform_decl;
using namespace semantic_ir;
using namespace load_item;
using namespace module;
using namespace cpp_naming;
using namespace profile;
using namespace compile_commands;
using namespace verify_ast;
using namespace verify_semantic_ir;
using namespace preserved_analyses;
using namespace pass_manager;
using namespace dump_flags;
using namespace interpreter;
using namespace ast_tokens;

void write_text_if_changed(mlc::String path, mlc::String content) noexcept;

bool pipeline_wants_timing(pipeline::ModularCompileInput input) noexcept;

mlc::String modular_input_entry_label(pipeline::ModularCompileInput input) noexcept;

context::PrecomputedCtx empty_precomputed_context() noexcept;

pipeline::PipelineContext pipeline_context_new(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState checked_state) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(pipeline::TransformedCompileState transformed_state, bool emit_compile_commands) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(pipeline::ModularCompileInput input) noexcept;

void maybe_emit_dump_semantic(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept;

void maybe_emit_dump_mir(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept;

void maybe_emit_mir_bootstrap_report(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> maybe_run_interpreter(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline_with_manager(pass_manager::PassManager manager, pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_pass_manager_descriptors(pass_manager::PassManager manager, pipeline::PipelineContext context, int pass_index) noexcept;

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> advance_pipeline_context(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor, pipeline::PipelineContext context) noexcept;

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_pipeline_pass(pass_manager::PassDescriptor descriptor, pipeline::PipelineContext context) noexcept;

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_verify_ast_pass(pipeline::PipelineContext context) noexcept;

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_checker_pipeline_pass(pipeline::PipelineContext context) noexcept;

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_transform_pipeline_pass(pipeline::PipelineContext context) noexcept;

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_codegen_pipeline_pass(pipeline::PipelineContext context) noexcept;

void write_text_if_changed(mlc::String path, mlc::String content) noexcept{
if (!(mlc::file::exists(path) && mlc::file::read_to_string(path) == content)){
mlc::file::write_string(path, content);
}
}

bool pipeline_wants_timing(pipeline::ModularCompileInput input) noexcept{return input.profile_enabled || input.time_passes;}

mlc::String modular_input_entry_label(pipeline::ModularCompileInput input) noexcept{
return input.load_items.size() > 0 ? input.load_items[input.load_items.size() - 1].path : mlc::String("program");
}

context::PrecomputedCtx empty_precomputed_context() noexcept{return context::PrecomputedCtx{{}, mlc::HashMap<mlc::String, mlc::String>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, std::shared_ptr<load_item::LoadItem>>(), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>(), mlc::HashMap<mlc::String, std::shared_ptr<ctor_info::CtorTypeInfo>>(), mlc::HashMap<mlc::String, std::shared_ptr<ast::TypeExpr>>(), mlc::HashMap<mlc::String, mlc::Array<mlc::String>>()};}

pipeline::PipelineContext pipeline_context_new(pipeline::ModularCompileInput input) noexcept{return pipeline::PipelineContext{input, false, pipeline::CheckedCompileState{input.load_items, input.full_program, registry::empty_registry(), input.output_directory, pipeline_wants_timing(input), input.verify_each_pass}, false, pipeline::TransformedCompileState{input.load_items, {}, input.full_program, empty_precomputed_context(), input.output_directory, pipeline_wants_timing(input)}, mlc::Array<mlc::String>{mlc::String("modular_input")}, preserved_analyses::preserved_analyses_empty()};}

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept{
bool timing_enabled = pipeline_wants_timing(input);
profile::profile_maybe_begin(timing_enabled, mlc::String("check"));
auto __try_check_output = check::check_with_context(input.entry_program, input.full_program);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_check_output)) return ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_check_output));
check::CheckOut check_output = std::get<ast::Ok<check::CheckOut>>(__try_check_output).field0;

profile::profile_maybe_end(timing_enabled, mlc::String("check"));
return ast::Ok<pipeline::CheckedCompileState>(pipeline::CheckedCompileState{input.load_items, input.full_program, check_output.registry, input.output_directory, timing_enabled, input.verify_each_pass});
}

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState checked_state) noexcept{
profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("expand_destructure"));
ast::Program expanded_program = param_destructure_expand::expand_parameter_destructuring_in_program(checked_state.full_program);
profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("expand_destructure"));
trait_param_expand::TraitNominalMaps trait_maps = trait_param_expand::build_trait_nominal_maps(expanded_program);
profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("transform"));
mlc::Array<semantic_ir::SemanticLoadItem> transformed_items = transform_decl::transform_load_items(checked_state.load_items, checked_state.registry, trait_maps);
profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("transform"));
if (checked_state.verify_each_pass){
{
mlc::Array<mlc::String> semantic_errors = verify_semantic_ir::verify_semantic_ir_load_items(transformed_items);
if (semantic_errors.size() > 0){
return ast::Err<mlc::Array<mlc::String>>(semantic_errors);
}
}
}
profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("precompute"));
context::PrecomputedCtx precomputed = module::precompute(expanded_program, checked_state.load_items);
profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("precompute"));
return ast::Ok<pipeline::TransformedCompileState>(pipeline::TransformedCompileState{checked_state.load_items, transformed_items, expanded_program, precomputed, checked_state.output_directory, checked_state.profile_enabled});
}

ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(pipeline::TransformedCompileState transformed_state, bool emit_compile_commands) noexcept{
profile::profile_maybe_begin(transformed_state.profile_enabled, mlc::String("codegen"));
mlc::Array<mlc::String> implementation_paths = {};
int index = 0;
while (index < transformed_state.transformed_items.size()){
{
semantic_ir::SemanticLoadItem transformed_load_item = transformed_state.transformed_items[index];
context::GenModuleOut generated_output = module::gen_module(transformed_load_item, transformed_state.load_items, transformed_state.expanded_program, transformed_state.precomputed);
mlc::String module_base = cpp_naming::path_to_module_base(transformed_load_item.path);
mlc::String output_directory_prefix = transformed_state.output_directory.length() > 0 ? transformed_state.output_directory + mlc::String("/") : mlc::String("");
mlc::String header_path = output_directory_prefix + module_base + mlc::String(".hpp");
mlc::String implementation_path = output_directory_prefix + module_base + mlc::String(".cpp");
write_text_if_changed(header_path, generated_output.header);
write_text_if_changed(implementation_path, generated_output.source);
implementation_paths.push_back(implementation_path);
index = index + 1;
}
}
if (emit_compile_commands){
{
compile_commands::write_compile_commands_file(transformed_state.output_directory, implementation_paths);
}
}
profile::profile_maybe_end(transformed_state.profile_enabled, mlc::String("codegen"));
return ast::Ok<mlc::String>(mlc::String(""));
}

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(pipeline::ModularCompileInput input) noexcept{
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<pass_manager::PassManager>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [manager] = ok; return run_modular_compiler_pipeline_with_manager(manager, input); }
}, pass_manager::build_compiler_pass_manager());
}

void maybe_emit_dump_semantic(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept{
if (input.dump_sem && context.has_transformed){
dump_flags::emit_dump_semantic_items(context.transformed_state.transformed_items, modular_input_entry_label(input));
}
}

void maybe_emit_dump_mir(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept{
if (input.dump_mir && context.has_transformed){
dump_flags::emit_dump_mir_from_semantic_items(context.transformed_state.transformed_items, modular_input_entry_label(input));
}
}

void maybe_emit_mir_bootstrap_report(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept{
if (input.mir_bootstrap_report && context.has_transformed){
dump_flags::emit_mir_bootstrap_report_from_semantic_items(context.transformed_state.transformed_items, modular_input_entry_label(input));
}
}

ast::Result<mlc::String, mlc::Array<mlc::String>> maybe_run_interpreter(pipeline::ModularCompileInput input, pipeline::PipelineContext context) noexcept{
if (!input.run_interpreter){
{
return ast::Ok<mlc::String>(mlc::String(""));
}
}
if (!context.has_transformed){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pipeline: --run requires transform pass")});
}
}
return std::visit(overloaded{
  [&](const ast::Ok<int>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [exit_code] = ok; return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  mlc::io::exit(exit_code);
  return ast::Ok<mlc::String>(mlc::String(""));
 }(); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); }
}, interpreter::run_mir_program_from_semantic_items(context.transformed_state.transformed_items, input.trace_vm));
}

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline_with_manager(pass_manager::PassManager manager, pipeline::ModularCompileInput input) noexcept{
if (input.time_passes && !input.profile_enabled){
{
profile::profile_reset_if_enabled(true);
}
}
ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> stepped = run_pass_manager_descriptors(manager, pipeline_context_new(input), 0);
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<pipeline::PipelineContext>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [final_context] = ok; return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  maybe_emit_dump_semantic(input, final_context);
  maybe_emit_dump_mir(input, final_context);
  maybe_emit_mir_bootstrap_report(input, final_context);
  return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mlc::String>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [message] = ok; return [&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  if (input.time_passes && !input.profile_enabled){
{
profile::profile_finish(true);
}
}
  return ast::Ok<mlc::String>(message);
 }(); }
}, maybe_run_interpreter(input, final_context));
 }(); }
}, stepped);
}

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_pass_manager_descriptors(pass_manager::PassManager manager, pipeline::PipelineContext context, int pass_index) noexcept{
if (pass_index >= manager.descriptors.size()){
{
return ast::Ok<pipeline::PipelineContext>(context);
}
}
pass_manager::PassDescriptor descriptor = manager.descriptors[pass_index];
ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> advanced = advance_pipeline_context(manager, descriptor, context);
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<pipeline::PipelineContext>& ok) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [next] = ok; return run_pass_manager_descriptors(manager, next, pass_index + 1); }
}, advanced);
}

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> advance_pipeline_context(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor, pipeline::PipelineContext context) noexcept{
ast::Result<mlc::String, mlc::Array<mlc::String>> validation = pass_manager::pass_manager_validate_descriptor(manager, descriptor, context.produced_keys);
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mlc::String>& ok) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [_w0] = ok; return [&]() -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { 
  bool timing = pipeline_wants_timing(context.modular_input);
  profile::profile_maybe_begin(timing, descriptor.name);
  ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> stepped_output = run_pipeline_pass(descriptor, context);
  profile::profile_maybe_end(timing, descriptor.name);
  return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<pipeline::PipelineContext>& ok) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [stepped] = ok; return ast::Ok<pipeline::PipelineContext>(pipeline::PipelineContext{stepped.modular_input, stepped.has_checked, stepped.checked_state, stepped.has_transformed, stepped.transformed_state, pass_manager::context_mark_keys(stepped.produced_keys, descriptor.produced_keys), pass_manager::pass_manager_apply_preserved(stepped.preserved, descriptor)}); }
}, stepped_output);
 }(); }
}, validation);
}

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_pipeline_pass(pass_manager::PassDescriptor descriptor, pipeline::PipelineContext context) noexcept{
return descriptor.name == mlc::String("verify_ast") ? ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(run_verify_ast_pass(context)) : ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(descriptor.name == mlc::String("checker") ? ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(run_checker_pipeline_pass(context)) : ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(descriptor.name == mlc::String("transform") ? ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(run_transform_pipeline_pass(context)) : ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(descriptor.name == mlc::String("codegen") ? ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(run_codegen_pipeline_pass(context)) : ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pipeline: unknown pass ") + descriptor.name})))));
}

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_verify_ast_pass(pipeline::PipelineContext context) noexcept{
if (!context.modular_input.verify_each_pass){
{
return ast::Ok<pipeline::PipelineContext>(context);
}
}
mlc::Array<mlc::String> ast_errors = verify_ast::verify_ast_program(context.modular_input.full_program);
if (ast_errors.size() > 0){
{
return ast::Err<mlc::Array<mlc::String>>(ast_errors);
}
}
return ast::Ok<pipeline::PipelineContext>(context);
}

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_checker_pipeline_pass(pipeline::PipelineContext context) noexcept{
auto __try_checked_state = run_checker_pass(context.modular_input);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_checked_state)) return ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_checked_state));
pipeline::CheckedCompileState checked_state = std::get<ast::Ok<pipeline::CheckedCompileState>>(__try_checked_state).field0;

return ast::Ok<pipeline::PipelineContext>(pipeline::PipelineContext{context.modular_input, true, checked_state, context.has_transformed, context.transformed_state, context.produced_keys, context.preserved});
}

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_transform_pipeline_pass(pipeline::PipelineContext context) noexcept{
if (!context.has_checked){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pipeline: checker pass required before transform")});
}
}
auto __try_transformed_state = run_transform_pass(context.checked_state);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_transformed_state)) return ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_transformed_state));
pipeline::TransformedCompileState transformed_state = std::get<ast::Ok<pipeline::TransformedCompileState>>(__try_transformed_state).field0;

return ast::Ok<pipeline::PipelineContext>(pipeline::PipelineContext{context.modular_input, context.has_checked, context.checked_state, true, transformed_state, context.produced_keys, context.preserved});
}

ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> run_codegen_pipeline_pass(pipeline::PipelineContext context) noexcept{
if (context.modular_input.check_only){
{
return ast::Ok<pipeline::PipelineContext>(context);
}
}
if (!context.has_transformed){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("pipeline: transform pass required before codegen")});
}
}
ast::Result<mlc::String, mlc::Array<mlc::String>> codegen_result = run_codegen_pass(context.transformed_state, context.modular_input.emit_compile_commands);
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mlc::String>& ok) -> ast::Result<pipeline::PipelineContext, mlc::Array<mlc::String>> { auto [_w0] = ok; return ast::Ok<pipeline::PipelineContext>(context); }
}, codegen_result);
}

} // namespace pipeline
