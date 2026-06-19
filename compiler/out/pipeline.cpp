#include "pipeline.hpp"

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

using namespace ast;
using namespace check;
using namespace registry;
using namespace trait_param_expand;
using namespace param_destructure_expand;
using namespace transform_decl;
using namespace semantic_ir;
using namespace decl_index;
using namespace module;
using namespace cpp_naming;
using namespace profile;
using namespace compile_commands;
using namespace ast_tokens;

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState checked_state) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(pipeline::TransformedCompileState transformed_state, bool emit_compile_commands) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept{
profile::profile_maybe_begin(input.profile_enabled, mlc::String("check"));
auto __try_check_output = check::check_with_context(input.entry_program, input.full_program);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_check_output)) return ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_check_output));
check::CheckOut check_output = std::get<ast::Ok<check::CheckOut>>(__try_check_output).field0;

profile::profile_maybe_end(input.profile_enabled, mlc::String("check"));
return ast::Ok<pipeline::CheckedCompileState>(pipeline::CheckedCompileState{input.load_items, input.full_program, check_output.registry, input.output_directory, input.profile_enabled});
}

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState checked_state) noexcept{
profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("expand_destructure"));
ast::Program expanded_program = param_destructure_expand::expand_parameter_destructuring_in_program(checked_state.full_program);
profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("expand_destructure"));
trait_param_expand::TraitNominalMaps trait_maps = trait_param_expand::build_trait_nominal_maps(expanded_program);
profile::profile_maybe_begin(checked_state.profile_enabled, mlc::String("transform"));
mlc::Array<semantic_ir::SemanticLoadItem> transformed_items = transform_decl::transform_load_items(checked_state.load_items, checked_state.registry, trait_maps);
profile::profile_maybe_end(checked_state.profile_enabled, mlc::String("transform"));
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
mlc::String header_path = transformed_state.output_directory.length() > 0 ? transformed_state.output_directory + mlc::String("/") + module_base + mlc::String(".hpp") : module_base + mlc::String(".hpp");
mlc::String implementation_path = transformed_state.output_directory.length() > 0 ? transformed_state.output_directory + mlc::String("/") + module_base + mlc::String(".cpp") : module_base + mlc::String(".cpp");
mlc::file::write_string(header_path, generated_output.header);
mlc::file::write_string(implementation_path, generated_output.source);
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
auto __try_checked_state = run_checker_pass(input);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_checked_state)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_checked_state));
pipeline::CheckedCompileState checked_state = std::get<ast::Ok<pipeline::CheckedCompileState>>(__try_checked_state).field0;

auto __try_transformed_state = run_transform_pass(checked_state);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_transformed_state)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_transformed_state));
pipeline::TransformedCompileState transformed_state = std::get<ast::Ok<pipeline::TransformedCompileState>>(__try_transformed_state).field0;

return input.check_only ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Ok<mlc::String>(mlc::String(""))) : ast::Result<mlc::String, mlc::Array<mlc::String>>(run_codegen_pass(transformed_state, input.emit_compile_commands));
}

} // namespace pipeline
