#include "test_verify_cpp_ast.hpp"

#include "test_runner.hpp"
#include "verify_cpp_ast.hpp"
#include "cpp_ast.hpp"

namespace test_verify_cpp_ast {

using namespace test_runner;
using namespace verify_cpp_ast;
using namespace cpp_ast;

mlc::Array<test_runner::TestResult> verify_cpp_ast_tests() noexcept;

mlc::Array<test_runner::TestResult> verify_cpp_ast_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("cpp_identifier_is_safe accepts Widget"), verify_cpp_ast::cpp_identifier_is_safe(mlc::String("Widget"))));
results.push_back(test_runner::assert_true(mlc::String("cpp_identifier_is_safe rejects semicolon"), !verify_cpp_ast::cpp_identifier_is_safe(mlc::String("foo;bar"))));
results.push_back(test_runner::assert_true(mlc::String("cpp_include_path_is_safe accepts string header"), verify_cpp_ast::cpp_include_path_is_safe(mlc::String("string"))));
results.push_back(test_runner::assert_true(mlc::String("cpp_include_path_is_safe rejects parent segment"), !verify_cpp_ast::cpp_include_path_is_safe(mlc::String("../evil.h"))));
cpp_ast::CppProgram valid_program = cpp_ast::CppProgram{mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(true, mlc::String("vector"))), std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(mlc::String(""), mlc::String("Point"), mlc::Array<std::shared_ptr<cpp_ast::CppField>>{std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{mlc::String("int"), mlc::String("x")})}, false))}};
results.push_back(test_runner::assert_eq_int(mlc::String("verify_cpp_program valid"), verify_cpp_ast::verify_cpp_program(valid_program).size(), 0));
cpp_ast::CppProgram unsafe_include = cpp_ast::CppProgram{mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(false, mlc::String("../bad.h")))}};
results.push_back(test_runner::assert_true(mlc::String("verify_cpp_program rejects unsafe include"), verify_cpp_ast::verify_cpp_program(unsafe_include).size() > 0));
return results;
}

} // namespace test_verify_cpp_ast
