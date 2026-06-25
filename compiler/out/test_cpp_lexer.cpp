#include "test_cpp_lexer.hpp"

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_tokens.hpp"

namespace test_cpp_lexer {

using namespace test_runner;
using namespace cpp_lexer;
using namespace cpp_tokens;

int cpp_tok_count(mlc::String source_text) noexcept;

cpp_tokens::CppTokenKind cpp_kind_at(mlc::String source_text, int index) noexcept;

mlc::String cpp_leading_trivia_at(mlc::String source_text, int index) noexcept;

mlc::String cpp_trailing_trivia_at(mlc::String source_text, int index) noexcept;

cpp_tokens::CppTokenKind cpp_first_kind(mlc::String source_text) noexcept;

mlc::String cpp_ident_at(mlc::String source_text, int index) noexcept;

mlc::String cpp_str_at(mlc::String source_text, int index) noexcept;

int cpp_int_at(mlc::String source_text, int index) noexcept;

mlc::Array<test_runner::TestResult> cpp_lexer_tests() noexcept;

int cpp_tok_count(mlc::String source_text) noexcept{return cpp_lexer::cpp_tokenize(source_text).tokens.size();}

cpp_tokens::CppTokenKind cpp_kind_at(mlc::String source_text, int index) noexcept{return cpp_lexer::cpp_tokenize(source_text).tokens[index].kind;}

mlc::String cpp_leading_trivia_at(mlc::String source_text, int index) noexcept{return cpp_lexer::cpp_tokenize(source_text).tokens[index].leading_trivia;}

mlc::String cpp_trailing_trivia_at(mlc::String source_text, int index) noexcept{return cpp_lexer::cpp_tokenize(source_text).tokens[index].trailing_trivia;}

cpp_tokens::CppTokenKind cpp_first_kind(mlc::String source_text) noexcept{return cpp_kind_at(source_text, 0);}

mlc::String cpp_ident_at(mlc::String source_text, int index) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CIdent>(cpp_kind_at(source_text, index))) { auto _v_cident = std::get<cpp_tokens::CIdent>(cpp_kind_at(source_text, index)); auto [identifier] = _v_cident; return identifier; } return mlc::String(""); }();}

mlc::String cpp_str_at(mlc::String source_text, int index) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CLStr>(cpp_kind_at(source_text, index))) { auto _v_clstr = std::get<cpp_tokens::CLStr>(cpp_kind_at(source_text, index)); auto [string_value] = _v_clstr; return string_value; } return mlc::String(""); }();}

int cpp_int_at(mlc::String source_text, int index) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLInt>(cpp_kind_at(source_text, index))) { auto _v_clint = std::get<cpp_tokens::CLInt>(cpp_kind_at(source_text, index)); auto [integer_value] = _v_clint; return integer_value; } return -1; }();}

