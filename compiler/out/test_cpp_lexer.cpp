#include "test_cpp_lexer.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"

namespace test_cpp_lexer {

using namespace test_runner;
using namespace lexer;

int cpp_tok_count(mlc::String src) noexcept;

lexer::CppTKind cpp_kind_at(mlc::String src, int i) noexcept;

lexer::CppTKind cpp_first_kind(mlc::String src) noexcept;

mlc::String cpp_ident_at(mlc::String src, int i) noexcept;

mlc::String cpp_str_at(mlc::String src, int i) noexcept;

int cpp_int_at(mlc::String src, int i) noexcept;

mlc::Array<test_runner::TestResult> cpp_lexer_tests() noexcept;

int cpp_tok_count(mlc::String src) noexcept{return lexer::cpp_tokenize(src).tokens.size();}

lexer::CppTKind cpp_kind_at(mlc::String src, int i) noexcept{return lexer::cpp_tokenize(src).tokens[i].kind;}

lexer::CppTKind cpp_first_kind(mlc::String src) noexcept{return cpp_kind_at(src, 0);}

mlc::String cpp_ident_at(mlc::String src, int i) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<lexer::CIdent>(cpp_kind_at(src, i))) { auto _v_cident = std::get<lexer::CIdent>(cpp_kind_at(src, i)); auto [s] = _v_cident; return s; } return mlc::String(""); }();}

mlc::String cpp_str_at(mlc::String src, int i) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<lexer::CLStr>(cpp_kind_at(src, i))) { auto _v_clstr = std::get<lexer::CLStr>(cpp_kind_at(src, i)); auto [s] = _v_clstr; return s; } return mlc::String(""); }();}

int cpp_int_at(mlc::String src, int i) noexcept{return [&]() { if (std::holds_alternative<lexer::CLInt>(cpp_kind_at(src, i))) { auto _v_clint = std::get<lexer::CLInt>(cpp_kind_at(src, i)); auto [n] = _v_clint; return n; } return -1; }();}

