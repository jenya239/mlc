#include "test_formatter.hpp"

#include "test_runner.hpp"
#include "ast_printer.hpp"
#include "lexer.hpp"
#include "predicates.hpp"
#include "exprs.hpp"

namespace test_formatter {

using namespace test_runner;
using namespace ast_printer;
using namespace lexer;
using namespace predicates;
using namespace exprs;

mlc::String format_expression_source(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> formatter_tests() noexcept;

mlc::String format_expression_source(mlc::String source) noexcept{return ast_printer::print_expr(exprs::parse_expr(predicates::parser_new(lexer::tokenize(source).tokens)).value);}

mlc::Array<test_runner::TestResult> formatter_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_str(mlc::String("format int literal"), format_expression_source(mlc::String("42")), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("format string literal"), format_expression_source(mlc::String("\"hello\"")), mlc::String("\"hello\"")));
results.push_back(test_runner::assert_eq_str(mlc::String("format bool true"), format_expression_source(mlc::String("true")), mlc::String("true")));
results.push_back(test_runner::assert_eq_str(mlc::String("format bool false"), format_expression_source(mlc::String("false")), mlc::String("false")));
results.push_back(test_runner::assert_eq_str(mlc::String("format unit"), format_expression_source(mlc::String("()")), mlc::String("()")));
results.push_back(test_runner::assert_eq_str(mlc::String("format ident"), format_expression_source(mlc::String("my_variable")), mlc::String("my_variable")));
mlc::String first_format = format_expression_source(mlc::String("my_variable"));
mlc::String second_format = format_expression_source(first_format);
results.push_back(test_runner::assert_eq_str(mlc::String("format round-trip ident"), second_format, mlc::String("my_variable")));
mlc::String integer_round_trip = format_expression_source(format_expression_source(mlc::String("7")));
results.push_back(test_runner::assert_eq_str(mlc::String("format round-trip int"), integer_round_trip, mlc::String("7")));
return results;
}

} // namespace test_formatter
