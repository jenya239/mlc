#include "test_cpp_exprs.hpp"

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_exprs.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_exprs {

using namespace test_runner;
using namespace cpp_lexer;
using namespace cpp_exprs;
using namespace cpp_ast;

std::shared_ptr<cpp_ast::CppExpression> cpp_expr_parse(mlc::String source_text) noexcept;

int cpp_expr_parse_error_count(mlc::String source_text) noexcept;

bool cpp_expr_is_int(std::shared_ptr<cpp_ast::CppExpression> expression, int expected) noexcept;

bool cpp_expr_is_hex(std::shared_ptr<cpp_ast::CppExpression> expression, int expected) noexcept;

bool cpp_expr_is_float(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept;

bool cpp_expr_is_char(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept;

bool cpp_expr_is_string(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept;

bool cpp_expr_is_ident(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept;

bool cpp_expr_is_addition(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_binary_mul(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_mul_precedence(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_unary_minus(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_call(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_member(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_pointer_member(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_index(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_ternary(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_true(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

bool cpp_expr_is_false(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

mlc::Array<test_runner::TestResult> cpp_expr_parser_tests() noexcept;

std::shared_ptr<cpp_ast::CppExpression> cpp_expr_parse(mlc::String source_text) noexcept{return cpp_exprs::parse_cpp_expression(cpp_lexer::cpp_tokenize(source_text).tokens, 0).expression;}

int cpp_expr_parse_error_count(mlc::String source_text) noexcept{return cpp_exprs::parse_cpp_expression(cpp_lexer::cpp_tokenize(source_text).tokens, 0).error_message != mlc::String("") ? 1 : 0;}

bool cpp_expr_is_int(std::shared_ptr<cpp_ast::CppExpression> expression, int expected) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppInt>((*expression)._)) { auto _v_cppint = std::get<cpp_ast::CppInt>((*expression)._); auto [value] = _v_cppint; return value == expected; } return false; }();}

bool cpp_expr_is_hex(std::shared_ptr<cpp_ast::CppExpression> expression, int expected) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppInt>((*expression)._)) { auto _v_cppint = std::get<cpp_ast::CppInt>((*expression)._); auto [value] = _v_cppint; return value == expected; } return false; }();}

bool cpp_expr_is_float(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppFloatLiteral>((*expression)._)) { auto _v_cppfloatliteral = std::get<cpp_ast::CppFloatLiteral>((*expression)._); auto [text] = _v_cppfloatliteral; return text == expected; } return false; }();}

bool cpp_expr_is_char(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppCharLiteral>((*expression)._)) { auto _v_cppcharliteral = std::get<cpp_ast::CppCharLiteral>((*expression)._); auto [character] = _v_cppcharliteral; return character == expected; } return false; }();}

bool cpp_expr_is_string(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppStr>((*expression)._)) { auto _v_cppstr = std::get<cpp_ast::CppStr>((*expression)._); auto [text] = _v_cppstr; return text == expected; } return false; }();}

bool cpp_expr_is_ident(std::shared_ptr<cpp_ast::CppExpression> expression, mlc::String expected) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppIdent>((*expression)._)) { auto _v_cppident = std::get<cpp_ast::CppIdent>((*expression)._); auto [name] = _v_cppident; return name == expected; } return false; }();}

bool cpp_expr_is_addition(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppBinary>((*expression)._)) { auto _v_cppbinary = std::get<cpp_ast::CppBinary>((*expression)._); auto [operation, left, right] = _v_cppbinary; return operation == mlc::String("+") && cpp_expr_is_int(left, 1) && cpp_expr_is_int(right, 2); } return false; }();
}

bool cpp_expr_is_binary_mul(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppBinary>((*expression)._)) { auto _v_cppbinary = std::get<cpp_ast::CppBinary>((*expression)._); auto [operation, _w0, _w1] = _v_cppbinary; return operation == mlc::String("*"); } return false; }();
}

bool cpp_expr_is_mul_precedence(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppBinary>((*expression)._)) { auto _v_cppbinary = std::get<cpp_ast::CppBinary>((*expression)._); auto [operation, left, right] = _v_cppbinary; return operation == mlc::String("+") && cpp_expr_is_int(left, 1) && cpp_expr_is_binary_mul(right); } return false; }();
}

bool cpp_expr_is_unary_minus(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppUnary>((*expression)._)) { auto _v_cppunary = std::get<cpp_ast::CppUnary>((*expression)._); auto [operation, inner] = _v_cppunary; return operation == mlc::String("-") && cpp_expr_is_ident(inner, mlc::String("x")); } return false; }();
}

bool cpp_expr_is_call(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppCall>((*expression)._)) { auto _v_cppcall = std::get<cpp_ast::CppCall>((*expression)._); auto [callee, arguments] = _v_cppcall; return cpp_expr_is_ident(callee, mlc::String("foo")) && arguments.size() == 2; } return false; }();
}

