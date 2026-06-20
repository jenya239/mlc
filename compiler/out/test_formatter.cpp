#include "test_formatter.hpp"

#include "test_runner.hpp"
#include "ast_printer.hpp"
#include "format_cli.hpp"
#include "lexer.hpp"
#include "predicates.hpp"
#include "exprs.hpp"
#include "decls.hpp"

namespace test_formatter {

using namespace test_runner;
using namespace ast_printer;
using namespace format_cli;
using namespace lexer;
using namespace predicates;
using namespace exprs;
using namespace decls;

mlc::String format_expression_source(mlc::String source) noexcept;

mlc::String format_program_source(mlc::String source) noexcept;

test_runner::TestResult assert_expression_format_idempotent(mlc::String label, mlc::String source) noexcept;

test_runner::TestResult assert_program_format_idempotent(mlc::String label, mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> formatter_tests() noexcept;

mlc::String format_expression_source(mlc::String source) noexcept{return ast_printer::print_mlc_expr(exprs::parse_expr(predicates::parser_new(lexer::tokenize(source).tokens)).value);}

mlc::String format_program_source(mlc::String source) noexcept{return ast_printer::print_mlc_program(decls::parse_program(lexer::tokenize(source).tokens));}

test_runner::TestResult assert_expression_format_idempotent(mlc::String label, mlc::String source) noexcept{
mlc::String once = format_expression_source(source);
mlc::String twice = format_expression_source(once);
return test_runner::assert_eq_str(label, twice, once);
}

test_runner::TestResult assert_program_format_idempotent(mlc::String label, mlc::String source) noexcept{
mlc::String once = format_program_source(source);
mlc::String twice = format_program_source(once);
return test_runner::assert_eq_str(label, twice, once);
}

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
results.push_back(test_runner::assert_eq_str(mlc::String("format do/end block"), format_expression_source(mlc::String("do\n  let value = 1\n  value\nend")), mlc::String("do\n  let value = 1\n  value\nend")));
results.push_back(test_runner::assert_eq_str(mlc::String("format nested do/end"), format_expression_source(mlc::String("do\n  do\n    1\n  end\nend")), mlc::String("do\n  do\n    1\n  end\nend")));
results.push_back(test_runner::assert_eq_str(mlc::String("format fn do/end body"), format_program_source(mlc::String("fn example() -> i32 = do\n  let value = 1\n  value\nend")), mlc::String("fn example() -> i32 = do\n  let value = 1\n  value\nend")));
results.push_back(test_runner::assert_eq_str(mlc::String("format record inline"), format_expression_source(mlc::String("Point { x: 1 }")), mlc::String("Point { x: 1 }")));
results.push_back(test_runner::assert_eq_str(mlc::String("format record multiline"), format_expression_source(mlc::String("Point { x: 1, y: 2 }")), mlc::String("Point {\n  x: 1,\n  y: 2,\n}")));
results.push_back(test_runner::assert_eq_str(mlc::String("format record aligned fields"), format_expression_source(mlc::String("Point { first: 1, second: 2 }")), mlc::String("Point {\n  first:  1,\n  second: 2,\n}")));
results.push_back(test_runner::assert_eq_str(mlc::String("format record update inline"), format_expression_source(mlc::String("Point { ...base, x: 10 }")), mlc::String("Point { ...base, x: 10 }")));
results.push_back(test_runner::assert_eq_str(mlc::String("format type record multiline"), format_program_source(mlc::String("type Point = { x: i32, y: i32 }")), mlc::String("type Point = {\n  x: i32,\n  y: i32,\n}")));
results.push_back(assert_expression_format_idempotent(mlc::String("idempotent do/end block"), mlc::String("do\n  let value = 1\n  value\nend")));
results.push_back(assert_expression_format_idempotent(mlc::String("idempotent nested do/end"), mlc::String("do\n  do\n    1\n  end\nend")));
results.push_back(assert_expression_format_idempotent(mlc::String("idempotent record inline"), mlc::String("Point { x: 1 }")));
results.push_back(assert_expression_format_idempotent(mlc::String("idempotent record multiline"), mlc::String("Point { x: 1, y: 2 }")));
results.push_back(assert_expression_format_idempotent(mlc::String("idempotent record aligned fields"), mlc::String("Point { first: 1, second: 2 }")));
results.push_back(assert_expression_format_idempotent(mlc::String("idempotent record update inline"), mlc::String("Point { ...base, x: 10 }")));
results.push_back(assert_program_format_idempotent(mlc::String("idempotent type record multiline"), mlc::String("type Point = { x: i32, y: i32 }")));
results.push_back(assert_program_format_idempotent(mlc::String("idempotent fn do/end body"), mlc::String("fn example() -> i32 = do\n  let value = 1\n  value\nend")));
std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> void { auto [formatted] = ok; results.push_back(test_runner::assert_eq_str(mlc::String("format_cli program source"), formatted, mlc::String("fn example() -> i32 = 42"))); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> void { auto [_w0] = err; results.push_back(test_runner::assert_eq_str(mlc::String("format_cli program source"), mlc::String("ok"), mlc::String("failed"))); }
}, format_cli::format_mlc_source_text(mlc::String("fn example() -> i32 = 42"), mlc::String("example.mlc")));
return results;
}

} // namespace test_formatter
