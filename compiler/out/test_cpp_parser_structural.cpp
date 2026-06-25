#include "test_cpp_parser_structural.hpp"

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_parser_structural {

using namespace test_runner;
using namespace cpp_lexer;
using namespace cpp_parser;
using namespace cpp_ast;

int cpp_structural_parse_count(mlc::String source_text) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> cpp_structural_parse_first(mlc::String source_text) noexcept;

bool cpp_structural_member_is_public(cpp_ast::CppClassMember member) noexcept;

bool cpp_structural_member_is_protected(cpp_ast::CppClassMember member) noexcept;

bool cpp_structural_member_is_private(cpp_ast::CppClassMember member) noexcept;

mlc::Array<cpp_ast::CppClassMember> cpp_structural_class_members(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_structural_has_public_access(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_structural_has_protected_access(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_structural_has_private_access(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_structural_is_member_field(cpp_ast::CppClassMember member) noexcept;

int cpp_structural_field_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_structural_is_struct(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_structural_template_inner_is_identity(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_structural_is_identity_fn_proto(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

int cpp_structural_enum_arm_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

int cpp_structural_namespace_inner_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

int cpp_structural_extern_inner_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::Array<test_runner::TestResult> cpp_parser_structural_tests() noexcept;

int cpp_structural_parse_count(mlc::String source_text) noexcept{return cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program.declarations.size();}

std::shared_ptr<cpp_ast::CppDeclaration> cpp_structural_parse_first(mlc::String source_text) noexcept{
cpp_ast::CppProgram program = cpp_parser::cpp_parse(cpp_lexer::cpp_tokenize(source_text).tokens).program;
return program.declarations.size() > 0 ? program.declarations[0] : std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppForwardDecl(mlc::String("struct"), mlc::String("")));
}

bool cpp_structural_member_is_public(cpp_ast::CppClassMember member) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppClassMemberAccess>(member)) { auto _v_cppclassmemberaccess = std::get<cpp_ast::CppClassMemberAccess>(member); auto [CppPublic] = _v_cppclassmemberaccess; return true; } return false; }();}

bool cpp_structural_member_is_protected(cpp_ast::CppClassMember member) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppClassMemberAccess>(member)) { auto _v_cppclassmemberaccess = std::get<cpp_ast::CppClassMemberAccess>(member); auto [CppProtected] = _v_cppclassmemberaccess; return true; } return false; }();}

bool cpp_structural_member_is_private(cpp_ast::CppClassMember member) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppClassMemberAccess>(member)) { auto _v_cppclassmemberaccess = std::get<cpp_ast::CppClassMemberAccess>(member); auto [CppPrivate] = _v_cppclassmemberaccess; return true; } return false; }();}

mlc::Array<cpp_ast::CppClassMember> cpp_structural_class_members(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<cpp_ast::CppClassMember> empty_members = {};
return [&]() -> mlc::Array<cpp_ast::CppClassMember> { if (std::holds_alternative<cpp_ast::CppClassDeclaration>((*declaration))) { auto _v_cppclassdeclaration = std::get<cpp_ast::CppClassDeclaration>((*declaration)); auto [definition] = _v_cppclassdeclaration; return definition.members; } return empty_members; }();
}

