#include "test_cpp_printer.hpp"

#include "test_runner.hpp"
#include "cpp_printer.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_printer {

using namespace test_runner;
using namespace cpp_printer;
using namespace cpp_ast;

std::shared_ptr<cpp_ast::CppType> integer_type() noexcept;

mlc::Array<test_runner::TestResult> cpp_printer_tests() noexcept;

std::shared_ptr<cpp_ast::CppType> integer_type() noexcept{return std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("int")));}

mlc::Array<test_runner::TestResult> cpp_printer_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
std::shared_ptr<cpp_ast::CppType> int_type = integer_type();
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppInt"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(42))), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppStr"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppStr(mlc::String("hello")))), mlc::String("\"hello\"")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppBool"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppBool(false))), mlc::String("false")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppCall"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("foo"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpr>>{std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(1)), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(2))}))), mlc::String("foo(1, 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppMember value"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppMember(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("object"))), mlc::String("field"), false))), mlc::String("object.field")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppMember pointer"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppMember(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("pointer"))), mlc::String("field"), true))), mlc::String("pointer->field")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppIndex"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIndex(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("items"))), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(0))))), mlc::String("items[0]")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppBinary"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppBinary(mlc::String("+"), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(1)), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(2))))), mlc::String("(1 + 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppUnary"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppUnary(mlc::String("-"), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(5))))), mlc::String("(-5)")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppTernary"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppTernary(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("ready"))), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(1)), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(0))))), mlc::String("(ready ? (1) : (0))")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppInitList empty"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInitList({}))), mlc::String("{}")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppInitList elements"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInitList(mlc::Array<std::shared_ptr<cpp_ast::CppExpr>>{std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(1)), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(2))}))), mlc::String("{1, 2}")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppCast"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppCast((cpp_ast::CppCastStatic{}), int_type, std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("value")))))), mlc::String("static_cast<int>(value)")));
std::shared_ptr<cpp_ast::CppCapture> capture = std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String("counter"), true});
std::shared_ptr<cpp_ast::CppParam> parameter = std::make_shared<cpp_ast::CppParam>(cpp_ast::CppParam{mlc::String("input"), int_type});
mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> lambda_body = mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppReturn(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("input")))))};
results.push_back(test_runner::assert_eq_str(mlc::String("print_expr CppLambda"), cpp_printer::print_expr(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppLambda(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{capture}, mlc::Array<std::shared_ptr<cpp_ast::CppParam>>{parameter}, int_type, lambda_body))), mlc::String("[&counter](int input) -> int { return input; }")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_statement CppReturn"), cpp_printer::print_statement(std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppReturn(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(0))))), mlc::String("return 0;")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_statement CppAutoDecl"), cpp_printer::print_statement(std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppAutoDecl(mlc::String("counter"), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(0))))), mlc::String("auto counter = 0;")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_statement CppConstDecl"), cpp_printer::print_statement(std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppConstDecl(mlc::String("limit"), int_type, std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(10))))), mlc::String("const int limit = 10;")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_statement CppIf"), cpp_printer::print_statement(std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppIf(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("flag"))), std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppBlock(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppReturn(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(1))))})), std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppBlock(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppReturn(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(0))))}))))), mlc::String("if (flag) {return 1;} else {return 0;}")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_statement CppFor"), cpp_printer::print_statement(std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppFor(mlc::String("item"), std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("items"))), mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppExprStmt(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("process"))), mlc::Array<std::shared_ptr<cpp_ast::CppExpr>>{std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(mlc::String("item")))}))))}))), mlc::String("for (auto item : items) {process(item);}")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_decl CppInclude angle"), cpp_printer::print_decl(std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppInclude(true, mlc::String("vector")))), mlc::String("#include <vector>")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_decl CppInclude quoted"), cpp_printer::print_decl(std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppInclude(false, mlc::String("local.h")))), mlc::String("#include \"local.h\"")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_decl CppStruct"), cpp_printer::print_decl(std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppStruct(mlc::String("Point"), mlc::Array<std::shared_ptr<cpp_ast::CppField>>{std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{mlc::String("int"), mlc::String("x")})}))), mlc::String("struct Point {\nint x;\n};")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_decl CppFnProto"), cpp_printer::print_decl(std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppFnProto(mlc::String("int"), mlc::String("add"), mlc::Array<mlc::String>{mlc::String("int left"), mlc::String("int right")}))), mlc::String("int add(int left, int right);")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_decl CppFnDef"), cpp_printer::print_decl(std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppFnDef(mlc::String("int"), mlc::String("answer"), {}, mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppReturn(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(42))))}))), mlc::String("int answer() {return 42;}")));
results.push_back(test_runner::assert_eq_str(mlc::String("print_decl CppNamespace"), cpp_printer::print_decl(std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppNamespace(mlc::String("detail"), mlc::Array<std::shared_ptr<cpp_ast::CppDecl>>{std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppUsing(mlc::String("Byte"), mlc::String("unsigned char")))}))), mlc::String("namespace detail {\nusing Byte = unsigned char;\n}")));
std::shared_ptr<cpp_ast::CppVariantArm> variant_arm = std::make_shared<cpp_ast::CppVariantArm>(cpp_ast::CppVariantArm{mlc::String("Integer"), mlc::Array<std::shared_ptr<cpp_ast::CppType>>{int_type}});
results.push_back(test_runner::assert_eq_str(mlc::String("print_decl CppVariant"), cpp_printer::print_decl(std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppVariant(mlc::String("Value"), mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>>{variant_arm}))), mlc::String("using Value = std::variant<int>;")));
std::shared_ptr<cpp_ast::CppDecl> header_include = std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppInclude(true, mlc::String("vector")));
std::shared_ptr<cpp_ast::CppDecl> source_function = std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppFnDef(mlc::String("int"), mlc::String("main"), {}, mlc::Array<std::shared_ptr<cpp_ast::CppStmt>>{std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppReturn(std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(0))))}));
std::shared_ptr<cpp_ast::CppFile> file = std::make_shared<cpp_ast::CppFile>(cpp_ast::CppFile{mlc::Array<std::shared_ptr<cpp_ast::CppDecl>>{header_include}, mlc::Array<std::shared_ptr<cpp_ast::CppDecl>>{source_function}});
results.push_back(test_runner::assert_eq_str(mlc::String("print_file header and source"), cpp_printer::print_file(file), mlc::String("#include <vector>\n\nint main() {return 0;}")));
return results;
}

} // namespace test_cpp_printer
