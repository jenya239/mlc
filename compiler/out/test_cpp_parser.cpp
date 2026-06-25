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

bool cpp_parse_is_class_named(std::shared_ptr<cpp_ast::CppDeclaration> declaration, mlc::String expected_name) noexcept;

bool cpp_parse_member_is_public_access(cpp_ast::CppClassMember member) noexcept;

bool cpp_parse_member_is_field_named(cpp_ast::CppClassMember member, mlc::String expected_name) noexcept;

bool cpp_parse_class_has_public_field(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_parse_is_typedef_size_t(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_parse_is_template_identity(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_parse_inner_is_identity_fn_proto(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_parse_is_extern_block(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::Array<test_runner::TestResult> cpp_parser_tests() noexcept;

int cpp_parse_decl_count(mlc::String source_text) noexcept{return cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program.declarations.size();}

std::shared_ptr<cpp_ast::CppDeclaration> cpp_parse_first(mlc::String source_text) noexcept{
cpp_ast::CppProgram program = cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program;
return program.declarations.size() > 0 ? program.declarations[0] : std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(true, mlc::String("")));
}

int cpp_parse_error_count(mlc::String source_text) noexcept{return cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).errors.size();}

bool cpp_parse_is_class_named(std::shared_ptr<cpp_ast::CppDeclaration> declaration, mlc::String expected_name) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppClassDeclaration>((*declaration))) { auto _v_cppclassdeclaration = std::get<cpp_ast::CppClassDeclaration>((*declaration)); auto [definition] = _v_cppclassdeclaration; return definition.name == expected_name; } return false; }();
}

bool cpp_parse_member_is_public_access(cpp_ast::CppClassMember member) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppClassMemberAccess>(member)) { auto _v_cppclassmemberaccess = std::get<cpp_ast::CppClassMemberAccess>(member); auto [CppPublic] = _v_cppclassmemberaccess; return true; } return false; }();}

bool cpp_parse_member_is_field_named(cpp_ast::CppClassMember member, mlc::String expected_name) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppClassMemberField>(member)) { auto _v_cppclassmemberfield = std::get<cpp_ast::CppClassMemberField>(member); auto [_w0, field_name, _w1] = _v_cppclassmemberfield; return field_name == expected_name; } return false; }();}

bool cpp_parse_class_has_public_field(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppClassDeclaration>((*declaration))) { auto _v_cppclassdeclaration = std::get<cpp_ast::CppClassDeclaration>((*declaration)); auto [definition] = _v_cppclassdeclaration; return definition.members.size() >= 2 && cpp_parse_member_is_public_access(definition.members[0]) && cpp_parse_member_is_field_named(definition.members[1], mlc::String("x")); } return false; }();
}

bool cpp_parse_is_typedef_size_t(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppTypedefDeclaration>((*declaration))) { auto _v_cpptypedefdeclaration = std::get<cpp_ast::CppTypedefDeclaration>((*declaration)); auto [name, type_node] = _v_cpptypedefdeclaration; return name == mlc::String("size_t"); } return false; }();
}

bool cpp_parse_is_template_identity(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppTemplateDeclaration>((*declaration))) { auto _v_cpptemplatedeclaration = std::get<cpp_ast::CppTemplateDeclaration>((*declaration)); auto [parameters, inner] = _v_cpptemplatedeclaration; return parameters == mlc::String("typename T") && cpp_parse_inner_is_identity_fn_proto(inner); } return false; }();
}

bool cpp_parse_inner_is_identity_fn_proto(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppFnProto>((*declaration))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*declaration)); auto [_w0, _w1, name, _w2] = _v_cppfnproto; return name == mlc::String("identity"); } return false; }();}

bool cpp_parse_is_extern_block(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
return [&]() { if (std::holds_alternative<cpp_ast::CppExternBlock>((*declaration))) { auto _v_cppexternblock = std::get<cpp_ast::CppExternBlock>((*declaration)); auto [linkage, inner] = _v_cppexternblock; return linkage == mlc::String("\"C\"") && inner.size() == 1; } return false; }();
}

