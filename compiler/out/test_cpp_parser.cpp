#include "test_cpp_parser.hpp"

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_parser {

using namespace test_runner;
using namespace cpp_lexer;
using namespace cpp_parser;
using namespace cpp_ast;

int cpp_parse_decl_count(mlc::String source_text) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> cpp_parse_first(mlc::String source_text) noexcept;

int cpp_parse_error_count(mlc::String source_text) noexcept;

mlc::Array<test_runner::TestResult> cpp_parser_tests() noexcept;

int cpp_parse_decl_count(mlc::String source_text) noexcept{return cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program.declarations.size();}

std::shared_ptr<cpp_ast::CppDeclaration> cpp_parse_first(mlc::String source_text) noexcept{
cpp_ast::CppProgram program = cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program;
return program.declarations.size() > 0 ? program.declarations[0] : std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(true, mlc::String("")));
}

int cpp_parse_error_count(mlc::String source_text) noexcept{return cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).errors.size();}

mlc::Array<test_runner::TestResult> cpp_parser_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("#include <string> - 1 decl"), cpp_parse_decl_count(mlc::String("#include <string>")), 1));
results.push_back(test_runner::assert_true(mlc::String("#include <string> - CppInclude angle"), [&]() { if (std::holds_alternative<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include <string>"))))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include <string>")))); auto [is_angle, path] = _v_cppinclude; return is_angle && path == mlc::String("string"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("#include \"foo.h\" - 1 decl"), cpp_parse_decl_count(mlc::String("#include \"foo.h\"")), 1));
results.push_back(test_runner::assert_true(mlc::String("#include \"foo.h\" - CppInclude quote"), [&]() { if (std::holds_alternative<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include \"foo.h\""))))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include \"foo.h\"")))); auto [is_angle, path] = _v_cppinclude; return !is_angle && path == mlc::String("foo.h"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("using X = int; - 1 decl"), cpp_parse_decl_count(mlc::String("using X = int;")), 1));
results.push_back(test_runner::assert_true(mlc::String("using X = int; - CppUsing"), [&]() { if (std::holds_alternative<cpp_ast::CppUsing>((*cpp_parse_first(mlc::String("using X = int;"))))) { auto _v_cppusing = std::get<cpp_ast::CppUsing>((*cpp_parse_first(mlc::String("using X = int;")))); auto [alias, type_string] = _v_cppusing; return alias == mlc::String("X") && type_string == mlc::String("int"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("struct S { int x; }; - 1 decl"), cpp_parse_decl_count(mlc::String("struct S { int x; };")), 1));
results.push_back(test_runner::assert_true(mlc::String("struct S { int x; }; - CppStruct"), [&]() { if (std::holds_alternative<cpp_ast::CppStruct>((*cpp_parse_first(mlc::String("struct S { int x; };"))))) { auto _v_cppstruct = std::get<cpp_ast::CppStruct>((*cpp_parse_first(mlc::String("struct S { int x; };")))); auto [_w0, name, _w1, _w2] = _v_cppstruct; return name == mlc::String("S"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("class Widget; - CppForwardDecl"), [&]() { if (std::holds_alternative<cpp_ast::CppForwardDecl>((*cpp_parse_first(mlc::String("class Widget;"))))) { auto _v_cppforwarddecl = std::get<cpp_ast::CppForwardDecl>((*cpp_parse_first(mlc::String("class Widget;")))); auto [kind, name] = _v_cppforwarddecl; return kind == mlc::String("class") && name == mlc::String("Widget"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("int add(int a, int b); - 1 decl"), cpp_parse_decl_count(mlc::String("int add(int a, int b);")), 1));
results.push_back(test_runner::assert_true(mlc::String("int add(int a, int b); - CppFnProto"), [&]() { if (std::holds_alternative<cpp_ast::CppFnProto>((*cpp_parse_first(mlc::String("int add(int a, int b);"))))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*cpp_parse_first(mlc::String("int add(int a, int b);")))); auto [_w0, return_type, name, parameters] = _v_cppfnproto; return return_type == mlc::String("int") && name == mlc::String("add") && parameters.size() == 2; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("enum Color { Red, Green }; - CppVariant"), [&]() { if (std::holds_alternative<cpp_ast::CppVariant>((*cpp_parse_first(mlc::String("enum Color { Red, Green };"))))) { auto _v_cppvariant = std::get<cpp_ast::CppVariant>((*cpp_parse_first(mlc::String("enum Color { Red, Green };")))); auto [_w0, name, arms] = _v_cppvariant; return name == mlc::String("Color") && arms.size() == 2; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("namespace foo { int x; } - CppNamespace"), [&]() { if (std::holds_alternative<cpp_ast::CppNamespace>((*cpp_parse_first(mlc::String("namespace foo { int x; }"))))) { auto _v_cppnamespace = std::get<cpp_ast::CppNamespace>((*cpp_parse_first(mlc::String("namespace foo { int x; }")))); auto [name, inner] = _v_cppnamespace; return name == mlc::String("foo") && inner.size() == 1; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("error recovery - garbage then ok"), cpp_parse_error_count(mlc::String("??? garbage; int ok();")) > 0 && cpp_parse_decl_count(mlc::String("??? garbage; int ok();")) == 1));
return results;
}

} // namespace test_cpp_parser
