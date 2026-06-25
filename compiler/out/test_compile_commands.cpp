#include "test_compile_commands.hpp"

#include "test_runner.hpp"
#include "compile_commands.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "load_item.hpp"
#include "pipeline.hpp"

namespace test_compile_commands {

using namespace test_runner;
using namespace compile_commands;
using namespace lexer;
using namespace decls;
using namespace load_item;
using namespace pipeline;

pipeline::ModularCompileInput compile_commands_pipeline_input(mlc::String source, mlc::String output_directory) noexcept;

bool compile_commands_json_contents_ok(mlc::String json) noexcept;

bool compile_commands_json_has_expected_fields(mlc::String json_path) noexcept;

bool pipeline_compile_commands_json_ok(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> compile_commands_tests() noexcept;

pipeline::ModularCompileInput compile_commands_pipeline_input(mlc::String source, mlc::String output_directory) noexcept{
ast::Program program = decls::parse_program(lexer::tokenize(source).tokens);
load_item::LoadItem load_item = load_item::LoadItem{mlc::String("compile_commands_probe.mlc"), program.decls, {}, {}};
return pipeline::ModularCompileInput{mlc::Array<load_item::LoadItem>{load_item}, program, program, output_directory, false, false, true, false, false, false, false};
}

bool compile_commands_json_contents_ok(mlc::String json) noexcept{return json.contains(mlc::String("\"directory\"")) && json.contains(mlc::String("\"file\"")) && json.contains(mlc::String("\"arguments\"")) && json.contains(mlc::String("clang++")) && json.contains(compile_commands::runtime_include_directory()) && json.contains(mlc::String("compile_commands_probe.cpp"));}

bool compile_commands_json_has_expected_fields(mlc::String json_path) noexcept{return mlc::file::exists(json_path) ? compile_commands_json_contents_ok(mlc::file::read_to_string(json_path)) : false;}

bool pipeline_compile_commands_json_ok(mlc::String source) noexcept{
mlc::String output_directory = mlc::file::make_temp_directory(mlc::String("mlcc_test_"));
mlc::String json_path = output_directory + mlc::String("/compile_commands.json");
return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return compile_commands_json_has_expected_fields(json_path); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return false; }
}, pipeline::run_modular_compiler_pipeline(compile_commands_pipeline_input(source, output_directory)));
}

mlc::Array<test_runner::TestResult> compile_commands_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
mlc::Array<mlc::String> arguments = compile_commands::compile_command_arguments(mlc::String("/tmp/out"), mlc::String("/tmp/out/module.cpp"));
results.push_back(test_runner::assert_eq_str(mlc::String("compile_command_arguments compiler"), arguments[0], mlc::String("clang++")));
results.push_back(test_runner::assert_true(mlc::String("compile_command_arguments includes output include path"), arguments.size() >= 6 && arguments[4] == mlc::String("/tmp/out")));
results.push_back(test_runner::assert_true(mlc::String("compile_command_arguments includes runtime include path"), arguments.size() >= 8 && arguments[6] == compile_commands::runtime_include_directory()));
results.push_back(test_runner::assert_true(mlc::String("compile_command_arguments includes source file"), arguments.size() >= 9 && arguments[8] == mlc::String("/tmp/out/module.cpp")));
mlc::String json = compile_commands::build_compile_commands_json(mlc::String("/tmp/out"), mlc::Array<mlc::String>{mlc::String("/tmp/out/module.cpp")});
results.push_back(test_runner::assert_true(mlc::String("build_compile_commands_json array open"), json.contains(mlc::String("[\n"))));
results.push_back(test_runner::assert_true(mlc::String("build_compile_commands_json contains directory field"), json.contains(mlc::String("\"directory\": \"/tmp/out\""))));
results.push_back(test_runner::assert_true(mlc::String("build_compile_commands_json contains file field"), json.contains(mlc::String("\"file\": \"/tmp/out/module.cpp\""))));
results.push_back(test_runner::assert_true(mlc::String("build_compile_commands_json contains clang++ argument"), json.contains(mlc::String("\"clang++\""))));
results.push_back(test_runner::assert_true(mlc::String("pipeline emit_compile_commands writes json"), pipeline_compile_commands_json_ok(mlc::String("fn probe() -> i32 = 42"))));
return results;
}

} // namespace test_compile_commands