mlc::Array<test_runner::TestResult> cpp_parser_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("#include <string> - 1 decl"), cpp_parse_decl_count(mlc::String("#include <string>")), 1));
results.push_back(test_runner::assert_true(mlc::String("#include <string> - CppInclude angle"), [&]() { if (std::holds_alternative<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include <string>"))))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include <string>")))); auto [is_angle, path] = _v_cppinclude; return is_angle && path == mlc::String("string"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("#include \"foo.h\" - 1 decl"), cpp_parse_decl_count(mlc::String("#include \"foo.h\"")), 1));
results.push_back(test_runner::assert_true(mlc::String("#include \"foo.h\" - CppInclude quote"), [&]() { if (std::holds_alternative<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include \"foo.h\""))))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*cpp_parse_first(mlc::String("#include \"foo.h\"")))); auto [is_angle, path] = _v_cppinclude; return !is_angle && path == mlc::String("foo.h"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("using X = int; - 1 decl"), cpp_parse_decl_count(mlc::String("using X = int;")), 1));
results.push_back(test_runner::assert_true(mlc::String("using X = int; - CppUsing"), [&]() { if (std::holds_alternative<cpp_ast::CppUsing>((*cpp_parse_first(mlc::String("using X = int;"))))) { auto _v_cppusing = std::get<cpp_ast::CppUsing>((*cpp_parse_first(mlc::String("using X = int;")))); auto [alias, type_string] = _v_cppusing; return alias == mlc::String("X") && type_string == mlc::String("int"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("struct S { int x; }; - 1 decl"), cpp_parse_decl_count(mlc::String("struct S { int x; };")), 1));
results.push_back(test_runner::assert_true(mlc::String("struct S { int x; }; - CppClassDeclaration"), cpp_parse_is_class_named(cpp_parse_first(mlc::String("struct S { int x; };")), mlc::String("S"))));
results.push_back(test_runner::assert_true(mlc::String("class Widget; - CppForwardDecl"), [&]() { if (std::holds_alternative<cpp_ast::CppForwardDecl>((*cpp_parse_first(mlc::String("class Widget;"))))) { auto _v_cppforwarddecl = std::get<cpp_ast::CppForwardDecl>((*cpp_parse_first(mlc::String("class Widget;")))); auto [kind, name] = _v_cppforwarddecl; return kind == mlc::String("class") && name == mlc::String("Widget"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("int add(int a, int b); - 1 decl"), cpp_parse_decl_count(mlc::String("int add(int a, int b);")), 1));
results.push_back(test_runner::assert_true(mlc::String("int add(int a, int b); - CppFnProto"), [&]() { if (std::holds_alternative<cpp_ast::CppFnProto>((*cpp_parse_first(mlc::String("int add(int a, int b);"))))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*cpp_parse_first(mlc::String("int add(int a, int b);")))); auto [_w0, return_type, name, parameters] = _v_cppfnproto; return return_type == mlc::String("int") && name == mlc::String("add") && parameters.size() == 2; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("enum Color { Red, Green }; - CppVariant"), [&]() { if (std::holds_alternative<cpp_ast::CppVariant>((*cpp_parse_first(mlc::String("enum Color { Red, Green };"))))) { auto _v_cppvariant = std::get<cpp_ast::CppVariant>((*cpp_parse_first(mlc::String("enum Color { Red, Green };")))); auto [_w0, name, arms] = _v_cppvariant; return name == mlc::String("Color") && arms.size() == 2; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("namespace foo { int x; } - CppNamespace"), [&]() { if (std::holds_alternative<cpp_ast::CppNamespace>((*cpp_parse_first(mlc::String("namespace foo { int x; }"))))) { auto _v_cppnamespace = std::get<cpp_ast::CppNamespace>((*cpp_parse_first(mlc::String("namespace foo { int x; }")))); auto [name, inner] = _v_cppnamespace; return name == mlc::String("foo") && inner.size() == 1; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("typedef unsigned long size_t; - CppTypedefDeclaration"), cpp_parse_is_typedef_size_t(cpp_parse_first(mlc::String("typedef unsigned long size_t;")))));
results.push_back(test_runner::assert_true(mlc::String("template<typename T> T identity(T value); - CppTemplateDeclaration"), cpp_parse_is_template_identity(cpp_parse_first(mlc::String("template<typename T> T identity(T value);")))));
results.push_back(test_runner::assert_true(mlc::String("class Foo { public: int x; }; - access and field"), cpp_parse_class_has_public_field(cpp_parse_first(mlc::String("class Foo { public: int x; };")))));
results.push_back(test_runner::assert_true(mlc::String("[[nodiscard]] int fn(); - attribute skipped"), cpp_parse_decl_count(mlc::String("[[nodiscard]] int fn();")) == 1));
results.push_back(test_runner::assert_true(mlc::String("extern \"C\" { int x; } - CppExternBlock"), cpp_parse_is_extern_block(cpp_parse_first(mlc::String("extern \"C\" { int x; }")))));
results.push_back(test_runner::assert_true(mlc::String("error recovery - garbage then ok"), cpp_parse_error_count(mlc::String("??? garbage; int ok();")) > 0 && cpp_parse_decl_count(mlc::String("??? garbage; int ok();")) == 1));
return results;
}

} // namespace test_cpp_parser