bool cpp_expr_is_member(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppMember>((*expression)._)) { auto _v_cppmember = std::get<cpp_ast::CppMember>((*expression)._); auto [object, member_name, is_pointer] = _v_cppmember; return cpp_expr_is_ident(object, mlc::String("obj")) && member_name == mlc::String("field") && !is_pointer; } return false; }();
}

bool cpp_expr_is_pointer_member(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppMember>((*expression)._)) { auto _v_cppmember = std::get<cpp_ast::CppMember>((*expression)._); auto [object, member_name, is_pointer] = _v_cppmember; return cpp_expr_is_ident(object, mlc::String("ptr")) && member_name == mlc::String("method") && is_pointer; } return false; }();
}

bool cpp_expr_is_index(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppIndex>((*expression)._)) { auto _v_cppindex = std::get<cpp_ast::CppIndex>((*expression)._); auto [object, index] = _v_cppindex; return cpp_expr_is_ident(object, mlc::String("arr")) && cpp_expr_is_int(index, 0); } return false; }();
}

bool cpp_expr_is_ternary(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppTernary>((*expression)._)) { auto _v_cppternary = std::get<cpp_ast::CppTernary>((*expression)._); auto [condition, then_branch, else_branch] = _v_cppternary; return cpp_expr_is_ident(condition, mlc::String("a")) && cpp_expr_is_ident(then_branch, mlc::String("b")) && cpp_expr_is_ident(else_branch, mlc::String("c")); } return false; }();
}

bool cpp_expr_is_true(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppBool>((*expression)._)) { auto _v_cppbool = std::get<cpp_ast::CppBool>((*expression)._); auto [value] = _v_cppbool; return value; } return false; }();}

bool cpp_expr_is_false(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppBool>((*expression)._)) { auto _v_cppbool = std::get<cpp_ast::CppBool>((*expression)._); auto [value] = _v_cppbool; return !value; } return false; }();}

mlc::Array<test_runner::TestResult> cpp_expr_parser_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("integer literal"), cpp_expr_is_int(cpp_expr_parse(mlc::String("42")), 42)));
results.push_back(test_runner::assert_true(mlc::String("hex literal"), cpp_expr_is_hex(cpp_expr_parse(mlc::String("0xFF")), 255)));
results.push_back(test_runner::assert_true(mlc::String("float literal"), cpp_expr_is_float(cpp_expr_parse(mlc::String("3.14f")), mlc::String("3.14f"))));
results.push_back(test_runner::assert_true(mlc::String("char literal"), cpp_expr_is_char(cpp_expr_parse(mlc::String("'a'")), mlc::String("a"))));
results.push_back(test_runner::assert_true(mlc::String("string literal"), cpp_expr_is_string(cpp_expr_parse(mlc::String("\"hello\"")), mlc::String("hello"))));
results.push_back(test_runner::assert_true(mlc::String("true keyword"), cpp_expr_is_true(cpp_expr_parse(mlc::String("true")))));
results.push_back(test_runner::assert_true(mlc::String("false keyword"), cpp_expr_is_false(cpp_expr_parse(mlc::String("false")))));
results.push_back(test_runner::assert_true(mlc::String("nullptr keyword"), cpp_expr_is_ident(cpp_expr_parse(mlc::String("nullptr")), mlc::String("nullptr"))));
results.push_back(test_runner::assert_true(mlc::String("identifier"), cpp_expr_is_ident(cpp_expr_parse(mlc::String("foo")), mlc::String("foo"))));
results.push_back(test_runner::assert_true(mlc::String("binary addition"), cpp_expr_is_addition(cpp_expr_parse(mlc::String("1 + 2")))));
results.push_back(test_runner::assert_true(mlc::String("multiplication binds tighter than addition"), cpp_expr_is_mul_precedence(cpp_expr_parse(mlc::String("1 + 2 * 3")))));
results.push_back(test_runner::assert_true(mlc::String("unary minus"), cpp_expr_is_unary_minus(cpp_expr_parse(mlc::String("-x")))));
results.push_back(test_runner::assert_true(mlc::String("function call"), cpp_expr_is_call(cpp_expr_parse(mlc::String("foo(1, 2)")))));
results.push_back(test_runner::assert_true(mlc::String("member access"), cpp_expr_is_member(cpp_expr_parse(mlc::String("obj.field")))));
results.push_back(test_runner::assert_true(mlc::String("pointer member access"), cpp_expr_is_pointer_member(cpp_expr_parse(mlc::String("ptr->method")))));
results.push_back(test_runner::assert_true(mlc::String("index access"), cpp_expr_is_index(cpp_expr_parse(mlc::String("arr[0]")))));
results.push_back(test_runner::assert_true(mlc::String("ternary expression"), cpp_expr_is_ternary(cpp_expr_parse(mlc::String("a ? b : c")))));
results.push_back(test_runner::assert_eq_int(mlc::String("parse error on garbage"), cpp_expr_parse_error_count(mlc::String("@@@")), 1));
return results;
}

} // namespace test_cpp_exprs
