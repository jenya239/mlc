#include "test_dump_flags.hpp"

#include "test_runner.hpp"
#include "compile_options.hpp"
#include "dump_flags.hpp"
#include "semantic_ir_dump.hpp"
#include "program_to_semantic.hpp"
#include "decls.hpp"
#include "lexer.hpp"

namespace test_dump_flags {

using namespace test_runner;
using namespace compile_options;
using namespace dump_flags;
using namespace semantic_ir_dump;
using namespace program_to_semantic;
using namespace decls;
using namespace lexer;

mlc::Array<test_runner::TestResult> dump_flags_tests() noexcept;

mlc::Array<test_runner::TestResult> dump_flags_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("dump_label_is_safe accepts entry.mlc"), dump_flags::dump_label_is_safe(mlc::String("entry.mlc"))));
results.push_back(test_runner::assert_true(mlc::String("dump_label_is_safe rejects semicolon"), !dump_flags::dump_label_is_safe(mlc::String("bad;path"))));
results.push_back(test_runner::assert_true(mlc::String("dump_label_is_safe rejects empty"), !dump_flags::dump_label_is_safe(mlc::String(""))));
compile_options::CompileOptions parsed = compile_options::parse_compile_options(mlc::Array<mlc::String>{mlc::String("--dump-ast"), mlc::String("--dump-sem"), mlc::String("--time-passes"), mlc::String("--check-only"), mlc::String("entry.mlc")});
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options --dump-ast"), parsed.dump_ast));
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options --dump-sem"), parsed.dump_sem));
results.push_back(test_runner::assert_true(mlc::String("parse_compile_options --time-passes"), parsed.time_passes));
ast::Program program = decls::parse_program(lexer::tokenize(mlc::String("fn main() -> i32 = 0")).tokens);
std::visit(overloaded{
  [&](const ast::Ok<semantic_ir::SemanticLoadItem>& ok) -> void { auto [item] = ok; [&]() { 
  mlc::String summary = semantic_ir_dump::print_semantic_load_item(item);
  return results.push_back(test_runner::assert_true(mlc::String("print_semantic_load_item mentions fn main"), summary.contains(mlc::String("fn main"))));
 }(); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> void { auto [_w0] = err; results.push_back(test_runner::assert_true(mlc::String("program_to_semantic_load_item for dump test"), false)); }
}, program_to_semantic::program_to_semantic_load_item(program, mlc::String("probe.mlc")));
return results;
}

} // namespace test_dump_flags