bool cpp_structural_has_public_access(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<cpp_ast::CppClassMember> members = cpp_structural_class_members(declaration);
int index = 0;
bool found = false;
while (index < members.size() && !found){
{
if (cpp_structural_member_is_public(members[index])){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}

bool cpp_structural_has_protected_access(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<cpp_ast::CppClassMember> members = cpp_structural_class_members(declaration);
int index = 0;
bool found = false;
while (index < members.size() && !found){
{
if (cpp_structural_member_is_protected(members[index])){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}

bool cpp_structural_has_private_access(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<cpp_ast::CppClassMember> members = cpp_structural_class_members(declaration);
int index = 0;
bool found = false;
while (index < members.size() && !found){
{
if (cpp_structural_member_is_private(members[index])){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}

bool cpp_structural_is_member_field(cpp_ast::CppClassMember member) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppClassMemberField>(member)) { auto _v_cppclassmemberfield = std::get<cpp_ast::CppClassMemberField>(member); auto [_w0, _w1, _w2] = _v_cppclassmemberfield; return true; } return false; }();}

int cpp_structural_field_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<cpp_ast::CppClassMember> members = cpp_structural_class_members(declaration);
int count = 0;
int index = 0;
while (index < members.size()){
{
if (cpp_structural_is_member_field(members[index])){
{
count = count + 1;
}
}
index = index + 1;
}
}
return count;
}

bool cpp_structural_is_struct(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppClassDeclaration>((*declaration))) { auto _v_cppclassdeclaration = std::get<cpp_ast::CppClassDeclaration>((*declaration)); auto [definition] = _v_cppclassdeclaration; return definition.is_struct; } return false; }();}

bool cpp_structural_template_inner_is_identity(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppTemplateDeclaration>((*declaration))) { auto _v_cpptemplatedeclaration = std::get<cpp_ast::CppTemplateDeclaration>((*declaration)); auto [_w0, inner] = _v_cpptemplatedeclaration; return cpp_structural_is_identity_fn_proto(inner); } return false; }();}

bool cpp_structural_is_identity_fn_proto(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppFnProto>((*declaration))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*declaration)); auto [_w0, _w1, name, _w2] = _v_cppfnproto; return name == mlc::String("identity"); } return false; }();}

int cpp_structural_enum_arm_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> empty_arms = {};
mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms = [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> { if (std::holds_alternative<cpp_ast::CppVariant>((*declaration))) { auto _v_cppvariant = std::get<cpp_ast::CppVariant>((*declaration)); auto [_w0, _w1, arms] = _v_cppvariant; return arms; } return empty_arms; }();
return arms.size();
}

int cpp_structural_namespace_inner_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty_declarations = {};
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> inner = [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<cpp_ast::CppNamespace>((*declaration))) { auto _v_cppnamespace = std::get<cpp_ast::CppNamespace>((*declaration)); auto [_w0, inner] = _v_cppnamespace; return inner; } return empty_declarations; }();
return inner.size();
}

int cpp_structural_extern_inner_count(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty_declarations = {};
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> inner = [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<cpp_ast::CppExternBlock>((*declaration))) { auto _v_cppexternblock = std::get<cpp_ast::CppExternBlock>((*declaration)); auto [_w0, inner] = _v_cppexternblock; return inner; } return empty_declarations; }();
return inner.size();
}

