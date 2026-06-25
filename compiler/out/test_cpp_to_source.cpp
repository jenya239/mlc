#include "test_cpp_to_source.hpp"

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "to_source.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_to_source {

using namespace test_runner;
using namespace cpp_lexer;
using namespace to_source;
using namespace cpp_parser;
using namespace cpp_ast;

mlc::Array<test_runner::TestResult> cpp_to_source_tests() noexcept;

mlc::Array<test_runner::TestResult> cpp_to_source_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_str(mlc::String("tokens span - include"), to_source::cpp_tokens_span_to_source(cpp_lexer::cpp_tokenize(mlc::String("#include <vector>")).tokens, 0, 5), mlc::String("#include <vector>")));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - include"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("#include <vector>"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - line comment"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("// header\n#include <vector>"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - pragma once"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("#pragma once\nstruct S { int x; };"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - using"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("using X = int;"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - fn proto"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("int add(int a, int b);"))));
results.push_back(test_runner::assert_true(mlc::String("lexer roundtrip - comment"), cpp_lexer::cpp_lex_roundtrip_preserves_source(mlc::String("// c\nint x"))));
results.push_back(test_runner::assert_eq_str(mlc::String("canonical parse to_source - include"), to_source::cpp_parse_to_source_canonical(mlc::String("#include <string>")), mlc::String("#include <string>\n")));
results.push_back(test_runner::assert_true(mlc::String("fragment program roundtrip"), to_source::cpp_program_to_source(to_source::cpp_program_from_tokens_preserving_trivia(cpp_lexer::cpp_tokenize(mlc::String("int x;")).tokens)) == mlc::String("int x;")));
results.push_back(test_runner::assert_true(mlc::String("fragment decl kind"), [&]() { if (std::holds_alternative<cpp_ast::CppDeclarationFragment>((*cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(mlc::String("int x;")).tokens).program.declarations[0]))) { auto _v_cppdeclarationfragment = std::get<cpp_ast::CppDeclarationFragment>((*cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(mlc::String("int x;")).tokens).program.declarations[0])); auto [code] = _v_cppdeclarationfragment; return code == mlc::String("int x;"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - fn proto"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("int add(int a, int b);"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - typedef"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("typedef unsigned long size_t;"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - template decl"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("template<typename T> T identity(T value);"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - extern block"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("extern \"C\" { int x; }"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - attribute decl"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("[[nodiscard]] int fn();"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - class access"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("class Foo { public: int x; };"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - enum"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("enum Color { Red, Green };"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - namespace"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("namespace box { int item; }"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - struct two fields"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("struct Pair { int left; int right; };"))));
results.push_back(test_runner::assert_true(mlc::String("trivia roundtrip - block comment"), to_source::cpp_source_roundtrip_preserving_trivia(mlc::String("/* header */\nusing Alias = int;"))));
return results;
}

} // namespace test_cpp_to_source
