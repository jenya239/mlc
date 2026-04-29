#ifndef TESTS_MAIN_HPP
#define TESTS_MAIN_HPP

#include "mlc.hpp"
#include <variant>

#include "test_lexer.hpp"
#include "test_parser.hpp"
#include "test_checker.hpp"
#include "test_codegen.hpp"
#include "test_pipe_and_record_update.hpp"
#include "test_decl_gen.hpp"
#include "test_runner.hpp"

namespace tests_main {

mlc::Array<test_runner::TestResult> all_tests() noexcept;
int main(int argc, char** argv) noexcept;

} // namespace tests_main

#endif // TESTS_MAIN_HPP