mlc::Array<test_runner::TestResult> cpp_parser_structural_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("struct two fields"), cpp_structural_field_count(cpp_structural_parse_first(mlc::String("struct Pair { int left; int right; };"))), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("class one field"), cpp_structural_field_count(cpp_structural_parse_first(mlc::String("class Cell { int value; };"))), 1));
results.push_back(test_runner::assert_true(mlc::String("struct is_struct flag"), cpp_structural_is_struct(cpp_structural_parse_first(mlc::String("struct Box { int value; };")))));
results.push_back(test_runner::assert_true(mlc::String("class is_struct false"), !cpp_structural_is_struct(cpp_structural_parse_first(mlc::String("class Box { int value; };")))));
results.push_back(test_runner::assert_true(mlc::String("class public access"), cpp_structural_has_public_access(cpp_structural_parse_first(mlc::String("class A { public: int x; };")))));
results.push_back(test_runner::assert_true(mlc::String("class protected access"), cpp_structural_has_protected_access(cpp_structural_parse_first(mlc::String("class A { protected: int x; };")))));
results.push_back(test_runner::assert_true(mlc::String("class private access"), cpp_structural_has_private_access(cpp_structural_parse_first(mlc::String("class A { private: int x; };")))));
results.push_back(test_runner::assert_eq_int(mlc::String("enum three arms"), cpp_structural_enum_arm_count(cpp_structural_parse_first(mlc::String("enum RGB { Red, Green, Blue };"))), 3));
results.push_back(test_runner::assert_eq_int(mlc::String("namespace inner decl count"), cpp_structural_namespace_inner_count(cpp_structural_parse_first(mlc::String("namespace box { int item; int other; }"))), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("triple decl file"), cpp_structural_parse_count(mlc::String("int a(); struct B { int y; }; enum E { X };")), 3));
results.push_back(test_runner::assert_true(mlc::String("typedef alias name"), [&]() { if (std::holds_alternative<cpp_ast::CppTypedefDeclaration>((*cpp_structural_parse_first(mlc::String("typedef unsigned long size_t;"))))) { auto _v_cpptypedefdeclaration = std::get<cpp_ast::CppTypedefDeclaration>((*cpp_structural_parse_first(mlc::String("typedef unsigned long size_t;")))); auto [name, _w0] = _v_cpptypedefdeclaration; return name == mlc::String("size_t"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("template parameters string"), [&]() { if (std::holds_alternative<cpp_ast::CppTemplateDeclaration>((*cpp_structural_parse_first(mlc::String("template<typename T> T identity(T value);"))))) { auto _v_cpptemplatedeclaration = std::get<cpp_ast::CppTemplateDeclaration>((*cpp_structural_parse_first(mlc::String("template<typename T> T identity(T value);")))); auto [parameters, _w0] = _v_cpptemplatedeclaration; return parameters == mlc::String("typename T"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("template inner fn proto"), cpp_structural_template_inner_is_identity(cpp_structural_parse_first(mlc::String("template<typename T> T identity(T value);")))));
results.push_back(test_runner::assert_eq_int(mlc::String("extern block inner count"), cpp_structural_extern_inner_count(cpp_structural_parse_first(mlc::String("extern \"C\" { int x; void f(); }"))), 2));
results.push_back(test_runner::assert_true(mlc::String("forward enum"), [&]() { if (std::holds_alternative<cpp_ast::CppForwardDecl>((*cpp_structural_parse_first(mlc::String("enum State;"))))) { auto _v_cppforwarddecl = std::get<cpp_ast::CppForwardDecl>((*cpp_structural_parse_first(mlc::String("enum State;")))); auto [kind, name] = _v_cppforwarddecl; return kind == mlc::String("enum") && name == mlc::String("State"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("forward struct"), [&]() { if (std::holds_alternative<cpp_ast::CppForwardDecl>((*cpp_structural_parse_first(mlc::String("struct Pending;"))))) { auto _v_cppforwarddecl = std::get<cpp_ast::CppForwardDecl>((*cpp_structural_parse_first(mlc::String("struct Pending;")))); auto [kind, name] = _v_cppforwarddecl; return kind == mlc::String("struct") && name == mlc::String("Pending"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("fn proto zero parameters"), [&]() { if (std::holds_alternative<cpp_ast::CppFnProto>((*cpp_structural_parse_first(mlc::String("void reset();"))))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*cpp_structural_parse_first(mlc::String("void reset();")))); auto [_w0, return_type, name, parameters] = _v_cppfnproto; return return_type == mlc::String("void") && name == mlc::String("reset") && parameters.size() == 0; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("fn proto three parameters"), [&]() { if (std::holds_alternative<cpp_ast::CppFnProto>((*cpp_structural_parse_first(mlc::String("int mix(int a, int b, int c);"))))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*cpp_structural_parse_first(mlc::String("int mix(int a, int b, int c);")))); auto [_w0, _w1, name, parameters] = _v_cppfnproto; return name == mlc::String("mix") && parameters.size() == 3; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("using alias type"), [&]() { if (std::holds_alternative<cpp_ast::CppUsing>((*cpp_structural_parse_first(mlc::String("using Counter = int;"))))) { auto _v_cppusing = std::get<cpp_ast::CppUsing>((*cpp_structural_parse_first(mlc::String("using Counter = int;")))); auto [alias, type_string] = _v_cppusing; return alias == mlc::String("Counter") && type_string == mlc::String("int"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("two template decls"), cpp_structural_parse_count(mlc::String("template<typename T> T one(T value); template<typename U> U two(U value);")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("garbage recovery keeps good decl"), cpp_structural_parse_count(mlc::String("??? bad; int good();")), 1));
return results;
}

} // namespace test_cpp_parser_structural
