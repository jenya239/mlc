#include "test_parser.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast.hpp"

namespace test_parser {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace ast;
using namespace ast_tokens;

ast::Program parse_source(mlc::String source) noexcept;

int decl_count(mlc::String source) noexcept;

std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> parser_tests() noexcept;

ast::Program parse_source(mlc::String source) noexcept{return decls::parse_program(lexer::tokenize(source).tokens);}

int decl_count(mlc::String source) noexcept{return parse_source(source).decls.size();}

std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept{return parse_source(source).decls[0];}

mlc::Array<test_runner::TestResult> parser_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty source - 0 decls"), decl_count(mlc::String("")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("single fn decl - 1 decl"), decl_count(mlc::String("fn f() -> i32 = 42")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("two fn decls - 2 decls"), decl_count(mlc::String("fn f() -> i32 = 1\nfn g() -> i32 = 2")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("type decl - 1 decl"), decl_count(mlc::String("type Color = Red | Green | Blue")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with params - 1 decl"), decl_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with do block - 1 decl"), decl_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("type + fn - 2 decls"), decl_count(mlc::String("type Color = Red\nfn f() -> i32 = 1")), 2));
results.push_back(test_runner::assert_true(mlc::String("fn decl is DeclFn"), [&]() { if (std::holds_alternative<ast::DeclFn>((*first_decl(mlc::String("fn f() -> i32 = 42"))))) { auto _v_declfn = std::get<ast::DeclFn>((*first_decl(mlc::String("fn f() -> i32 = 42")))); auto [_w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("type decl is DeclType"), [&]() { if (std::holds_alternative<ast::DeclType>((*first_decl(mlc::String("type Color = Red | Green"))))) { auto _v_decltype = std::get<ast::DeclType>((*first_decl(mlc::String("type Color = Red | Green")))); auto [_w0, _w1, _w2] = _v_decltype; return true; } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with if expr - 1 decl"), decl_count(mlc::String("fn f(x: i32) -> i32 = if x > 0 then 1 else 0 end")), 1));
return results;
}

} // namespace test_parser
