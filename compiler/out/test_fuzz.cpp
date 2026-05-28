#include "test_fuzz.hpp"

#include "test_runner.hpp"
#include "random_program.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "decl_index.hpp"
#include "pipeline.hpp"

namespace test_fuzz {

using namespace test_runner;
using namespace random_program;
using namespace lexer;
using namespace decls;
using namespace decl_index;
using namespace pipeline;

bool fuzz_pipeline_completes(int seed) noexcept;

pipeline::ModularCompileInput fuzz_pipeline_input(int seed) noexcept;

bool fuzz_lex_parse_completes(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept;

bool fuzz_pipeline_completes(int seed) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return true; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return true; }
}, pipeline::run_modular_compiler_pipeline(fuzz_pipeline_input(seed)));}

pipeline::ModularCompileInput fuzz_pipeline_input(int seed) noexcept{
mlc::String source = random_program::fuzz_random_program(seed);
ast::Program program = decls::parse_program(lexer::tokenize(source).tokens);
decl_index::LoadItem load_item = decl_index::LoadItem{mlc::String("fuzz_probe.mlc"), program.decls, {}, {}};
return pipeline::ModularCompileInput{mlc::Array<decl_index::LoadItem>{load_item}, program, program, mlc::String(""), false, true};
}

bool fuzz_lex_parse_completes(mlc::String source) noexcept{
ast_tokens::LexOut lex = lexer::tokenize(source);
return ast_tokens::LexOut_has_errors(lex) ? true : [&]() -> bool { 
  decls::parse_program(lex.tokens);
  return true;
 }();
}

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("fuzz generator non-empty"), random_program::fuzz_random_program(0).length() > 0));
results.push_back(test_runner::assert_eq_str(mlc::String("fuzz generator deterministic"), random_program::fuzz_random_program(42), random_program::fuzz_random_program(42)));
results.push_back(test_runner::assert_true(mlc::String("fuzz mix varies with seed"), random_program::fuzz_mix(0) != random_program::fuzz_mix(1)));
int seed = 0;
while (seed < 9){
{
results.push_back(test_runner::assert_true(mlc::String("fuzz program variants seed ") + mlc::to_string(seed), random_program::fuzz_random_program(seed).contains(mlc::String("fn "))));
seed = seed + 1;
}
}
results.push_back(test_runner::assert_true(mlc::String("fuzz if variant"), random_program::fuzz_random_program(4).contains(mlc::String("if true then"))));
results.push_back(test_runner::assert_true(mlc::String("fuzz let variant"), random_program::fuzz_random_program(0).contains(mlc::String("let x ="))));
results.push_back(test_runner::assert_true(mlc::String("fuzz match variant"), random_program::fuzz_random_program(5).contains(mlc::String("match "))));
results.push_back(test_runner::assert_true(mlc::String("fuzz garbage lex parse completes"), fuzz_lex_parse_completes(mlc::String("fn @@@"))));
int pipeline_seed = 0;
while (pipeline_seed < 32){
{
mlc::String source = random_program::fuzz_random_program(pipeline_seed);
results.push_back(test_runner::assert_true(mlc::String("fuzz check_only pipeline seed ") + mlc::to_string(pipeline_seed), fuzz_pipeline_completes(pipeline_seed)));
results.push_back(test_runner::assert_true(mlc::String("fuzz lex parse seed ") + mlc::to_string(pipeline_seed), fuzz_lex_parse_completes(source)));
pipeline_seed = pipeline_seed + 1;
}
}
return results;
}

} // namespace test_fuzz
