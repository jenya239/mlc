#include "test_pass.hpp"

#include "test_runner.hpp"
#include "compile_options.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "decl_index.hpp"
#include "pipeline.hpp"

namespace test_pass {

using namespace test_runner;
using namespace compile_options;
using namespace lexer;
using namespace decls;
using namespace decl_index;
using namespace pipeline;

bool pipeline_check_only_ok(mlc::String source) noexcept;

int pipeline_check_only_error_count(mlc::String source) noexcept;

pipeline::ModularCompileInput pipeline_input_for_source(mlc::String source, bool check_only) noexcept;

bool check_only_writes_no_header(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> pass_type_tests() noexcept;

bool pipeline_check_only_ok(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return true; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return false; }
}, pipeline::run_modular_compiler_pipeline(pipeline_input_for_source(source, true)));}

int pipeline_check_only_error_count(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return errors.size(); }
}, pipeline::run_modular_compiler_pipeline(pipeline_input_for_source(source, true)));}

pipeline::ModularCompileInput pipeline_input_for_source(mlc::String source, bool check_only) noexcept{
ast::Program program = decls::parse_program(lexer::tokenize(source).tokens);
decl_index::LoadItem load_item = decl_index::LoadItem{mlc::String("check_only_probe.mlc"), program.decls, {}, {}};
return pipeline::ModularCompileInput{mlc::Array<decl_index::LoadItem>{load_item}, program, program, mlc::String(""), false, check_only, false};
}

bool check_only_writes_no_header(mlc::String source) noexcept{
mlc::String header_path = mlc::String("check_only_probe.hpp");
bool before = mlc::file::exists(header_path);
bool ok = pipeline_check_only_ok(source);
return before ? ok : ok && !mlc::file::exists(header_path);
}

mlc::Array<test_runner::TestResult> pass_type_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
compile_options::CompileOptions parsed = compile_options::parse_compile_options(mlc::Array<mlc::String>{mlc::String("--check-only"), mlc::String("--profile"), mlc::String("entry.mlc"), mlc::String("-o"), mlc::String("build")});
results.push_back(test_runner::assert_eq_str(mlc::String("parse_compile_options --check-only entry and -o"), parsed.entry_path, mlc::String("entry.mlc")));
results.push_back(test_runner::assert_eq_str(mlc::String("parse_compile_options --check-only out dir"), parsed.out_directory, mlc::String("build")));
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options --check-only profile flag"), parsed.profile_enabled));
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options --check-only flag"), parsed.check_only));
compile_options::CompileOptions compile_commands_parsed = compile_options::parse_compile_options(mlc::Array<mlc::String>{mlc::String("--emit-compile-commands"), mlc::String("entry.mlc"), mlc::String("-o"), mlc::String("build")});
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options --emit-compile-commands flag"), compile_commands_parsed.emit_compile_commands));
results.push_back(test_runner::assert_eq_str(mlc::String("parse_compile_options --emit-compile-commands entry"), compile_commands_parsed.entry_path, mlc::String("entry.mlc")));
compile_options::CompileOptions default_parsed = compile_options::parse_compile_options(mlc::Array<mlc::String>{mlc::String("entry.mlc")});
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options default emit_compile_commands false"), !default_parsed.emit_compile_commands));
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options default temp out dir"), default_parsed.out_directory.contains(mlc::String("mlcc_"))));
compile_options::CompileOptions check_only_parsed = compile_options::parse_compile_options(mlc::Array<mlc::String>{mlc::String("--check-only"), mlc::String("entry.mlc")});
results.push_back(test_runner::assert_eq_str(mlc::String("parse_compile_options check_only empty out dir"), check_only_parsed.out_directory, mlc::String("")));
results.push_back(test_runner::assert_true(mlc::String("check_only pipeline valid program"), pipeline_check_only_ok(mlc::String("fn f() -> i32 = 42"))));
results.push_back(test_runner::assert_true(mlc::String("check_only pipeline rejects type error"), pipeline_check_only_error_count(mlc::String("fn f() -> i32 = \"x\"")) > 0));
results.push_back(test_runner::assert_true(mlc::String("check_only pipeline writes no header file"), check_only_writes_no_header(mlc::String("fn f() -> i32 = 42"))));
return results;
}

} // namespace test_pass
