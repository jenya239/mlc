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
using namespace ast_tokens;

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState state) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(pipeline::TransformedCompileState state) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(pipeline::ModularCompileInput input) noexcept;

ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>> run_checker_pass(pipeline::ModularCompileInput input) noexcept{
profile::profile_maybe_begin(input.profile_enabled, mlc::String("check"));
auto __try_check_output = check::check_with_context(input.entry_program, input.full_program);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_check_output)) return ast::Result<pipeline::CheckedCompileState, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_check_output));
check::CheckOut check_output = std::get<ast::Ok<check::CheckOut>>(__try_check_output).field0;

profile::profile_maybe_end(input.profile_enabled, mlc::String("check"));
return ast::Ok<pipeline::CheckedCompileState>(pipeline::CheckedCompileState{input.load_items, input.full_program, check_output.registry, input.output_directory, input.profile_enabled});
}

ast::Result<pipeline::TransformedCompileState, mlc::Array<mlc::String>> run_transform_pass(pipeline::CheckedCompileState state) noexcept{
profile::profile_maybe_begin(state.profile_enabled, mlc::String("expand_destructure"));
ast::Program expanded_program = param_destructure_expand::expand_parameter_destructuring_in_program(state.full_program);
profile::profile_maybe_end(state.profile_enabled, mlc::String("expand_destructure"));
trait_param_expand::TraitNominalMaps trait_maps = trait_param_expand::build_trait_nominal_maps(expanded_program);
profile::profile_maybe_begin(state.profile_enabled, mlc::String("transform"));
mlc::Array<semantic_ir::SLoadItem> transformed_items = transform_decl::transform_load_items(state.load_items, state.registry, trait_maps);
profile::profile_maybe_end(state.profile_enabled, mlc::String("transform"));
profile::profile_maybe_begin(state.profile_enabled, mlc::String("precompute"));
context::PrecomputedCtx precomputed = module::precompute(expanded_program, state.load_items);
profile::profile_maybe_end(state.profile_enabled, mlc::String("precompute"));
return ast::Ok<pipeline::TransformedCompileState>(pipeline::TransformedCompileState{state.load_items, transformed_items, expanded_program, precomputed, state.output_directory, state.profile_enabled});
}

ast::Result<mlc::String, mlc::Array<mlc::String>> run_codegen_pass(pipeline::TransformedCompileState state) noexcept{
profile::profile_maybe_begin(state.profile_enabled, mlc::String("codegen"));
int index = 0;
while (index < state.transformed_items.size()){
{
semantic_ir::SLoadItem transformed_load_item = state.transformed_items[index];
context::GenModuleOut generated_output = module::gen_module(transformed_load_item, state.load_items, state.expanded_program, state.precomputed);
mlc::String module_base = cpp_naming::path_to_module_base(transformed_load_item.path);
mlc::String header_path = state.output_directory.length() > 0 ? state.output_directory + mlc::String("/") + module_base + mlc::String(".hpp") : module_base + mlc::String(".hpp");
mlc::String implementation_path = state.output_directory.length() > 0 ? state.output_directory + mlc::String("/") + module_base + mlc::String(".cpp") : module_base + mlc::String(".cpp");
mlc::file::write_string(header_path, generated_output.h);
mlc::file::write_string(implementation_path, generated_output.c);
index = index + 1;
}
}
profile::profile_maybe_end(state.profile_enabled, mlc::String("codegen"));
return ast::Ok<mlc::String>(mlc::String(""));
}

ast::Result<mlc::String, mlc::Array<mlc::String>> run_modular_compiler_pipeline(pipeline::ModularCompileInput input) noexcept{
auto __try_checked = run_checker_pass(input);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_checked)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_checked));
pipeline::CheckedCompileState checked = std::get<ast::Ok<pipeline::CheckedCompileState>>(__try_checked).field0;

auto __try_transformed = run_transform_pass(checked);
if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_transformed)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_transformed));
pipeline::TransformedCompileState transformed = std::get<ast::Ok<pipeline::TransformedCompileState>>(__try_transformed).field0;

return run_codegen_pass(transformed);
}

} // namespace pipeline
