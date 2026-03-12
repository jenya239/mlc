#include "test_cpp_ast.hpp"

#include "test_runner.hpp"
#include "ast.hpp"

namespace test_cpp_ast {

using namespace test_runner;
using namespace ast;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> cpp_ast_tests() noexcept;

mlc::Array<test_runner::TestResult> cpp_ast_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
std::shared_ptr<ast::CppDecl> inc = std::make_shared<ast::CppDecl>(ast::CppInclude(true, mlc::String("string")));
ast::CppProgram prog = ast::CppProgram{mlc::Array<std::shared_ptr<ast::CppDecl>>{inc}};
results.push_back(test_runner::assert_eq_int(mlc::String("CppProgram with 1 decl"), prog.decls.size(), 1));
std::shared_ptr<ast::CppField> field = std::make_shared<ast::CppField>(ast::CppField{mlc::String("int"), mlc::String("x")});
std::shared_ptr<ast::CppDecl> st = std::make_shared<ast::CppDecl>(ast::CppStruct(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::CppField>>{field}));
ast::CppProgram prog2 = ast::CppProgram{mlc::Array<std::shared_ptr<ast::CppDecl>>{st}};
results.push_back(test_runner::assert_true(mlc::String("CppStruct in program"), [&]() -> bool { if (std::holds_alternative<ast::CppStruct>((*prog2.decls[0]))) { auto _v_cppstruct = std::get<ast::CppStruct>((*prog2.decls[0])); auto [_w0, _w1] = _v_cppstruct; return true; } return false; }()));
return results;
}

} // namespace test_cpp_ast
