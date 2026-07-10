#define main mlc_user_main
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

mlc::Array<mlc::String> prefix_parse_errors(mlc::String source_path, mlc::Array<mlc::String> messages) noexcept{
  auto prefixed = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < messages.length()))   {
    prefixed.push_back(((((mlc::String("parse ", 6) + mlc::to_string(source_path)) + mlc::String(": ", 2)) + mlc::to_string(messages[index])) + mlc::String("", 0)));
    (index = mlc::arith::checked_add(index, 1));
  }
  return prefixed;
}
ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled, bool check_only, bool emit_compile_commands, bool verify_each_pass, bool dump_ast, bool dump_sem, bool dump_mir, bool mir_bootstrap_report, bool time_passes, bool run_interpreter, bool trace_vm) noexcept{
  if ((!path_normalize::driver_source_path_is_safe(entry_path)))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("driver: unsafe entry path", 25)}};
  }
  profile::profile_reset_if_enabled(profile_enabled);
  profile::profile_maybe_begin(profile_enabled, mlc::String("total", 5));
  profile::profile_maybe_begin(profile_enabled, mlc::String("load_io", 7));
  auto entry_source = [&]() -> mlc::String {
  if ((entry_path == mlc::String("-", 1)))   {
    return mlc::io::read_all();
  } else   {
    return mlc::file::read_to_string(entry_path);
  }
}();
  profile::profile_maybe_end(profile_enabled, mlc::String("load_io", 7));
  profile::profile_maybe_begin(profile_enabled, mlc::String("lex", 3));
  auto lexer_output = lexer::tokenize(entry_source);
  profile::profile_maybe_end(profile_enabled, mlc::String("lex", 3));
  if (ast_tokens::LexOut_has_errors(lexer_output))   {
    profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
    profile::profile_finish(profile_enabled);
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("lex: ", 5) + mlc::to_string(lexer_output.errors[0])) + mlc::String("", 0))}};
  } else   {
    auto source_path = path_normalize::resolve_dotdot(entry_path);
    profile::profile_maybe_begin(profile_enabled, mlc::String("parse", 5));
    auto parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, source_path);
    profile::profile_maybe_end(profile_enabled, mlc::String("parse", 5));
    if ((check_only && (parse_parsed.errors.length() > 0)))     {
      profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
      profile::profile_finish(profile_enabled);
      return ast::Err<mlc::Array<mlc::String>>{prefix_parse_errors(source_path, parse_parsed.errors)};
    } else     {
      profile::profile_maybe_begin(profile_enabled, mlc::String("merge", 5));
      auto merged = program_merge::merge_program(entry_path, parse_parsed.program, profile_enabled);
      profile::profile_maybe_end(profile_enabled, mlc::String("merge", 5));
      if ((merged.errors.length() > 0))       {
        profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
        profile::profile_finish(profile_enabled);
        return ast::Err<mlc::Array<mlc::String>>{merged.errors};
      } else       {
        auto merged_item_count = merged.items.length();
        auto entry_load_item = merged.items[mlc::arith::checked_sub(merged_item_count, 1)];
        auto entry_only_program = ast::Program{entry_load_item.decls};
        if (dump_ast)         {
          dump_flags::emit_dump_ast(merged.program, entry_path);
        }
        auto pipeline_input = pipeline::ModularCompileInput{merged.items, merged.program, entry_only_program, out_dir, profile_enabled, (check_only || run_interpreter), emit_compile_commands, verify_each_pass, dump_ast, dump_sem, dump_mir, mir_bootstrap_report, time_passes, run_interpreter, trace_vm};
        auto pipeline_parsed = ({ auto __q = pipeline::run_modular_compiler_pipeline(pipeline_input); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
        profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
        profile::profile_finish((profile_enabled || time_passes));
        return ast::Ok<mlc::String>{mlc::String("", 0)};
      }
    }
  }
}

} // namespace compile_driver
