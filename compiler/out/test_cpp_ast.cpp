#include "test_cpp_ast.hpp"

#include "test_runner.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_ast {

using namespace test_runner;
using namespace cpp_ast;

mlc::Array<test_runner::TestResult> cpp_ast_tests() noexcept;

mlc::Array<test_runner::TestResult> cpp_ast_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
std::shared_ptr<cpp_ast::CppDeclaration> inc = std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(true, mlc::String("string")));
cpp_ast::CppProgram program = cpp_ast::CppProgram{mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{inc}};
results.push_back(test_runner::assert_eq_int(mlc::String("CppProgram with 1 decl"), program.declarations.size(), 1));
std::shared_ptr<cpp_ast::CppField> field = std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{mlc::String("int"), mlc::String("x")});
std::shared_ptr<cpp_ast::CppDeclaration> struct_declaration = std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(mlc::String(""), mlc::String("Point"), mlc::Array<std::shared_ptr<cpp_ast::CppField>>{field}, false));
cpp_ast::CppProgram program_with_struct = cpp_ast::CppProgram{mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{struct_declaration}};
results.push_back(test_runner::assert_true(mlc::String("CppStruct in program"), [&]() { if (std::holds_alternative<cpp_ast::CppStruct>((*program_with_struct.declarations[0]))) { auto _v_cppstruct = std::get<cpp_ast::CppStruct>((*program_with_struct.declarations[0])); auto [_w0, _w1, _w2, _w3] = _v_cppstruct; return true; } return false; }()));
std::shared_ptr<cpp_ast::CppType> vector_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("std::vector")));
std::shared_ptr<cpp_ast::CppType> int_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("int")));
std::shared_ptr<cpp_ast::CppType> vector_int = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeTemplate(mlc::String("std::vector"), mlc::Array<std::shared_ptr<cpp_ast::CppType>>{int_type}));
std::shared_ptr<cpp_ast::CppType> int_ref = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRef(int_type));
results.push_back(test_runner::assert_true(mlc::String("CppTypeName"), [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*vector_type))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*vector_type)); auto [name] = _v_cpptypename; return name == mlc::String("std::vector"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("CppTypeTemplate"), [&]() { if (std::holds_alternative<cpp_ast::CppTypeTemplate>((*vector_int))) { auto _v_cpptypetemplate = std::get<cpp_ast::CppTypeTemplate>((*vector_int)); auto [name, args] = _v_cpptypetemplate; return name == mlc::String("std::vector") && args.size() == 1; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("CppTypeRef"), [&]() { if (std::holds_alternative<cpp_ast::CppTypeRef>((*int_ref))) { auto _v_cpptyperef = std::get<cpp_ast::CppTypeRef>((*int_ref)); auto [inner] = _v_cpptyperef; return [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*inner))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*inner)); auto [name] = _v_cpptypename; return name == mlc::String("int"); } return false; }(); } return false; }()));
std::shared_ptr<cpp_ast::CppType> int_pointer = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypePtr(int_type));
std::shared_ptr<cpp_ast::CppType> const_int = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeConst(int_type));
std::shared_ptr<cpp_ast::CppType> int_rvalue_reference = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRRef(int_type));
results.push_back(test_runner::assert_true(mlc::String("CppTypePtr"), [&]() { if (std::holds_alternative<cpp_ast::CppTypePtr>((*int_pointer))) { auto _v_cpptypeptr = std::get<cpp_ast::CppTypePtr>((*int_pointer)); auto [inner] = _v_cpptypeptr; return [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*inner))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*inner)); auto [name] = _v_cpptypename; return name == mlc::String("int"); } return false; }(); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("CppTypeConst"), [&]() { if (std::holds_alternative<cpp_ast::CppTypeConst>((*const_int))) { auto _v_cpptypeconst = std::get<cpp_ast::CppTypeConst>((*const_int)); auto [inner] = _v_cpptypeconst; return [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*inner))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*inner)); auto [name] = _v_cpptypename; return name == mlc::String("int"); } return false; }(); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("CppTypeRRef"), [&]() { if (std::holds_alternative<cpp_ast::CppTypeRRef>((*int_rvalue_reference))) { auto _v_cpptyperref = std::get<cpp_ast::CppTypeRRef>((*int_rvalue_reference)); auto [inner] = _v_cpptyperref; return [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*inner))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*inner)); auto [name] = _v_cpptypename; return name == mlc::String("int"); } return false; }(); } return false; }()));
std::shared_ptr<cpp_ast::CppDeclaration> class_forward = std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppForwardDecl(mlc::String("class"), mlc::String("Widget")));
results.push_back(test_runner::assert_true(mlc::String("CppForwardDecl"), [&]() { if (std::holds_alternative<cpp_ast::CppForwardDecl>((*class_forward))) { auto _v_cppforwarddecl = std::get<cpp_ast::CppForwardDecl>((*class_forward)); auto [kind, name] = _v_cppforwarddecl; return kind == mlc::String("class") && name == mlc::String("Widget"); } return false; }()));
std::shared_ptr<cpp_ast::CppExpression> flag = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBool(true));
results.push_back(test_runner::assert_true(mlc::String("CppBool"), [&]() { if (std::holds_alternative<cpp_ast::CppBool>((*flag)._)) { auto _v_cppbool = std::get<cpp_ast::CppBool>((*flag)._); auto [value] = _v_cppbool; return value; } return false; }()));
std::shared_ptr<cpp_ast::CppExpression> condition = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("ready")));
std::shared_ptr<cpp_ast::CppExpression> then_branch = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInt(1));
std::shared_ptr<cpp_ast::CppExpression> else_branch = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInt(0));
std::shared_ptr<cpp_ast::CppExpression> ternary = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppTernary(condition, then_branch, else_branch));
results.push_back(test_runner::assert_true(mlc::String("CppTernary"), [&]() { if (std::holds_alternative<cpp_ast::CppTernary>((*ternary)._)) { auto _v_cppternary = std::get<cpp_ast::CppTernary>((*ternary)._); auto [condition_expression, then_expression, else_expression] = _v_cppternary; return [&]() -> bool { 
  bool condition_ok = [&]() { if (std::holds_alternative<cpp_ast::CppIdent>((*condition_expression)._)) { auto _v_cppident = std::get<cpp_ast::CppIdent>((*condition_expression)._); auto [name] = _v_cppident; return name == mlc::String("ready"); } return false; }();
  bool then_ok = [&]() { if (std::holds_alternative<cpp_ast::CppInt>((*then_expression)._)) { auto _v_cppint = std::get<cpp_ast::CppInt>((*then_expression)._); auto [value] = _v_cppint; return value == 1; } return false; }();
  bool else_ok = [&]() { if (std::holds_alternative<cpp_ast::CppInt>((*else_expression)._)) { auto _v_cppint = std::get<cpp_ast::CppInt>((*else_expression)._); auto [value] = _v_cppint; return value == 0; } return false; }();
  return condition_ok && then_ok && else_ok;
 }(); } return false; }()));
