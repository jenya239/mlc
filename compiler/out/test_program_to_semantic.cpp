#include "test_program_to_semantic.hpp"

#include "test_runner.hpp"
#include "program_to_semantic.hpp"
#include "decls.hpp"
#include "lexer.hpp"

namespace test_program_to_semantic {

using namespace test_runner;
using namespace program_to_semantic;
using namespace decls;
using namespace lexer;

mlc::Array<test_runner::TestResult> program_to_semantic_tests() noexcept;

mlc::Array<test_runner::TestResult> program_to_semantic_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("module_path_is_safe accepts test_main"), program_to_semantic::module_path_is_safe(mlc::String("test_main"))));
results.push_back(test_runner::assert_true(mlc::String("module_path_is_safe rejects empty"), !program_to_semantic::module_path_is_safe(mlc::String(""))));
results.push_back(test_runner::assert_true(mlc::String("module_path_is_safe rejects semicolon"), !program_to_semantic::module_path_is_safe(mlc::String("foo;bar"))));
mlc::String source = mlc::String("fn main() -> i32 = 0");
ast::Program program = decls::parse_program(lexer::tokenize(source).tokens);
ast::Result<semantic_ir::SemanticLoadItem, mlc::Array<mlc::String>> converted = program_to_semantic::program_to_semantic_load_item(program, mlc::String("test_main"));
results.push_back(test_runner::assert_true(mlc::String("program_to_semantic_load_item ok for minimal program"), std::visit(overloaded{
  [&](const ast::Ok<semantic_ir::SemanticLoadItem>& ok) -> bool { auto [_w0] = ok; return true; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return false; }
}, converted)));
ast::Result<semantic_ir::SemanticLoadItem, mlc::Array<mlc::String>> rejected = program_to_semantic::program_to_semantic_load_item(program, mlc::String("../escape"));
results.push_back(test_runner::assert_true(mlc::String("program_to_semantic_load_item rejects unsafe path"), std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return true; },
  [&](const ast::Ok<semantic_ir::SemanticLoadItem>& ok) -> bool { auto [_w0] = ok; return false; }
}, rejected)));
return results;
}

} // namespace test_program_to_semantic
