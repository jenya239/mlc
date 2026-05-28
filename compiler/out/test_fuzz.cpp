#include "test_fuzz.hpp"

#include "test_runner.hpp"
#include "random_program.hpp"

namespace test_fuzz {

using namespace test_runner;
using namespace random_program;

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept;

mlc::Array<test_runner::TestResult> fuzz_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("fuzz generator non-empty"), random_program::fuzz_random_program(0).length() > 0));
results.push_back(test_runner::assert_eq_str(mlc::String("fuzz generator deterministic"), random_program::fuzz_random_program(42), random_program::fuzz_random_program(42)));
results.push_back(test_runner::assert_true(mlc::String("fuzz mix varies with seed"), random_program::fuzz_mix(0) != random_program::fuzz_mix(1)));
int seed = 0;
while (seed < 6){
{
results.push_back(test_runner::assert_true(mlc::String("fuzz program variants seed ") + mlc::to_string(seed), random_program::fuzz_random_program(seed).contains(mlc::String("fn "))));
seed = seed + 1;
}
}
return results;
}

} // namespace test_fuzz
