#include "test_cpp_types.hpp"

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_types.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"

namespace test_cpp_types {

using namespace test_runner;
using namespace cpp_lexer;
using namespace cpp_types;
using namespace cpp_ast;
using namespace print;

mlc::String cpp_type_string(mlc::String source_text) noexcept;

std::shared_ptr<cpp_ast::CppType> cpp_parse_type_node(mlc::String source_text) noexcept;

mlc::Array<test_runner::TestResult> cpp_type_parser_tests() noexcept;

mlc::String cpp_type_string(mlc::String source_text) noexcept{return cpp_types::parse_cpp_type_to_string(cpp_lexer::cpp_tokenize(source_text).tokens, 0).type_string;}

std::shared_ptr<cpp_ast::CppType> cpp_parse_type_node(mlc::String source_text) noexcept{return cpp_types::parse_cpp_type(cpp_lexer::cpp_tokenize(source_text).tokens, 0).type_node;}

mlc::Array<test_runner::TestResult> cpp_type_parser_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_str(mlc::String("int"), cpp_type_string(mlc::String("int")), mlc::String("int")));
results.push_back(test_runner::assert_eq_str(mlc::String("const int*"), cpp_type_string(mlc::String("const int*")), mlc::String("const int*")));
results.push_back(test_runner::assert_eq_str(mlc::String("std::vector<int>"), cpp_type_string(mlc::String("std::vector<int>")), mlc::String("std::vector<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("int&&"), cpp_type_string(mlc::String("int&&")), mlc::String("int&&")));
results.push_back(test_runner::assert_eq_str(mlc::String("const int&"), cpp_type_string(mlc::String("const int&")), mlc::String("const int&")));
results.push_back(test_runner::assert_true(mlc::String("CppTypeTemplate std::vector<int>"), [&]() { if (std::holds_alternative<cpp_ast::CppTypeTemplate>((*cpp_parse_type_node(mlc::String("std::vector<int>"))))) { auto _v_cpptypetemplate = std::get<cpp_ast::CppTypeTemplate>((*cpp_parse_type_node(mlc::String("std::vector<int>")))); auto [name, arguments] = _v_cpptypetemplate; return name == mlc::String("std::vector") && arguments.size() == 1; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("CppTypePtr const int"), [&]() { if (std::holds_alternative<cpp_ast::CppTypePtr>((*cpp_parse_type_node(mlc::String("const int*"))))) { auto _v_cpptypeptr = std::get<cpp_ast::CppTypePtr>((*cpp_parse_type_node(mlc::String("const int*")))); auto [inner] = _v_cpptypeptr; return [&]() { if (std::holds_alternative<cpp_ast::CppTypeConst>((*inner))) { auto _v_cpptypeconst = std::get<cpp_ast::CppTypeConst>((*inner)); auto [inner_type] = _v_cpptypeconst; return [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*inner_type))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*inner_type)); auto [name] = _v_cpptypename; return name == mlc::String("int"); } return false; }(); } return false; }(); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("CppTypeRRef int"), [&]() { if (std::holds_alternative<cpp_ast::CppTypeRRef>((*cpp_parse_type_node(mlc::String("int&&"))))) { auto _v_cpptyperref = std::get<cpp_ast::CppTypeRRef>((*cpp_parse_type_node(mlc::String("int&&")))); auto [inner] = _v_cpptyperref; return [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*inner))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*inner)); auto [name] = _v_cpptypename; return name == mlc::String("int"); } return false; }(); } return false; }()));
results.push_back(test_runner::assert_eq_str(mlc::String("print roundtrip unsigned long int"), print::print_cpp_type(cpp_parse_type_node(mlc::String("unsigned long int"))), mlc::String("unsigned long int")));
return results;
}

} // namespace test_cpp_types