std::shared_ptr<cpp_ast::CppExpression> element = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInt(42));
std::shared_ptr<cpp_ast::CppExpression> init_list = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{element}));
results.push_back(test_runner::assert_true(mlc::String("CppInitList"), [&]() { if (std::holds_alternative<cpp_ast::CppInitList>((*init_list)._)) { auto _v_cppinitlist = std::get<cpp_ast::CppInitList>((*init_list)._); auto [elements] = _v_cppinitlist; return elements.size() == 1; } return false; }()));
std::shared_ptr<cpp_ast::CppType> cast_target = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("int")));
std::shared_ptr<cpp_ast::CppExpression> cast_operand = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent(mlc::String("value")));
std::shared_ptr<cpp_ast::CppExpression> cast_expression = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCast((cpp_ast::CppCastStatic{}), cast_target, cast_operand));
results.push_back(test_runner::assert_true(mlc::String("CppCast"), [&]() { if (std::holds_alternative<cpp_ast::CppCast>((*cast_expression)._)) { auto _v_cppcast = std::get<cpp_ast::CppCast>((*cast_expression)._); auto [kind, target_type, operand] = _v_cppcast; return [&]() -> bool { 
  bool kind_ok = [&]() { if (std::holds_alternative<cpp_ast::CppCastStatic>(kind)) {  return true; } return false; }();
  bool target_ok = [&]() { if (std::holds_alternative<cpp_ast::CppTypeName>((*target_type))) { auto _v_cpptypename = std::get<cpp_ast::CppTypeName>((*target_type)); auto [name] = _v_cpptypename; return name == mlc::String("int"); } return false; }();
  bool operand_ok = [&]() { if (std::holds_alternative<cpp_ast::CppIdent>((*operand)._)) { auto _v_cppident = std::get<cpp_ast::CppIdent>((*operand)._); auto [name] = _v_cppident; return name == mlc::String("value"); } return false; }();
  return kind_ok && target_ok && operand_ok;
 }(); } return false; }()));
return results;
}

} // namespace test_cpp_ast
