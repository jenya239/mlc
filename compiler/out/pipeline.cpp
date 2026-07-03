#define main mlc_user_main
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

void write_text_if_changed(mlc::String path, mlc::String content) noexcept{
  if ((!(mlc::file::exists(path) && (mlc::file::read_to_string(path) == content))))   {
    mlc::file::write_string(path, content);
  }
}
bool pipeline_wants_timing(ModularCompileInput input) noexcept{
  return (input.profile_enabled || input.time_passes);
}
mlc::String modular_input_entry_label(ModularCompileInput input) noexcept{
  if ((input.load_items.length() > 0))   {
    return input.load_items[(input.load_items.length() - 1)].path;
  } else   {
    return mlc::String("program", 7);
  }
}
context::PrecomputedCtx empty_precomputed_context() noexcept{
  return context::PrecomputedCtx{{}, {}, {}, {}, {}, {}, {}, {}, {}};
}
PipelineContext pipeline_context_new(ModularCompileInput input) noexcept{
  return PipelineContext{input, false, CheckedCompileState{input.load_items, input.full_program, registry::empty_registry(), input.output_directory, pipeline_wants_timing(input), input.verify_each_pass}, false, TransformedCompileState{input.load_items, {}, input.full_program, empty_precomputed_context(), input.output_directory, pipeline_wants_timing(input)}, mlc::Array<mlc::String>{mlc::String("modular_input", 13)}, preserved_analyses::preserved_analyses_empty()};
}
ast::Result<CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(ModularCompileInput input) noexcept{
  auto timing_enabled = pipeline_wants_timing(input);
  profile::profile_maybe_begin(timing_enabled, mlc::String("check", 5));
  auto check_output = ({ auto __q = check::check_with_context(input.entry_program, input.full_program); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
  profile::profile_maybe_end(timing_enabled, mlc::String("check", 5));
  return ast::Ok<CheckedCompileState>{CheckedCompileState{input.load_items, input.full_program, check_output.registry, input.output_directory, timing_enabled, input.verify_each_pass}};
}
ast::Result<TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(CheckedCompileState checked_state) noexcept{
  profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("expand_destructure", 18));
  auto expanded_program = param_destructure_expand::expand_parameter_destructuring_in_program(checked_state.full_program);
  profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("expand_destructure", 18));
  auto trait_maps = trait_param_expand::build_trait_nominal_maps(expanded_program);
  profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("transform", 9));
  auto transformed_items = transform_decl::transform_load_items(checked_state.load_items, checked_state.registry, trait_maps);
  profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("transform", 9));
  if (checked_state.verify_each_pass)   {
    auto semantic_errors = verify_semantic_ir::verify_semantic_ir_load_items(transformed_items);
    if ((semantic_errors.length() > 0))     {
      return ast::Err<mlc::Array<mlc::String>>{semantic_errors};
    }
  }
  profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("precompute", 10));
  auto precomputed = module::precompute(expanded_program, checked_state.load_items);
  profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("precompute", 10));
  return ast::Ok<TransformedCompileState>{TransformedCompileState{checked_state.load_items, transformed_items, expanded_program, precomputed, checked_state.output_directory, checked_state.profile_enabled}};
}
ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(TransformedCompileState transformed_state, bool emit_compile_commands) noexcept{
  profile::profile_maybe_begin(transformed_state.profile_enabled, mlc::String("codegen", 7));
  auto implementation_paths = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < transformed_state.transformed_items.length()))   {
    auto transformed_load_item = transformed_state.transformed_items[index];
    auto generated_output = module::gen_module(transformed_load_item, transformed_state.load_items, transformed_state.expanded_program, transformed_state.precomputed);
    auto module_base = cpp_naming::path_to_module_base(transformed_load_item.path);
    auto output_directory_prefix = ((transformed_state.output_directory.length() > 0) ? ((transformed_state.output_directory + mlc::String("/", 1))) : (mlc::String("", 0)));
    auto header_path = ((output_directory_prefix + module_base) + mlc::String(".hpp", 4));
    auto implementation_path = ((output_directory_prefix + module_base) + mlc::String(".cpp", 4));
    write_text_if_changed(header_path, generated_output.header);
    write_text_if_changed(implementation_path, generated_output.source);
    implementation_paths.push_back(implementation_path);
    (index = (index + 1));
  }
  if (emit_compile_commands)   {
    compile_commands::write_compile_commands_file(transformed_state.output_directory, implementation_paths);
  }
  profile::profile_maybe_end(transformed_state.profile_enabled, mlc::String("codegen", 7));
  return ast::Ok<mlc::String>{mlc::String("", 0)};
}
ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(ModularCompileInput input) noexcept{
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<pass_manager::PassManager>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [manager] = ok; return run_modular_compiler_pipeline_with_manager(manager, input); }
}, pass_manager::build_compiler_pass_manager());
}
void maybe_emit_dump_semantic(ModularCompileInput input, PipelineContext context) noexcept{
  if ((input.dump_sem && context.has_transformed))   {
    dump_flags::emit_dump_semantic_items(context.transformed_state.transformed_items, modular_input_entry_label(input));
  }
}
void maybe_emit_dump_mir(ModularCompileInput input, PipelineContext context) noexcept{
  if ((input.dump_mir && context.has_transformed))   {
    dump_flags::emit_dump_mir_from_semantic_items(context.transformed_state.transformed_items, modular_input_entry_label(input));
  }
}
void maybe_emit_mir_bootstrap_report(ModularCompileInput input, PipelineContext context) noexcept{
  if ((input.mir_bootstrap_report && context.has_transformed))   {
    dump_flags::emit_mir_bootstrap_report_from_semantic_items(context.transformed_state.transformed_items, modular_input_entry_label(input));
  }
}
ast::Result<mlc::String, mlc::Array<mlc::String>> maybe_run_interpreter(ModularCompileInput input, PipelineContext context) noexcept{
  if ((!input.run_interpreter))   {
    return ast::Ok<mlc::String>{mlc::String("", 0)};
  }
  if ((!context.has_transformed))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("pipeline: --run requires transform pass", 39)}};
  }
  return std::visit(overloaded{[&](const ast::Ok<int>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [exit_code] = ok; return [&]() {
mlc::io::exit(exit_code);
return ast::Ok<mlc::String>{mlc::String("", 0)};
}(); },
[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; }
}, interpreter::run_mir_program_from_semantic_items(context.transformed_state.transformed_items, input.trace_vm));
}
ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline_with_manager(pass_manager::PassManager manager, ModularCompileInput input) noexcept{
  if ((input.time_passes && (!input.profile_enabled)))   {
    profile::profile_reset_if_enabled(true);
  }
  auto stepped = run_pass_manager_descriptors(manager, pipeline_context_new(input), 0);
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<PipelineContext>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [final_context] = ok; return [&]() {
maybe_emit_dump_semantic(input, final_context);
maybe_emit_dump_mir(input, final_context);
maybe_emit_mir_bootstrap_report(input, final_context);
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mlc::String>& ok) -> ast::Result<mlc::String, mlc::Array<mlc::String>> { auto [message] = ok; return [&]() {
if ((input.time_passes && (!input.profile_enabled))) {
  profile::profile_finish(true);
}
return ast::Ok<mlc::String>{message};
}(); }
}, maybe_run_interpreter(input, final_context));
}(); }
}, stepped);
}
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_pass_manager_descriptors(pass_manager::PassManager manager, PipelineContext context, int pass_index) noexcept{
  if ((pass_index >= manager.descriptors.length()))   {
    return ast::Ok<PipelineContext>{context};
  }
  auto descriptor = manager.descriptors[pass_index];
  auto advanced = advance_pipeline_context(manager, descriptor, context);
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<PipelineContext>& ok) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [next] = ok; return run_pass_manager_descriptors(manager, next, (pass_index + 1)); }
}, advanced);
}
ast::Result<PipelineContext, mlc::Array<mlc::String>> advance_pipeline_context(pass_manager::PassManager manager, pass_manager::PassDescriptor descriptor, PipelineContext context) noexcept{
  auto validation = pass_manager::pass_manager_validate_descriptor(manager, descriptor, context.produced_keys);
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mlc::String>& ok) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [__0] = ok; return [&]() {
auto timing = pipeline_wants_timing(context.modular_input);
profile::profile_maybe_begin(timing, descriptor.name);
auto stepped_output = run_pipeline_pass(descriptor, context);
profile::profile_maybe_end(timing, descriptor.name);
return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<PipelineContext>& ok) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [stepped] = ok; return ast::Ok<PipelineContext>{PipelineContext{stepped.modular_input, stepped.has_checked, stepped.checked_state, stepped.has_transformed, stepped.transformed_state, pass_manager::context_mark_keys(stepped.produced_keys, descriptor.produced_keys), pass_manager::pass_manager_apply_preserved(stepped.preserved, descriptor)}}; }
}, stepped_output);
}(); }
}, validation);
}
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_pipeline_pass(pass_manager::PassDescriptor descriptor, PipelineContext context) noexcept{
  if ((descriptor.name == mlc::String("verify_ast", 10)))   {
    return run_verify_ast_pass(context);
  } else if ((descriptor.name == mlc::String("checker", 7)))   {
    return run_checker_pipeline_pass(context);
  } else if ((descriptor.name == mlc::String("transform", 9)))   {
    return run_transform_pipeline_pass(context);
  } else if ((descriptor.name == mlc::String("codegen", 7)))   {
    return run_codegen_pipeline_pass(context);
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("pipeline: unknown pass ", 23) + mlc::to_string(descriptor.name)) + mlc::String("", 0))}};
  }
}
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_verify_ast_pass(PipelineContext context) noexcept{
  if ((!context.modular_input.verify_each_pass))   {
    return ast::Ok<PipelineContext>{context};
  }
  auto ast_errors = verify_ast::verify_ast_program(context.modular_input.full_program);
  if ((ast_errors.length() > 0))   {
    return ast::Err<mlc::Array<mlc::String>>{ast_errors};
  }
  return ast::Ok<PipelineContext>{context};
}
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_checker_pipeline_pass(PipelineContext context) noexcept{
  auto checked_state = ({ auto __q = run_checker_pass(context.modular_input); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
  return ast::Ok<PipelineContext>{PipelineContext{context.modular_input, true, checked_state, context.has_transformed, context.transformed_state, context.produced_keys, context.preserved}};
}
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_transform_pipeline_pass(PipelineContext context) noexcept{
  if ((!context.has_checked))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("pipeline: checker pass required before transform", 48)}};
  }
  auto transformed_state = ({ auto __q = run_transform_pass(context.checked_state); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
  return ast::Ok<PipelineContext>{PipelineContext{context.modular_input, context.has_checked, context.checked_state, true, transformed_state, context.produced_keys, context.preserved}};
}
ast::Result<PipelineContext, mlc::Array<mlc::String>> run_codegen_pipeline_pass(PipelineContext context) noexcept{
  if (context.modular_input.check_only)   {
    return ast::Ok<PipelineContext>{context};
  }
  if ((!context.has_transformed))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("pipeline: transform pass required before codegen", 48)}};
  }
  auto codegen_result = run_codegen_pass(context.transformed_state, context.modular_input.emit_compile_commands);
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<mlc::String>& ok) -> ast::Result<PipelineContext, mlc::Array<mlc::String>> { auto [__0] = ok; return ast::Ok<PipelineContext>{context}; }
}, codegen_result);
}

} // namespace pipeline