mlc::Array<test_runner::TestResult> cpp_lexer_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty - 1 token (CEof)"), cpp_tok_count(mlc::String("")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("'int x' - 3 tokens"), cpp_tok_count(mlc::String("int x")), 3));
results.push_back(test_runner::assert_eq_int(mlc::String("'::'  - 2 tokens"), cpp_tok_count(mlc::String("::")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'->'  - 2 tokens"), cpp_tok_count(mlc::String("->")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'// comment\\nint' - 2 tokens"), cpp_tok_count(mlc::String("// comment\nint")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'/* block */int' - 2 tokens"), cpp_tok_count(mlc::String("/* block */int")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("'#include' - 3 tokens"), cpp_tok_count(mlc::String("#include")), 3));
results.push_back(test_runner::assert_true(mlc::String("'auto'       - CKAuto"), [&]() { if (std::holds_alternative<cpp_tokens::CKAuto>(cpp_first_kind(mlc::String("auto")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'const'      - CKConst"), [&]() { if (std::holds_alternative<cpp_tokens::CKConst>(cpp_first_kind(mlc::String("const")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'constexpr'  - CKConstexpr"), [&]() { if (std::holds_alternative<cpp_tokens::CKConstexpr>(cpp_first_kind(mlc::String("constexpr")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'int'        - CKInt"), [&]() { if (std::holds_alternative<cpp_tokens::CKInt>(cpp_first_kind(mlc::String("int")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'void'       - CKVoid"), [&]() { if (std::holds_alternative<cpp_tokens::CKVoid>(cpp_first_kind(mlc::String("void")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'bool'       - CKBool"), [&]() { if (std::holds_alternative<cpp_tokens::CKBool>(cpp_first_kind(mlc::String("bool")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'return'     - CKReturn"), [&]() { if (std::holds_alternative<cpp_tokens::CKReturn>(cpp_first_kind(mlc::String("return")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'struct'     - CKStruct"), [&]() { if (std::holds_alternative<cpp_tokens::CKStruct>(cpp_first_kind(mlc::String("struct")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'using'      - CKUsing"), [&]() { if (std::holds_alternative<cpp_tokens::CKUsing>(cpp_first_kind(mlc::String("using")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'noexcept'   - CKNoexcept"), [&]() { if (std::holds_alternative<cpp_tokens::CKNoexcept>(cpp_first_kind(mlc::String("noexcept")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'::'  - CScope"), [&]() { if (std::holds_alternative<cpp_tokens::CScope>(cpp_first_kind(mlc::String("::")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'->'  - CArrow"), [&]() { if (std::holds_alternative<cpp_tokens::CArrow>(cpp_first_kind(mlc::String("->")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'=='  - CEq"), [&]() { if (std::holds_alternative<cpp_tokens::CEq>(cpp_first_kind(mlc::String("==")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'!='  - CNe"), [&]() { if (std::holds_alternative<cpp_tokens::CNe>(cpp_first_kind(mlc::String("!=")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'<='  - CLe"), [&]() { if (std::holds_alternative<cpp_tokens::CLe>(cpp_first_kind(mlc::String("<=")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'>='  - CGe"), [&]() { if (std::holds_alternative<cpp_tokens::CGe>(cpp_first_kind(mlc::String(">=")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'&&'  - CAnd"), [&]() { if (std::holds_alternative<cpp_tokens::CAnd>(cpp_first_kind(mlc::String("&&")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'||'  - COr"), [&]() { if (std::holds_alternative<cpp_tokens::COr>(cpp_first_kind(mlc::String("||")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'++'  - CInc"), [&]() { if (std::holds_alternative<cpp_tokens::CInc>(cpp_first_kind(mlc::String("++")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'--'  - CDec"), [&]() { if (std::holds_alternative<cpp_tokens::CDec>(cpp_first_kind(mlc::String("--")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'{' - CLBrace"), [&]() { if (std::holds_alternative<cpp_tokens::CLBrace>(cpp_first_kind(mlc::String("{")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'}' - CRBrace"), [&]() { if (std::holds_alternative<cpp_tokens::CRBrace>(cpp_first_kind(mlc::String("}")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'<' - CLAngle"), [&]() { if (std::holds_alternative<cpp_tokens::CLAngle>(cpp_first_kind(mlc::String("<")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'>' - CRAngle"), [&]() { if (std::holds_alternative<cpp_tokens::CRAngle>(cpp_first_kind(mlc::String(">")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_eq_str(mlc::String("'foo' - CIdent(\"foo\")"), cpp_ident_at(mlc::String("foo"), 0), mlc::String("foo")));
results.push_back(test_runner::assert_eq_str(mlc::String("'std::string' [0] - \"std\""), cpp_ident_at(mlc::String("std::string"), 0), mlc::String("std")));
results.push_back(test_runner::assert_true(mlc::String("'std::string' [1] - CScope"), [&]() { if (std::holds_alternative<cpp_tokens::CScope>(cpp_kind_at(mlc::String("std::string"), 1))) {  return true; } return false; }()));
results.push_back(test_runner::assert_eq_str(mlc::String("'std::string' [2] - \"string\""), cpp_ident_at(mlc::String("std::string"), 2), mlc::String("string")));
results.push_back(test_runner::assert_eq_int(mlc::String("'42' - CLInt(42)"), cpp_int_at(mlc::String("42"), 0), 42));
results.push_back(test_runner::assert_eq_int(mlc::String("'0'  - CLInt(0)"), cpp_int_at(mlc::String("0"), 0), 0));
results.push_back(test_runner::assert_eq_str(mlc::String("'\"hello\"' - CLStr(\"hello\")"), cpp_str_at(mlc::String("\"hello\""), 0), mlc::String("hello")));
results.push_back(test_runner::assert_eq_str(mlc::String("escape \\n in string"), cpp_str_at(mlc::String("\"a\\nb\""), 0), mlc::String("a\nb")));
results.push_back(test_runner::assert_eq_str(mlc::String("escaped quote in string"), cpp_str_at(mlc::String("\"a\\\"b\""), 0), mlc::String("a\"b")));
results.push_back(test_runner::assert_true(mlc::String("'#' - CHash"), [&]() { if (std::holds_alternative<cpp_tokens::CHash>(cpp_first_kind(mlc::String("#")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'class' - CKClass"), [&]() { if (std::holds_alternative<cpp_tokens::CKClass>(cpp_first_kind(mlc::String("class")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'enum' - CKEnum"), [&]() { if (std::holds_alternative<cpp_tokens::CKEnum>(cpp_first_kind(mlc::String("enum")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("'extern' - CKExtern"), [&]() { if (std::holds_alternative<cpp_tokens::CKExtern>(cpp_first_kind(mlc::String("extern")))) {  return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("roundtrip kinds - int x"), cpp_lexer::cpp_lex_roundtrip_preserves_kinds(mlc::String("int x"))));
results.push_back(test_runner::assert_true(mlc::String("roundtrip kinds - std::string"), cpp_lexer::cpp_lex_roundtrip_preserves_kinds(mlc::String("std::string"))));
results.push_back(test_runner::assert_true(mlc::String("roundtrip kinds - #include <vector>"), cpp_lexer::cpp_lex_roundtrip_preserves_kinds(mlc::String("#include <vector>"))));
results.push_back(test_runner::assert_eq_str(mlc::String("tokens_to_source - int x"), cpp_tokens::cpp_tokens_to_source(cpp_lexer::cpp_tokenize(mlc::String("int x")).tokens), mlc::String("int x")));
results.push_back(test_runner::assert_eq_str(mlc::String("line comment leading trivia"), cpp_leading_trivia_at(mlc::String("// comment\nint"), 0), mlc::String("// comment\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("block comment leading trivia"), cpp_leading_trivia_at(mlc::String("/* block */int"), 0), mlc::String("/* block */")));
results.push_back(test_runner::assert_eq_str(mlc::String("#pragma once leading trivia"), cpp_leading_trivia_at(mlc::String("#pragma once\nint"), 0), mlc::String("#pragma once\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("space between tokens - trailing on int"), cpp_trailing_trivia_at(mlc::String("int x"), 0), mlc::String(" ")));
results.push_back(test_runner::assert_true(mlc::String("roundtrip source - int x"), cpp_lexer::cpp_lex_roundtrip_preserves_source(mlc::String("int x"))));
results.push_back(test_runner::assert_true(mlc::String("roundtrip source - comment"), cpp_lexer::cpp_lex_roundtrip_preserves_source(mlc::String("// comment\nint x"))));
results.push_back(test_runner::assert_true(mlc::String("roundtrip source - pragma once"), cpp_lexer::cpp_lex_roundtrip_preserves_source(mlc::String("#pragma once\nint x"))));
results.push_back(test_runner::assert_true(mlc::String("char literal - CLChar"), [&]() { if (std::holds_alternative<cpp_tokens::CLChar>(cpp_first_kind(mlc::String("'a'")))) { auto _v_clchar = std::get<cpp_tokens::CLChar>(cpp_first_kind(mlc::String("'a'"))); auto [character] = _v_clchar; return character == mlc::String("a"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("float literal - CLFloat"), [&]() { if (std::holds_alternative<cpp_tokens::CLFloat>(cpp_first_kind(mlc::String("3.14f")))) { auto _v_clfloat = std::get<cpp_tokens::CLFloat>(cpp_first_kind(mlc::String("3.14f"))); auto [text] = _v_clfloat; return text == mlc::String("3.14f"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("hex literal - CLHex"), [&]() { if (std::holds_alternative<cpp_tokens::CLHex>(cpp_first_kind(mlc::String("0xFF")))) { auto _v_clhex = std::get<cpp_tokens::CLHex>(cpp_first_kind(mlc::String("0xFF"))); auto [text] = _v_clhex; return text == mlc::String("0xFF"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("roundtrip source - char literal"), cpp_lexer::cpp_lex_roundtrip_preserves_source(mlc::String("'a'"))));
results.push_back(test_runner::assert_true(mlc::String("roundtrip source - hex literal"), cpp_lexer::cpp_lex_roundtrip_preserves_source(mlc::String("0xFF"))));
return results;
}

} // namespace test_cpp_lexer
