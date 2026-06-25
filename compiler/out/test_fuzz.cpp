#include "test_fuzz.hpp"

#include "test_runner.hpp"
#include "random_program.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "load_item.hpp"
#include "pipeline.hpp"

namespace test_fuzz {

using namespace test_runner;
using namespace random_program;
using namespace lexer;
using namespace decls;
using namespace load_item;
using namespace pipeline;

pipeline::ModularCompileInput fuzz_pipeline_input_for_source(mlc::String source) noexcept;

pipeline::ModularCompileInput fuzz_pipeline_input(int seed) noexcept;

bool fuzz_differential_check_only_ok(mlc::String source) noexcept;

int fuzz_differential_check_only_error_count(mlc::String source) noexcept;

bool fuzz_pipeline_completes(int seed) noexcept;

bool fuzz_lex_parse_completes(mlc::String source) noexcept;

mlc::String fuzz_deep_nesting_source(int depth) noexcept;

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept;

pipeline::ModularCompileInput fuzz_pipeline_input_for_source(mlc::String source) noexcept{
ast::Program program = decls::parse_program(lexer::tokenize(source).tokens);
load_item::LoadItem load_item = load_item::LoadItem{mlc::String("fuzz_probe.mlc"), program.decls, {}, {}};
return pipeline::ModularCompileInput{mlc::Array<load_item::LoadItem>{load_item}, program, program, mlc::String(""), false, true, false, false, false, false, false};
}

pipeline::ModularCompileInput fuzz_pipeline_input(int seed) noexcept{return fuzz_pipeline_input_for_source(random_program::fuzz_random_program(seed));}

bool fuzz_differential_check_only_ok(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return true; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return false; }
}, pipeline::run_modular_compiler_pipeline(fuzz_pipeline_input_for_source(source)));}

int fuzz_differential_check_only_error_count(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return errors.size(); }
}, pipeline::run_modular_compiler_pipeline(fuzz_pipeline_input_for_source(source)));}

bool fuzz_pipeline_completes(int seed) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return true; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return true; }
}, pipeline::run_modular_compiler_pipeline(fuzz_pipeline_input(seed)));}

bool fuzz_lex_parse_completes(mlc::String source) noexcept{
ast_tokens::LexOut lex = lexer::tokenize(source);
return ast_tokens::LexOut_has_errors(lex) ? true : [&]() -> bool { 
  decls::parse_program(lex.tokens);
  return true;
 }();
}

mlc::String fuzz_deep_nesting_source(int depth) noexcept{
mlc::String body = mlc::String("0");
int level = 0;
while (level < depth){
{
body = mlc::String("do\n") + body + mlc::String("\nend");
level = level + 1;
}
}
return mlc::String("fn fuzz_entry() -> i32 = ") + body;
}

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("fuzz generator non-empty"), random_program::fuzz_random_program(0).length() > 0));
results.push_back(test_runner::assert_eq_str(mlc::String("fuzz generator deterministic"), random_program::fuzz_random_program(42), random_program::fuzz_random_program(42)));
results.push_back(test_runner::assert_true(mlc::String("fuzz mix varies with seed"), random_program::fuzz_mix(0) != random_program::fuzz_mix(1)));
results.push_back(test_runner::assert_eq_str(mlc::String("fuzz variant count"), mlc::to_string(random_program::fuzz_variant_count()), mlc::String("12")));
int seed = 0;
while (seed < random_program::fuzz_variant_count()){
{
results.push_back(test_runner::assert_true(mlc::String("fuzz program variants seed ") + mlc::to_string(seed), random_program::fuzz_random_program(seed).contains(mlc::String("fn "))));
seed = seed + 1;
}
}
results.push_back(test_runner::assert_true(mlc::String("fuzz if variant"), random_program::fuzz_random_program(7).contains(mlc::String("if true then"))));
results.push_back(test_runner::assert_true(mlc::String("fuzz let variant"), random_program::fuzz_random_program(0).contains(mlc::String("let x ="))));
results.push_back(test_runner::assert_true(mlc::String("fuzz match variant"), random_program::fuzz_random_program(5).contains(mlc::String("match "))));
results.push_back(test_runner::assert_true(mlc::String("fuzz type stub variant"), random_program::fuzz_program_kind(10) == 9 && random_program::fuzz_random_program(10).contains(mlc::String("type FuzzStub_"))));
results.push_back(test_runner::assert_true(mlc::String("fuzz record variant"), random_program::fuzz_program_kind(3) == 10 && random_program::fuzz_random_program(3).contains(mlc::String("FuzzRecord_"))));
results.push_back(test_runner::assert_true(mlc::String("fuzz nested block variant"), random_program::fuzz_program_kind(8) == 11 && random_program::fuzz_random_program(8).contains(mlc::String("let outer = do"))));
results.push_back(test_runner::assert_true(mlc::String("fuzz garbage lex parse completes"), fuzz_lex_parse_completes(mlc::String("fn @@@"))));
results.push_back(test_runner::assert_true(mlc::String("fuzz garbage unclosed string lex"), fuzz_lex_parse_completes(mlc::String("fn f() -> string = \"unclosed"))));
results.push_back(test_runner::assert_true(mlc::String("fuzz garbage deep nesting lex parse"), fuzz_lex_parse_completes(fuzz_deep_nesting_source(24))));
int pipeline_seed = 0;
while (pipeline_seed < 64){
{
mlc::String source = random_program::fuzz_random_program(pipeline_seed);
results.push_back(test_runner::assert_true(mlc::String("fuzz check_only pipeline seed ") + mlc::to_string(pipeline_seed), fuzz_pipeline_completes(pipeline_seed)));
results.push_back(test_runner::assert_true(mlc::String("fuzz lex parse seed ") + mlc::to_string(pipeline_seed), fuzz_lex_parse_completes(source)));
pipeline_seed = pipeline_seed + 1;
}
}
int differential_seed = 0;
while (differential_seed < 8){
{
results.push_back(test_runner::assert_true(mlc::String("fuzz differential seed ") + mlc::to_string(differential_seed) + mlc::String(" check_only ok"), fuzz_differential_check_only_ok(random_program::fuzz_random_program(differential_seed))));
differential_seed = differential_seed + 1;
}
}
mlc::String immutable_assign = mlc::String("fn f() -> i32 = do\n  let x = 1\n  x = 2\n  x\nend");
mlc::String not_callable = mlc::String("fn f() -> i32 = 42()");
mlc::String return_type_mismatch = mlc::String("fn f() -> i32 = \"not an integer\"");
mlc::String undefined_ident = mlc::String("fn f() -> i32 = no_such_binding");
mlc::String wrong_call_arity = mlc::String("fn pair_sum(first: i32, second: i32) -> i32 = first + second\nfn main() -> i32 = pair_sum(1)");
results.push_back(test_runner::assert_eq_int(mlc::String("fuzz differential checker-negative immutable assign"), fuzz_differential_check_only_error_count(immutable_assign), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fuzz differential checker-negative not callable"), fuzz_differential_check_only_error_count(not_callable), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fuzz differential checker-negative return type"), fuzz_differential_check_only_error_count(return_type_mismatch), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fuzz differential checker-negative undefined ident"), fuzz_differential_check_only_error_count(undefined_ident), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fuzz differential checker-negative wrong call arity"), fuzz_differential_check_only_error_count(wrong_call_arity), 1));
return results;
}

} // namespace test_fuzz