mlc::Array<test_runner::TestResult> cpp_lexer_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty → 1 token (CEof)"), cpp_tok_count(mlc::String("")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("'int x' → 3 tokens"), cpp_tok_count(mlc::String("int x")), 3));
results.push_back(test_runner::assert_eq_int(mlc::String("'::'  → 2 tokens"), cpp_tok_count(mlc::String("::")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'->'  → 2 tokens"), cpp_tok_count(mlc::String("->")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'// comment\\nint' → 2 tokens"), cpp_tok_count(mlc::String("// comment\nint")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'/* block */int' → 2 tokens"), cpp_tok_count(mlc::String("/* block */int")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'#include' → 3 tokens"), cpp_tok_count(mlc::String("#include")), 3));
results.push_back(test_runner::assert_true(mlc::String("'auto'       → CKAuto"), [&]() -> bool { if (std::holds_alternative<lexer::CKAuto>(cpp_first_kind(mlc::String("auto")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'const'      → CKConst"), [&]() -> bool { if (std::holds_alternative<lexer::CKConst>(cpp_first_kind(mlc::String("const")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'constexpr'  → CKConstexpr"), [&]() -> bool { if (std::holds_alternative<lexer::CKConstexpr>(cpp_first_kind(mlc::String("constexpr")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'int'        → CKInt"), [&]() -> bool { if (std::holds_alternative<lexer::CKInt>(cpp_first_kind(mlc::String("int")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'void'       → CKVoid"), [&]() -> bool { if (std::holds_alternative<lexer::CKVoid>(cpp_first_kind(mlc::String("void")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'bool'       → CKBool"), [&]() -> bool { if (std::holds_alternative<lexer::CKBool>(cpp_first_kind(mlc::String("bool")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'return'     → CKReturn"), [&]() -> bool { if (std::holds_alternative<lexer::CKReturn>(cpp_first_kind(mlc::String("return")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'struct'     → CKStruct"), [&]() -> bool { if (std::holds_alternative<lexer::CKStruct>(cpp_first_kind(mlc::String("struct")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'using'      → CKUsing"), [&]() -> bool { if (std::holds_alternative<lexer::CKUsing>(cpp_first_kind(mlc::String("using")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'noexcept'   → CKNoexcept"), [&]() -> bool { if (std::holds_alternative<lexer::CKNoexcept>(cpp_first_kind(mlc::String("noexcept")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'::'  → CScope"), [&]() -> bool { if (std::holds_alternative<lexer::CScope>(cpp_first_kind(mlc::String("::")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'->'  → CArrow"), [&]() -> bool { if (std::holds_alternative<lexer::CArrow>(cpp_first_kind(mlc::String("->")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'=='  → CEq"), [&]() -> bool { if (std::holds_alternative<lexer::CEq>(cpp_first_kind(mlc::String("==")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'!='  → CNe"), [&]() -> bool { if (std::holds_alternative<lexer::CNe>(cpp_first_kind(mlc::String("!=")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'<='  → CLe"), [&]() -> bool { if (std::holds_alternative<lexer::CLe>(cpp_first_kind(mlc::String("<=")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'>='  → CGe"), [&]() -> bool { if (std::holds_alternative<lexer::CGe>(cpp_first_kind(mlc::String(">=")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'&&'  → CAnd"), [&]() -> bool { if (std::holds_alternative<lexer::CAnd>(cpp_first_kind(mlc::String("&&")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'||'  → COr"), [&]() -> bool { if (std::holds_alternative<lexer::COr>(cpp_first_kind(mlc::String("||")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'++'  → CInc"), [&]() -> bool { if (std::holds_alternative<lexer::CInc>(cpp_first_kind(mlc::String("++")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'--'  → CDec"), [&]() -> bool { if (std::holds_alternative<lexer::CDec>(cpp_first_kind(mlc::String("--")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'{' → CLBrace"), [&]() -> bool { if (std::holds_alternative<lexer::CLBrace>(cpp_first_kind(mlc::String("{")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'}' → CRBrace"), [&]() -> bool { if (std::holds_alternative<lexer::CRBrace>(cpp_first_kind(mlc::String("}")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'<' → CLAngle"), [&]() -> bool { if (std::holds_alternative<lexer::CLAngle>(cpp_first_kind(mlc::String("<")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'>' → CRAngle"), [&]() -> bool { if (std::holds_alternative<lexer::CRAngle>(cpp_first_kind(mlc::String(">")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_eq_str(mlc::String("'foo' → CIdent(\"foo\")"), cpp_ident_at(mlc::String("foo"), 0), mlc::String("foo")));
results.push_back(test_runner::assert_eq_str(mlc::String("'std::string' [0] → \"std\""), cpp_ident_at(mlc::String("std::string"), 0), mlc::String("std")));
results.push_back(test_runner::assert_true(mlc::String("'std::string' [1] → CScope"), [&]() -> bool { if (std::holds_alternative<lexer::CScope>(cpp_kind_at(mlc::String("std::string"), 1))) {  return true; } return false; }()));
results.push_back(test_runner::assert_eq_str(mlc::String("'std::string' [2] → \"string\""), cpp_ident_at(mlc::String("std::string"), 2), mlc::String("string")));
results.push_back(test_runner::assert_eq_int(mlc::String("'42' → CLInt(42)"), cpp_int_at(mlc::String("42"), 0), 42));
results.push_back(test_runner::assert_eq_int(mlc::String("'0'  → CLInt(0)"), cpp_int_at(mlc::String("0"), 0), 0));
results.push_back(test_runner::assert_eq_str(mlc::String("'\"hello\"' → CLStr(\"hello\")"), cpp_str_at(mlc::String("\"hello\""), 0), mlc::String("hello")));
results.push_back(test_runner::assert_eq_str(mlc::String("escape \\n in string"), cpp_str_at(mlc::String("\"a\\nb\""), 0), mlc::String("a\nb")));
results.push_back(test_runner::assert_eq_str(mlc::String("escaped quote in string"), cpp_str_at(mlc::String("\"a\\\"b\""), 0), mlc::String("a\"b")));
results.push_back(test_runner::assert_true(mlc::String("'#' → COp(\"#\")"), [&]() -> bool { if (std::holds_alternative<lexer::COp>(cpp_first_kind(mlc::String("#")))) { auto _v_cop = std::get<lexer::COp>(cpp_first_kind(mlc::String("#"))); auto [s] = _v_cop; return s == mlc::String("#"); } return false; }()));
return results;
}

} // namespace test_cpp_lexer
