#include "test_merge_imports.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "trait_param_expand.hpp"

namespace test_merge_imports {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace check;
using namespace trait_param_expand;

int check_error_count(mlc::String source) noexcept;

int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept;

mlc::Array<test_runner::TestResult> merge_import_tests() noexcept;

int check_error_count(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return errors.size(); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));}

int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return errors.size(); }
}, check::check_with_context(decls::parse_program(lexer::tokenize(entry_source).tokens), decls::parse_program(lexer::tokenize(full_source).tokens)));}

mlc::Array<test_runner::TestResult> merge_import_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
mlc::String shared_library = mlc::String("trait Display { fn show(self: Point) -> string }\ntype Point = { x: i32, y: i32 }\n");
results.push_back(test_runner::assert_eq_int(mlc::String("merge: entry uses trait from full program"), check_with_context_error_count(mlc::String("fn main() -> i32 = do\n  let point = Point { x: 1, y: 2 }\n  0\nend"), shared_library + mlc::String("extend Point : Display { fn show(self: Point) -> string = \"ok\" }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("merge: diamond-shaped shared types in full program"), check_with_context_error_count(mlc::String("fn main() -> i32 = use_both()"), mlc::String("type Node = { value: i32 }\nfn use_left(node: Node) -> i32 = node.value\nfn use_right(node: Node) -> i32 = node.value + 1\nfn use_both() -> i32 = use_left(Node { value: 1 }) + use_right(Node { value: 2 })")), 0));
results.push_back(test_runner::assert_true(mlc::String("merge: trait/type name conflict reported"), check_error_count(mlc::String("type Display = { x: i32 }\ntrait Display { fn show(self: Display) -> string }")) > 0));
ast::Program full_program = decls::parse_program(lexer::tokenize(mlc::String("type Writable { fn write(self: Buffer) -> unit }\ntype Buffer = { bytes: i32 }\n")).tokens);
trait_param_expand::TraitNominalMaps maps = trait_param_expand::build_trait_nominal_maps(full_program);
results.push_back(test_runner::assert_true(mlc::String("merge: trait nominal maps contain trait name"), maps.trait_declaration_names.has(mlc::String("Writable"))));
results.push_back(test_runner::assert_true(mlc::String("merge: trait nominal maps contain type name"), maps.nominal_type_declaration_names.has(mlc::String("Buffer"))));
return results;
}

} // namespace test_merge_imports
