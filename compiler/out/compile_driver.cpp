#include "compile_driver.hpp"

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "pipeline.hpp"
#include "profile.hpp"
#include "dump_flags.hpp"
#include "path_normalize.hpp"
#include "program_merge.hpp"

namespace compile_driver {

using namespace ast;
using namespace lexer;
using namespace decls;
using namespace pipeline;
using namespace profile;
using namespace dump_flags;
using namespace path_normalize;
using namespace program_merge;
using namespace ast_tokens;

mlc::Array<mlc::String> prefix_parse_errors(mlc::String source_path, mlc::Array<mlc::String> messages) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled, bool check_only, bool emit_compile_commands, bool verify_each_pass, bool dump_ast, bool dump_sem, bool time_passes) noexcept;

mlc::Array<mlc::String> prefix_parse_errors(mlc::String source_path, mlc::Array<mlc::String> messages) noexcept{
mlc::Array<mlc::String> prefixed = {};
int index = 0;
while (index < messages.size()){
{
prefixed.push_back(mlc::String("parse ") + source_path + mlc::String(": ") + messages[index]);
index = index + 1;
}
}
return prefixed;
}

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled, bool check_only, bool emit_compile_commands, bool verify_each_pass, bool dump_ast, bool dump_sem, bool time_passes) noexcept{
if (!path_normalize::driver_source_path_is_safe(entry_path)){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("driver: unsafe entry path")});
}
}
profile::profile_reset_if_enabled(profile_enabled);
profile::profile_maybe_begin(profile_enabled, mlc::String("total"));
profile::profile_maybe_begin(profile_enabled, mlc::String("load_io"));
mlc::String entry_source = mlc::file::read_to_string(entry_path);
profile::profile_maybe_end(profile_enabled, mlc::String("load_io"));
profile::profile_maybe_begin(profile_enabled, mlc::String("lex"));
ast_tokens::LexOut lexer_output = lexer::tokenize(entry_source);
profile::profile_maybe_end(profile_enabled, mlc::String("lex"));
return ast_tokens::LexOut_has_errors(lexer_output) ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("lex: ") + lexer_output.errors[0]});
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  mlc::String source_path = path_normalize::resolve_dotdot(entry_path);
  profile::profile_maybe_begin(profile_enabled, mlc::String("parse"));
  predicates::ProgramParseValue parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, source_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("parse"));
  return check_only && parse_parsed.errors.size() > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(prefix_parse_errors(source_path, parse_parsed.errors));
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_begin(profile_enabled, mlc::String("merge"));
  program_merge::MergeResult merged = program_merge::merge_program(entry_path, parse_parsed.program, profile_enabled);
  profile::profile_maybe_end(profile_enabled, mlc::String("merge"));
  return merged.errors.size() > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(merged.errors);
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  int merged_item_count = merged.items.size();
  load_item::LoadItem entry_load_item = merged.items[merged_item_count - 1];
  ast::Program entry_only_program = ast::Program{entry_load_item.decls};
  if (dump_ast){
{
dump_flags::emit_dump_ast(merged.program, entry_path);
}
}
  pipeline::ModularCompileInput pipeline_input = pipeline::ModularCompileInput{merged.items, merged.program, entry_only_program, out_dir, profile_enabled, check_only, emit_compile_commands, verify_each_pass, dump_ast, dump_sem, time_passes};
  auto __try_pipeline_parsed = pipeline::run_modular_compiler_pipeline(pipeline_input);
  if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_pipeline_parsed)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_pipeline_parsed));
  mlc::String pipeline_parsed = std::get<ast::Ok<mlc::String>>(__try_pipeline_parsed).field0;
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled || time_passes);
  return ast::Ok<mlc::String>(mlc::String(""));
 }());
 }());
 }());
}

} // namespace compile_driver
