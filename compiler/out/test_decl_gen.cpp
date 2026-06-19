#include "test_decl_gen.hpp"

#include "test_runner.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "transform_decl.hpp"
#include "context.hpp"
#include "module.hpp"
#include "decl_extend.hpp"
#include "decl.hpp"
#include "derive_methods_cpp.hpp"
#include "trait_struct_cpp.hpp"
#include "cpp_ast.hpp"
#include "decl_cpp.hpp"
#include "type_gen.hpp"
#include "expr.hpp"
#include "ast_builders.hpp"
#include "codegen_test_helpers.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "return_body.hpp"
#include "stmt_cpp.hpp"

namespace test_decl_gen {

using namespace test_runner;
using namespace ast;
using namespace lexer;
using namespace decls;
using namespace check;
using namespace transform_decl;
using namespace context;
using namespace module;
using namespace decl_extend;
using namespace decl;
using namespace derive_methods_cpp;
using namespace trait_struct_cpp;
using namespace cpp_ast;
using namespace decl_cpp;
using namespace type_gen;
using namespace expr;
using namespace ast_builders;
using namespace codegen_test_helpers;
using namespace semantic_ir;
using namespace registry;
using namespace return_body;
using namespace stmt_cpp;
using namespace ast_tokens;

std::shared_ptr<registry::Type> ti32() noexcept;

std::shared_ptr<registry::Type> tstr() noexcept;

std::shared_ptr<registry::Type> tunit() noexcept;

std::shared_ptr<registry::Type> tbool() noexcept;

std::shared_ptr<registry::Type> unk() noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> make_ident(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> make_bin(mlc::String op, std::shared_ptr<semantic_ir::SemanticExpression> left, std::shared_ptr<semantic_ir::SemanticExpression> right) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> make_unit_expr() noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> make_int(int value) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> make_bool(bool value) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> make_question(std::shared_ptr<semantic_ir::SemanticExpression> inner) noexcept;

std::shared_ptr<ast::Param> iparam(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SemanticDeclaration> fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;

std::shared_ptr<ast::Param> generic_param(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SemanticDeclaration> type_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;

std::shared_ptr<semantic_ir::SemanticDeclaration> trait_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

std::shared_ptr<semantic_ir::SemanticDeclaration> extend_decl(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

ast::Span first_transformed_decl_span(ast::Program program, registry::TypeRegistry registry) noexcept;

ast::Span transformed_decl_span(mlc::String source) noexcept;

ast::Span transformed_fn_decl_span(mlc::String source) noexcept;

ast::Span transformed_type_decl_span(mlc::String source) noexcept;

ast::Span transformed_trait_decl_span(mlc::String source) noexcept;

ast::Span transformed_extend_decl_span(mlc::String source) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_decl_sequence_children(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool is_cpp_decl_empty(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_decl_is_using(std::shared_ptr<cpp_ast::CppDeclaration> declaration, mlc::String alias, mlc::String type_cpp) noexcept;

bool cpp_decl_list_contains_using(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String alias, mlc::String type_cpp) noexcept;

bool cpp_decl_is_std_hash_specialization(std::shared_ptr<cpp_ast::CppDeclaration> declaration, mlc::String type_name) noexcept;

bool cpp_decl_is_fragment(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_stmt_is_fragment(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

bool cpp_stmt_tree_contains_fragment(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

bool cpp_stmt_list_contains_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_std_hash_body_statements(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_decl_list_hash_body_contains_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

bool cpp_decl_list_contains_std_hash_specialization(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String type_name) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> make_str_expr(mlc::String text) noexcept;

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept;

std::shared_ptr<registry::Type> ti32() noexcept{return std::make_shared<registry::Type>((registry::TI32{}));}

std::shared_ptr<registry::Type> tstr() noexcept{return std::make_shared<registry::Type>((registry::TString{}));}

std::shared_ptr<registry::Type> tunit() noexcept{return std::make_shared<registry::Type>((registry::TUnit{}));}

std::shared_ptr<registry::Type> tbool() noexcept{return std::make_shared<registry::Type>((registry::TBool{}));}

std::shared_ptr<registry::Type> unk() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

std::shared_ptr<semantic_ir::SemanticExpression> make_ident(mlc::String name) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(name, unk(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> make_bin(mlc::String op, std::shared_ptr<semantic_ir::SemanticExpression> left, std::shared_ptr<semantic_ir::SemanticExpression> right) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(op, left, right, unk(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> make_unit_expr() noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit(tunit(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> make_int(int value) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(value, ti32(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> make_bool(bool value) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBool(value, tbool(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> make_question(std::shared_ptr<semantic_ir::SemanticExpression> inner) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionQuestion(inner, unk(), ast::span_unknown()));}

std::shared_ptr<ast::Param> iparam(mlc::String name) noexcept{return std::make_shared<ast::Param>(ast::Param{name, false, std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});}

std::shared_ptr<semantic_ir::SemanticDeclaration> fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept{
mlc::Array<ast::WhereClauseBound> where_clause_bounds = {};
return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationFn(name, {}, {}, params, return_type, body, where_clause_bounds, ast::span_unknown()));
}

std::shared_ptr<ast::Param> generic_param(mlc::String name) noexcept{return std::make_shared<ast::Param>(ast::Param{name, false, std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("T"))), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});}

std::shared_ptr<semantic_ir::SemanticDeclaration> type_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationType(name, type_params, variants, derive_traits, ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticDeclaration> trait_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationTrait(name, type_params, methods, ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticDeclaration> extend_decl(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExtend(type_name, trait_name, methods, ast::span_unknown()));}

ast::Span first_transformed_decl_span(ast::Program program, registry::TypeRegistry registry) noexcept{
semantic_ir::SemanticProgram transformed = transform_decl::transform_program(program, registry);
return transformed.decls.size() == 0 ? ast::span_unknown() : semantic_ir::sdecl_span(transformed.decls[0]);
}

ast::Span transformed_decl_span(mlc::String source) noexcept{
ast::Program program = decls::parse_program(lexer::tokenize(source).tokens);
return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> ast::Span { auto [checked] = ok; return first_transformed_decl_span(program, checked.registry); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Span { auto [_w0] = err; return ast::span_unknown(); }
}, check::check(program));
}

ast::Span transformed_fn_decl_span(mlc::String source) noexcept{return transformed_decl_span(source);}

ast::Span transformed_type_decl_span(mlc::String source) noexcept{return transformed_decl_span(source);}

ast::Span transformed_trait_decl_span(mlc::String source) noexcept{return transformed_decl_span(source);}

ast::Span transformed_extend_decl_span(mlc::String source) noexcept{return transformed_decl_span(source);}

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_decl_sequence_children(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> { if (std::holds_alternative<cpp_ast::CppDeclarationSequence>((*declaration))) { auto _v_cppdeclarationsequence = std::get<cpp_ast::CppDeclarationSequence>((*declaration)); auto [declarations] = _v_cppdeclarationsequence; return declarations; } return {}; }();}

bool is_cpp_decl_empty(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppDeclarationEmpty>((*declaration))) {  return true; } return false; }();}

bool cpp_decl_is_using(std::shared_ptr<cpp_ast::CppDeclaration> declaration, mlc::String alias, mlc::String type_cpp) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppUsing>((*declaration))) { auto _v_cppusing = std::get<cpp_ast::CppUsing>((*declaration)); auto [decl_alias, decl_type] = _v_cppusing; return decl_alias == alias && decl_type == type_cpp; } return false; }();}

bool cpp_decl_list_contains_using(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String alias, mlc::String type_cpp) noexcept{
int index = 0;
bool found = false;
while (index < declarations.size() && !found){
{
found = cpp_decl_is_using(declarations[index], alias, type_cpp);
index = index + 1;
}
}
return found;
}

bool cpp_decl_is_std_hash_specialization(std::shared_ptr<cpp_ast::CppDeclaration> declaration, mlc::String type_name) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppStdHashSpecialization>((*declaration))) { auto _v_cppstdhashspecialization = std::get<cpp_ast::CppStdHashSpecialization>((*declaration)); auto [specialized_type_name, _w0] = _v_cppstdhashspecialization; return specialized_type_name == type_name; } return false; }();}

bool cpp_decl_is_fragment(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppDeclarationFragment>((*declaration))) { auto _v_cppdeclarationfragment = std::get<cpp_ast::CppDeclarationFragment>((*declaration)); auto [_w0] = _v_cppdeclarationfragment; return true; } return false; }();}

bool cpp_stmt_is_fragment(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppStatementFragment>((*statement)._)) { auto _v_cppstatementfragment = std::get<cpp_ast::CppStatementFragment>((*statement)._); auto [_w0] = _v_cppstatementfragment; return true; } return false; }();}

bool cpp_stmt_tree_contains_fragment(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return cpp_stmt_is_fragment(statement) ? true : [&]() { if (std::holds_alternative<cpp_ast::CppBlock>((*statement)._)) { auto _v_cppblock = std::get<cpp_ast::CppBlock>((*statement)._); auto [statements] = _v_cppblock; return cpp_stmt_list_contains_fragment(statements); } if (std::holds_alternative<cpp_ast::CppIf>((*statement)._)) { auto _v_cppif = std::get<cpp_ast::CppIf>((*statement)._); auto [_w0, then_branch, else_branch] = _v_cppif; return cpp_stmt_tree_contains_fragment(then_branch) || cpp_stmt_tree_contains_fragment(else_branch); } if (std::holds_alternative<cpp_ast::CppWhile>((*statement)._)) { auto _v_cppwhile = std::get<cpp_ast::CppWhile>((*statement)._); auto [_w0, body] = _v_cppwhile; return cpp_stmt_tree_contains_fragment(body); } if (std::holds_alternative<cpp_ast::CppFor>((*statement)._)) { auto _v_cppfor = std::get<cpp_ast::CppFor>((*statement)._); auto [_w0, _w1, body] = _v_cppfor; return cpp_stmt_list_contains_fragment(body); } return false; }();}

bool cpp_stmt_list_contains_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
int index = 0;
bool found = false;
while (index < statements.size() && !found){
{
found = cpp_stmt_tree_contains_fragment(statements[index]);
index = index + 1;
}
}
return found;
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_std_hash_body_statements(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { if (std::holds_alternative<cpp_ast::CppStdHashSpecialization>((*declaration))) { auto _v_cppstdhashspecialization = std::get<cpp_ast::CppStdHashSpecialization>((*declaration)); auto [_w0, body] = _v_cppstdhashspecialization; return body; } return {}; }();}

bool cpp_decl_list_hash_body_contains_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
int index = 0;
bool found = false;
while (index < declarations.size() && !found){
{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body = cpp_std_hash_body_statements(declarations[index]);
if (body.size() > 0){
{
found = cpp_stmt_list_contains_fragment(body);
}
}
index = index + 1;
}
}
return found;
}

bool cpp_decl_list_contains_std_hash_specialization(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations, mlc::String type_name) noexcept{
int index = 0;
bool found = false;
while (index < declarations.size() && !found){
{
found = cpp_decl_is_std_hash_specialization(declarations[index], type_name);
index = index + 1;
}
}
return found;
}

std::shared_ptr<semantic_ir::SemanticExpression> make_str_expr(mlc::String text) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionStr(text, tstr(), ast::span_unknown()));}

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
context::CodegenContext context = codegen_test_helpers::empty_codegen_context();
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: zero params"), decl::gen_fn_proto(mlc::String("compute"), {}, {}, {}, tunit(), context), mlc::String("void compute() noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: single i32 param"), decl::gen_fn_proto(mlc::String("double_it"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), context), mlc::String("int double_it(int value) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: two params"), decl::gen_fn_proto(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), context), mlc::String("int add(int left, int right) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: mut array param by reference"), decl::gen_fn_proto(mlc::String("push_token"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("tokens"), true, std::make_shared<ast::TypeExpr>(ast::TyArray(std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("Token"))))), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))})}, tunit(), context), mlc::String("void push_token(mlc::Array<Token>& tokens) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: string return type"), decl::gen_fn_proto(mlc::String("greet"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("name"), false, std::make_shared<ast::TypeExpr>((ast::TyString{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))})}, tstr(), context), mlc::String("mlc::String greet(mlc::String name) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: default param literal"), decl::gen_fn_proto(mlc::String("f"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("a"), false, std::make_shared<ast::TypeExpr>((ast::TyI32{})), true, std::make_shared<ast::Expr>(ast::ExprInt(2, ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))})}, ti32(), context), mlc::String("int f(int a = 2) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: body omits default value"), decl::gen_fn_decl(mlc::String("f"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("a"), false, std::make_shared<ast::TypeExpr>((ast::TyI32{})), true, std::make_shared<ast::Expr>(ast::ExprInt(2, ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))})}, ti32(), make_ident(mlc::String("a")), context), mlc::String("int f(int a) noexcept{\nreturn a;\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: identity function"), decl::gen_fn_decl(mlc::String("identity"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), make_ident(mlc::String("value")), context), mlc::String("int identity(int value) noexcept{\nreturn value;\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: addition"), decl::gen_fn_decl(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), make_bin(mlc::String("+"), make_ident(mlc::String("left")), make_ident(mlc::String("right"))), context), mlc::String("int add(int left, int right) noexcept{\nreturn (left + right);\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: unit return"), decl::gen_fn_decl(mlc::String("nothing"), {}, {}, {}, tunit(), make_unit_expr(), context), mlc::String("void nothing() noexcept{\n}\n")));
std::shared_ptr<semantic_ir::SemanticDeclaration> add_decl = fn_decl(mlc::String("add"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), make_bin(mlc::String("+"), make_ident(mlc::String("left")), make_ident(mlc::String("right"))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains return type"), decl::gen_decl(add_decl, context), mlc::String("int add")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains body"), decl::gen_decl(add_decl, context), mlc::String("return (left + right)")));
std::shared_ptr<semantic_ir::SemanticDeclaration> point_type = type_decl(mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})}, {});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: record struct"), decl::gen_decl(point_type, context), mlc::String("Point")));
std::shared_ptr<semantic_ir::SemanticDeclaration> color_type = type_decl(mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))}, {});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: unit variants"), decl::gen_decl(color_type, context), mlc::String("Red")));
std::shared_ptr<semantic_ir::SemanticDeclaration> point_display = type_decl(mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})}, mlc::Array<mlc::String>{mlc::String("Display")});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display record: generates to_string function"), decl::gen_decl(point_display, context), mlc::String("Point_to_string")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display record: uses mlc::to_string for i32 field"), decl::gen_decl(point_display, context), mlc::String("mlc::to_string(self.x)")));
std::shared_ptr<semantic_ir::SemanticDeclaration> point_eq = type_decl(mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})}, mlc::Array<mlc::String>{mlc::String("Eq")});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Eq record: generates operator=="), decl::gen_decl(point_eq, context), mlc::String("operator==")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Eq record: compares fields"), decl::gen_decl(point_eq, context), mlc::String("a.x == b.x")));
std::shared_ptr<semantic_ir::SemanticDeclaration> color_display = type_decl(mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))}, mlc::Array<mlc::String>{mlc::String("Display")});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display sum type: generates to_string"), decl::gen_decl(color_display, context), mlc::String("Color_to_string")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display sum type: handles Red variant"), decl::gen_decl(color_display, context), mlc::String("\"Red\"")));
std::shared_ptr<semantic_ir::SemanticDeclaration> point_hash = type_decl(mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("tag"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("flag"), std::make_shared<ast::TypeExpr>((ast::TyBool{}))), ast_builders::field_definition(mlc::String("label"), std::make_shared<ast::TypeExpr>((ast::TyString{})))})}, mlc::Array<mlc::String>{mlc::String("Hash")});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash record: namespace std and struct hash"), decl::gen_decl(point_hash, context), mlc::String("namespace std")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash record: struct hash<Point>"), decl::gen_decl(point_hash, context), mlc::String("struct hash<Point>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash record: combines i32 field"), decl::gen_decl(point_hash, context), mlc::String("std::hash<int>{}(self.tag)")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash record: combines bool field"), decl::gen_decl(point_hash, context), mlc::String("std::hash<bool>{}(self.flag)")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash record: combines string field"), decl::gen_decl(point_hash, context), mlc::String("std::hash<mlc::String>{}(self.label)")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash record: boost-style combine constant"), decl::gen_decl(point_hash, context), mlc::String("0x9e3779b97f4a7c15ULL")));
std::shared_ptr<semantic_ir::SemanticDeclaration> empty_hash = type_decl(mlc::String("Empty"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Empty"), {})}, mlc::Array<mlc::String>{mlc::String("Hash")});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash empty record: fnv offset basis seed"), decl::gen_decl(empty_hash, context), mlc::String("1469598103934665603ULL")));
std::shared_ptr<semantic_ir::SemanticDeclaration> hue_hash = type_decl(mlc::String("Hue"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("North")), ast_builders::unit_variant(mlc::String("South"))}, mlc::Array<mlc::String>{mlc::String("Hash")});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash sum: holds_alternative first variant"), decl::gen_decl(hue_hash, context), mlc::String("std::holds_alternative<North>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash sum: holds_alternative second variant"), decl::gen_decl(hue_hash, context), mlc::String("std::holds_alternative<South>")));
std::shared_ptr<semantic_ir::SemanticDeclaration> shape_hash = type_decl(mlc::String("Shape"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::tuple_variant(mlc::String("Circle"), mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>((ast::TyI32{}))}), ast_builders::unit_variant(mlc::String("Empty"))}, mlc::Array<mlc::String>{mlc::String("Hash")});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Hash sum tuple variant: std get and field0"), decl::gen_decl(shape_hash, context), mlc::String("std::get<Circle>(self._).field0")));
std::shared_ptr<ast::Param> display_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("Display"))), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});
std::shared_ptr<semantic_ir::SemanticDeclaration> display_method = fn_decl(mlc::String("Display_to_string"), mlc::Array<std::shared_ptr<ast::Param>>{display_self}, tstr(), make_unit_expr());
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_decl: concept keyword"), decl_extend::gen_trait_decl(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method}), mlc::String("concept")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_decl: trait identifier"), decl_extend::gen_trait_decl(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method}), mlc::String("Display")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_trait_decl_cpp matches string"), decl_cpp::print_cpp_declaration(decl_cpp::gen_trait_decl_cpp(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method})), decl_extend::gen_trait_decl(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method})));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_struct: std::function field"), decl_extend::gen_trait_struct(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method}), mlc::String("std::function")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_struct: vtable struct name"), decl_extend::gen_trait_struct(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method}), mlc::String("struct Display_vtable")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_struct: method name"), decl_extend::gen_trait_struct(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method}), mlc::String("to_string")));
context::CodegenContext trait_context = context::CodegenContext_for_type_body(context, mlc::String("Display"));
std::shared_ptr<cpp_ast::CppField> display_field = trait_struct_cpp::trait_method_field_cpp(trait_context, mlc::String("to_string"), mlc::Array<std::shared_ptr<ast::Param>>{display_self}, tstr());
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("trait_method_field_cpp: std::function"), decl_cpp::print_cpp_declaration(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(mlc::String(""), mlc::String("Display_vtable"), mlc::Array<std::shared_ptr<cpp_ast::CppField>>{display_field}, false))), mlc::String("std::function")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("trait_method_field_cpp: method name"), decl_cpp::print_cpp_declaration(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(mlc::String(""), mlc::String("Display_vtable"), mlc::Array<std::shared_ptr<cpp_ast::CppField>>{display_field}, false))), mlc::String("to_string")));
results.push_back(test_runner::assert_eq_str(mlc::String("trait_method_field_cpp struct matches string gen_trait_struct"), decl_cpp::print_cpp_declaration(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(mlc::String(""), mlc::String("Display_vtable"), mlc::Array<std::shared_ptr<cpp_ast::CppField>>{display_field}, false))), decl_extend::gen_trait_struct(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method})));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_trait_struct_cpp matches string"), decl_cpp::print_cpp_declaration(trait_struct_cpp::gen_trait_struct_cpp(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method})), decl_extend::gen_trait_struct(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method})));
std::shared_ptr<ast::Param> color_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("Color"))), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});
std::shared_ptr<semantic_ir::SemanticDeclaration> color_to_string = fn_decl(mlc::String("Color_to_string"), mlc::Array<std::shared_ptr<ast::Param>>{color_self}, tstr(), make_str_expr(mlc::String("red")));
std::shared_ptr<semantic_ir::SemanticDeclaration> extend_display_for_color = extend_decl(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SemanticDeclarationExtend: static_assert implements trait"), decl::gen_decl(extend_display_for_color, context), mlc::String("static_assert")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SemanticDeclarationExtend: trait name in assert"), decl::gen_decl(extend_display_for_color, context), mlc::String("Display")));
results.push_back(test_runner::assert_eq_str(mlc::String("extend_method_forward_segments_cpp matches string"), decl_cpp::print_cpp_declarations(decl_cpp::extend_method_forward_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{extend_display_for_color}, context)), decl::extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{extend_display_for_color}, context).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_extend_wrapper_protos_cpp matches string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_extend_wrapper_protos_cpp(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context)), decl_extend::gen_extend_wrapper_protos(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context)));
results.push_back(test_runner::assert_eq_str(mlc::String("extend_cpp_forward_segment_cpp matches string"), decl_cpp::print_cpp_declarations(decl_cpp::extend_cpp_forward_segment_cpp(extend_display_for_color, context)), decl::extend_cpp_forward_segment(extend_display_for_color, context)));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_fn_defs_cpp extend decl matches string fn_defs"), decl_cpp::print_cpp_declarations(decl_cpp::collect_fn_defs_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{extend_display_for_color}, context)), decl::collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{extend_display_for_color}, context).fn_defs.join(mlc::String(""))));
auto extend_context_resolve = [](context::CodegenContext codegen_context, mlc::String name) mutable { return context::CodegenContext_resolve(codegen_context, name); };
auto extend_gen_decl = [](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) mutable { return decl::gen_decl(declaration, codegen_context); };
mlc::String extend_string = decl_extend::gen_decl_extend(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context, extend_context_resolve, extend_gen_decl);
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_extend_cpp matches string gen_decl_extend"), decl_cpp::print_cpp_declarations(decl_cpp::gen_decl_extend_cpp(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context)), extend_string));
mlc::String trait_suffix_string = decl_extend::gen_decl_extend_trait_suffix(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context, extend_context_resolve);
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_extend_trait_suffix_cpp matches string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_decl_extend_trait_suffix_cpp(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context, extend_context_resolve)), trait_suffix_string));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> trait_suffix_native = decl_cpp::gen_decl_extend_trait_suffix_cpp(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context, extend_context_resolve);
int trait_static_assert_index = trait_suffix_native.size() - 1;
results.push_back(test_runner::assert_eq_str(mlc::String("extend_trait_static_assert_decl_cpp: Display generic trait"), decl_cpp::print_cpp_declaration(trait_suffix_native[trait_static_assert_index]), expr::static_assert_concept_for_type_line(mlc::String("Display"), mlc::String("Color"), mlc::String("Color does not implement Display"))));
std::shared_ptr<semantic_ir::SemanticDeclaration> color_visit = fn_decl(mlc::String("Color_visit"), mlc::Array<std::shared_ptr<ast::Param>>{color_self}, tstr(), make_str_expr(mlc::String("red")));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> expr_visitor_suffix = decl_cpp::gen_decl_extend_trait_suffix_cpp(mlc::String("Color"), mlc::String("ExprVisitor"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_visit}, context, extend_context_resolve);
int expr_visitor_static_assert_index = expr_visitor_suffix.size() - 1;
results.push_back(test_runner::assert_eq_str(mlc::String("extend_trait_static_assert_decl_cpp: ExprVisitor two-arg"), decl_cpp::print_cpp_declaration(expr_visitor_suffix[expr_visitor_static_assert_index]), expr::static_assert_concept_for_result_and_implementor_line(mlc::String("ExprVisitor"), mlc::String("mlc::String"), mlc::String("Color"), mlc::String("Color does not implement ExprVisitor"))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl_extend_trait_suffix_cpp: trait wrapper forward"), decl_cpp::print_cpp_declarations(decl_cpp::gen_decl_extend_trait_suffix_cpp(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context, extend_context_resolve)), mlc::String("inline mlc::String to_string")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationExtend matches string gen_decl"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(extend_display_for_color, context)), decl::gen_decl(extend_display_for_color, context)));
std::shared_ptr<cpp_ast::CppDeclaration> extend_display_native = decl_cpp::gen_decl_cpp(extend_display_for_color, context);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_display_children = cpp_decl_sequence_children(extend_display_native);
results.push_back(test_runner::assert_eq_int(mlc::String("gen_decl_cpp SemanticDeclarationExtend: CppDeclarationSequence child count"), extend_display_children.size(), 4));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationExtend: CppDeclarationSequence print parity"), decl_cpp::print_cpp_declaration(extend_display_native), decl_cpp::print_cpp_declarations(extend_display_children)));
std::shared_ptr<semantic_ir::SemanticDeclaration> color_bump = fn_decl(mlc::String("Color_bump"), mlc::Array<std::shared_ptr<ast::Param>>{color_self}, ti32(), make_int(1));
std::shared_ptr<semantic_ir::SemanticDeclaration> extend_color_only = extend_decl(mlc::String("Color"), mlc::String(""), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_bump});
mlc::String extend_only_string = decl_extend::gen_decl_extend(mlc::String("Color"), mlc::String(""), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_bump}, context, extend_context_resolve, extend_gen_decl);
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_extend_cpp non-trait matches string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_decl_extend_cpp(mlc::String("Color"), mlc::String(""), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_bump}, context)), extend_only_string));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationExtend non-trait matches string gen_decl"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(extend_color_only, context)), decl::gen_decl(extend_color_only, context)));
std::shared_ptr<ast::Param> box_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("Box"))), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});
std::shared_ptr<semantic_ir::SemanticDeclaration> item_assoc_bind = std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationAssocBind(mlc::String("Item"), ti32(), ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticDeclaration> bare_assoc_import = std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport(mlc::String(""), {}));
std::shared_ptr<semantic_ir::SemanticDeclaration> iter_next_method = fn_decl(mlc::String("Box_Iter_next"), mlc::Array<std::shared_ptr<ast::Param>>{box_self}, ti32(), make_int(0));
mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> assoc_extend_methods = mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{item_assoc_bind, bare_assoc_import, iter_next_method};
mlc::String assoc_extend_string = decl_extend::gen_decl_extend(mlc::String("Box"), mlc::String("Iter"), assoc_extend_methods, context, extend_context_resolve, extend_gen_decl);
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_extend_cpp with assoc entries matches string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_decl_extend_cpp(mlc::String("Box"), mlc::String("Iter"), assoc_extend_methods, context)), assoc_extend_string));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_extend_cpp assoc entries add no cpp from extend loop"), decl_cpp::print_cpp_declarations(decl_cpp::gen_decl_extend_cpp(mlc::String("Box"), mlc::String("Iter"), assoc_extend_methods, context)), decl_cpp::print_cpp_declarations(decl_cpp::gen_decl_extend_cpp(mlc::String("Box"), mlc::String("Iter"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{iter_next_method}, context))));
mlc::Array<std::shared_ptr<ast::TypeVariant>> box_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Box"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("val"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})};
std::shared_ptr<semantic_ir::SemanticDeclaration> box_assoc_extend = extend_decl(mlc::String("Box"), mlc::String("Iter"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{item_assoc_bind});
context::StructUsingData box_struct_using_data = module::build_struct_using_data(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{box_assoc_extend}, context);
context::CodegenContext box_struct_using_context = context::context_with_struct_using_data(context, box_struct_using_data);
mlc::Array<context::StructUsingEntry> box_struct_using_entries = box_struct_using_data.entries.get(mlc::String("Box"));
results.push_back(test_runner::assert_true(mlc::String("build_struct_using_data: Box has Item entry"), box_struct_using_data.entries.has(mlc::String("Box"))));
results.push_back(test_runner::assert_eq_int(mlc::String("build_struct_using_data: one assoc bind entry"), box_struct_using_entries.size(), 1));
results.push_back(test_runner::assert_eq_str(mlc::String("build_struct_using_data: Item alias"), box_struct_using_entries[0].alias, mlc::String("Item")));
results.push_back(test_runner::assert_eq_str(mlc::String("build_struct_using_data: int type_cpp"), box_struct_using_entries[0].type_cpp, mlc::String("int")));
results.push_back(test_runner::assert_eq_str(mlc::String("build_struct_using_data: lines in sync"), box_struct_using_data.lines.get(mlc::String("Box"))[0], mlc::String("using Item = int;\n")));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> box_body_native = decl_cpp::gen_type_decl_body_cpp(box_struct_using_context, mlc::String("Box"), {}, box_variants, {});
results.push_back(test_runner::assert_true(mlc::String("gen_type_decl_body_cpp: native CppUsing Item = int"), cpp_decl_list_contains_using(box_body_native, mlc::String("Item"), mlc::String("int"))));
results.push_back(test_runner::assert_eq_int(mlc::String("gen_type_decl_body_cpp: struct and using decl count"), box_body_native.size(), 2));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_type_decl_body_cpp: struct Box in print"), decl_cpp::print_cpp_declarations(box_body_native), mlc::String("struct Box")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_type_decl_body_cpp: using Item = int in print"), decl_cpp::print_cpp_declarations(box_body_native), mlc::String("using Item = int;")));
context::CodegenContext color_extend_context = context::CodegenContext_for_type_body(context, mlc::String("Color"));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_method_native = decl_cpp::gen_decl_extend_cpp(mlc::String("Color"), mlc::String(""), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_bump}, context);
results.push_back(test_runner::assert_eq_str(mlc::String("extend method native body matches string gen_fn_decl"), decl_cpp::print_cpp_declaration(extend_method_native[0]), decl::gen_fn_decl(mlc::String("Color_bump"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{color_self}, ti32(), make_int(1), color_extend_context)));
results.push_back(test_runner::assert_true(mlc::String("extend method native compact body not gen_fn_decl_cpp indent"), decl_cpp::print_cpp_declaration(extend_method_native[0]) != decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_decl_cpp(mlc::String("Color_bump"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{color_self}, ti32(), make_int(1), color_extend_context))));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> trait_extend_native = decl_cpp::gen_decl_extend_cpp(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_to_string}, context);
results.push_back(test_runner::assert_eq_str(mlc::String("extend trait method native body matches string gen_fn_decl"), decl_cpp::print_cpp_declaration(trait_extend_native[0]), decl::gen_fn_decl(mlc::String("Color_to_string"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{color_self}, tstr(), make_str_expr(mlc::String("red")), color_extend_context)));
std::shared_ptr<ast::Param> integer_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});
mlc::String extern_method_string = decl_extend::gen_extend_extern_method(mlc::String("i32_to_string"), mlc::String("i32"), mlc::Array<std::shared_ptr<ast::Param>>{integer_self}, tstr(), mlc::String("Display"), context, extend_context_resolve);
results.push_back(test_runner::assert_eq_str(mlc::String("gen_extend_extern_method_cpp matches string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_extend_extern_method_cpp(mlc::String("i32_to_string"), mlc::String("i32"), mlc::Array<std::shared_ptr<ast::Param>>{integer_self}, tstr(), mlc::String("Display"), context, extend_context_resolve)), extern_method_string));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_extend_extern_method_cpp: runtime to_string call"), decl_cpp::print_cpp_declarations(decl_cpp::gen_extend_extern_method_cpp(mlc::String("i32_to_string"), mlc::String("i32"), mlc::Array<std::shared_ptr<ast::Param>>{integer_self}, tstr(), mlc::String("Display"), context, extend_context_resolve)), mlc::String("mlc::to_string(self)")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_extend_extern_method_cpp: non-i32 empty"), decl_cpp::print_cpp_declarations(decl_cpp::gen_extend_extern_method_cpp(mlc::String("Color_to_string"), mlc::String("Color"), mlc::Array<std::shared_ptr<ast::Param>>{color_self}, tstr(), mlc::String("Display"), context, extend_context_resolve)), mlc::String("")));
std::shared_ptr<semantic_ir::SemanticDeclaration> exported_fn = std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExported(fn_decl(mlc::String("square"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), make_bin(mlc::String("*"), make_ident(mlc::String("value")), make_ident(mlc::String("value"))))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SemanticDeclarationExported: generates body"), decl::gen_decl(exported_fn, context), mlc::String("int square")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SemanticDeclarationExported: contains multiplication"), decl::gen_decl(exported_fn, context), mlc::String("value * value")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto SemanticDeclarationFn: produces prototype"), decl::gen_proto(add_decl, context), mlc::String("int add(int left, int right) noexcept;\n")));
results.push_back(test_runner::assert_true(mlc::String("transform_decl SemanticDeclarationFn span from body"), transformed_fn_decl_span(mlc::String("fn probe() -> i32 = 42")).line > 0));
results.push_back(test_runner::assert_true(mlc::String("transform_decl SemanticDeclarationType span from name"), transformed_type_decl_span(mlc::String("type Box = { value: i32 }")).line > 0));
results.push_back(test_runner::assert_true(mlc::String("transform_decl SemanticDeclarationTrait span from name"), transformed_trait_decl_span(mlc::String("trait Display { fn to_string(self: Display) -> string = \"\" }")).line > 0));
results.push_back(test_runner::assert_true(mlc::String("transform_decl SemanticDeclarationExtend span from name"), transformed_extend_decl_span(mlc::String("extend Color : Display { fn to_string(self: Color) -> string = \"red\" }")).line > 0));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto SemanticDeclarationType: empty string"), decl::gen_proto(point_type, context), mlc::String("")));
std::shared_ptr<semantic_ir::SemanticDeclaration> main_decl = fn_decl(mlc::String("main"), {}, tunit(), make_unit_expr());
std::shared_ptr<semantic_ir::SemanticDeclaration> other_decl = fn_decl(mlc::String("other"), {}, tunit(), make_unit_expr());
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: finds main"), decl::decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{main_decl, other_decl})));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: no main"), !decl::decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{other_decl})));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: empty list"), !decl::decls_have_main({})));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto_cpp matches string"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_proto_cpp(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), context)), decl::gen_fn_proto(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), context)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto_cpp: template matches string"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_proto_cpp(mlc::String("identity"), mlc::Array<mlc::String>{mlc::String("T")}, mlc::Array<mlc::Array<mlc::String>>{{}}, mlc::Array<std::shared_ptr<ast::Param>>{generic_param(mlc::String("value"))}, unk(), context)), decl::gen_fn_proto(mlc::String("identity"), mlc::Array<mlc::String>{mlc::String("T")}, mlc::Array<mlc::Array<mlc::String>>{{}}, mlc::Array<std::shared_ptr<ast::Param>>{generic_param(mlc::String("value"))}, unk(), context)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: unit body empty"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(make_unit_expr(), context)), mlc::String("")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: ident return matches string"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(make_ident(mlc::String("value")), context)), return_body::gen_return_body(make_ident(mlc::String("value")), context)));
std::shared_ptr<semantic_ir::SemanticExpression> question_body = make_question(make_int(7));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_return_body_cpp: question native get_if unwrap"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(question_body, context)), mlc::String("std::get_if<1>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_return_body_cpp: question native ok return"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(question_body, context)), mlc::String("std::get<0>(__try_ret).field0")));
std::shared_ptr<semantic_ir::SemanticExpression> block_return_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("counter"), false, make_int(1), ti32(), ast::span_unknown()))}, make_ident(mlc::String("counter")), ti32(), ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: block let and return matches string"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(block_return_body, context)), return_body::gen_return_body(block_return_body, context)));
std::shared_ptr<semantic_ir::SemanticExpression> block_unit_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("counter"), false, make_int(1), ti32(), ast::span_unknown()))}, make_unit_expr(), tunit(), ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: block unit tail matches string"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(block_unit_body, context)), return_body::gen_return_body(block_unit_body, context)));
std::shared_ptr<semantic_ir::SemanticExpression> if_return_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(make_bool(true), make_int(1), make_int(0), ti32(), ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: if else return native"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(if_return_body, context)), mlc::String("if (true) {\n  return 1;\n} else {\n  return 0;\n}\n")));
std::shared_ptr<semantic_ir::SemanticExpression> else_if_return_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(make_bool(true), make_int(1), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(make_bool(false), make_int(2), make_int(3), ti32(), ast::span_unknown())), ti32(), ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: else-if return native"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(else_if_return_body, context)), mlc::String("if (true) {\n  return 1;\n} else if (false) {\n  return 2;\n} else {\n  return 3;\n}\n")));
std::shared_ptr<semantic_ir::SemanticExpression> if_then_block_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(make_bool(true), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("temporary"), false, make_int(1), ti32(), ast::span_unknown()))}, make_ident(mlc::String("temporary")), ti32(), ast::span_unknown())), make_int(0), ti32(), ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: if then block prefix native"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(if_then_block_body, context)), mlc::String("if (true) {\n  auto temporary = 1;\n  return temporary;\n} else {\n  return 0;\n}\n")));
std::shared_ptr<semantic_ir::SemanticExpression> match_arm_block_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch(make_int(1), mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown())), false, make_unit_expr(), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("temporary"), false, make_int(2), ti32(), ast::span_unknown()))}, make_ident(mlc::String("temporary")), ti32(), ast::span_unknown()))})}, ti32(), ast::span_unknown()));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_return_body_cpp: match arm block prefix native"), stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(match_arm_block_body, context)), mlc::String("auto temporary = 2")));
std::shared_ptr<semantic_ir::SemanticExpression> string_match_return_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch(make_ident(mlc::String("word")), mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{std::make_shared<ast::Pattern>(ast::PatternStringLit(mlc::String("fn"), ast::span_unknown())), false, make_unit_expr(), make_int(1)}), std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{std::make_shared<ast::Pattern>(ast::PatternStringLit(mlc::String("type"), ast::span_unknown())), false, make_unit_expr(), make_int(2)}), std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown())), false, make_unit_expr(), make_int(0)})}, ti32(), ast::span_unknown()));
mlc::String string_match_return_cpp = stmt_cpp::print_cpp_statements(return_body::gen_return_body_cpp(string_match_return_body, context));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_return_body_cpp: string match else if chain"), string_match_return_cpp, mlc::String("else if")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_return_body_cpp: string match subject compare"), string_match_return_cpp, mlc::String("__match_subject ==")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("gen_return_body_cpp: string match avoids std::visit"), string_match_return_cpp, mlc::String("std::visit")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_return_body_cpp: string match matches string gen_return_body"), string_match_return_cpp, return_body::gen_return_body(string_match_return_body, context)));
std::shared_ptr<semantic_ir::SemanticExpression> while_return_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWhile(make_bool(true), mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(make_int(0), ast::span_unknown()))}, tunit(), ast::span_unknown()));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_fn_decl_cpp: while return body native"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_decl_cpp(mlc::String("spin"), {}, {}, {}, tunit(), while_return_body, context)), mlc::String("while (true)")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl_cpp: identity native return"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_decl_cpp(mlc::String("identity"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), make_ident(mlc::String("value")), context)), mlc::String("int identity(int value) noexcept{\n  return value;\n}\n")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_fn_decl_cpp: template native prefix"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_decl_cpp(mlc::String("identity"), mlc::Array<mlc::String>{mlc::String("T")}, mlc::Array<mlc::Array<mlc::String>>{{}}, mlc::Array<std::shared_ptr<ast::Param>>{generic_param(mlc::String("value"))}, unk(), make_ident(mlc::String("value")), context)), mlc::String("template<typename T>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_fn_decl_cpp: template native return"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_decl_cpp(mlc::String("identity"), mlc::Array<mlc::String>{mlc::String("T")}, mlc::Array<mlc::Array<mlc::String>>{{}}, mlc::Array<std::shared_ptr<ast::Param>>{generic_param(mlc::String("value"))}, unk(), make_ident(mlc::String("value")), context)), mlc::String("return value")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto_cpp: main native proto"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_proto_cpp(mlc::String("main"), {}, {}, {}, ti32(), context)), decl::gen_fn_proto(mlc::String("main"), {}, {}, {}, ti32(), context)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_fn_decl_cpp: main native argc argv"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_decl_cpp(mlc::String("main"), {}, {}, {}, ti32(), make_int(0), context)), mlc::String("int argc, char** argv")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_fn_decl_cpp: main native set_args preamble"), decl_cpp::print_cpp_declaration(decl_cpp::gen_fn_decl_cpp(mlc::String("main"), {}, {}, {}, ti32(), make_int(0), context)), mlc::String("mlc::io::set_args")));
results.push_back(test_runner::assert_eq_str(mlc::String("main_program_parameter_items_cpp matches string"), decl_cpp::main_program_parameter_items_cpp().join(mlc::String(", ")), expr::main_program_parameter_list()));
results.push_back(test_runner::assert_eq_str(mlc::String("main_set_args_preamble_cpp native prelude"), stmt_cpp::print_cpp_statements(decl_cpp::main_set_args_preamble_cpp()), mlc::String("mlc::io::set_args(std::vector<mlc::String>((argv + 1), (argv + argc)));\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_type_decl_fwd_cpp: single record empty"), decl_cpp::print_cpp_declarations(decl_cpp::gen_type_decl_fwd_cpp(context, mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})})), mlc::String("")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_type_decl_fwd_cpp: sum type matches string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_type_decl_fwd_cpp(context, mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))})), type_gen::gen_type_decl_fwd_only(context, mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))})));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_type_decl_body_cpp: sum type unit variants match string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_type_decl_body_cpp(context, mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))}, {})), type_gen::gen_type_decl_body_only(context, mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))})));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_type_decl_body_cpp: record struct fields"), decl_cpp::print_cpp_declarations(decl_cpp::gen_type_decl_body_cpp(context, mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})}, {})), mlc::String("int x")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_type_decl_body_cpp: derive Display Color_to_string"), decl_cpp::print_cpp_declarations(decl_cpp::gen_type_decl_body_cpp(context, mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))}, mlc::Array<mlc::String>{mlc::String("Display")})), mlc::String("Color_to_string")));
std::shared_ptr<semantic_ir::SemanticDeclaration> color_sum = type_decl(mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))}, {});
std::shared_ptr<semantic_ir::SemanticDeclaration> display_trait = trait_decl(mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_method});
results.push_back(test_runner::assert_eq_str(mlc::String("collect_decl_parts_cpp phase 0 type matches string"), decl_cpp::print_cpp_declarations(decl_cpp::collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_sum}, context, 0)), decl::collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_sum}, context, 0).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_decl_parts_cpp phase 1 type matches string"), decl_cpp::print_cpp_declarations(decl_cpp::collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_sum}, context, 1)), decl::collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_sum}, context, 1).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_decl_parts_cpp phase 0 trait matches string"), decl_cpp::print_cpp_declarations(decl_cpp::collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_trait}, context, 0)), decl::collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_trait}, context, 0).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_decl_parts_cpp phase 2 fn matches string"), decl_cpp::print_cpp_declarations(decl_cpp::collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{add_decl}, context, 2)), decl::collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{add_decl}, context, 2).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_decl_parts_cpp phase 4 trait matches string"), decl_cpp::print_cpp_declarations(decl_cpp::collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_trait}, context, 4)), decl::collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_trait}, context, 4).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_int(mlc::String("collect_decl_parts_cpp unknown phase returns empty"), decl_cpp::collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{add_decl}, context, 99).size(), 0));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_fn_defs_cpp trait matches string fn_defs"), decl_cpp::print_cpp_declarations(decl_cpp::collect_fn_defs_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_trait}, context)), decl::collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{display_trait}, context).fn_defs.join(mlc::String(""))));
mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> bundle_declarations = mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_sum, display_trait};
decl::DeclPartsBundle string_bundle = decl::collect_all_decl_parts(bundle_declarations, context);
decl_cpp::DeclPartsBundleCpp native_bundle = decl_cpp::collect_all_decl_parts_cpp(bundle_declarations, context);
results.push_back(test_runner::assert_eq_str(mlc::String("collect_all_decl_parts_cpp type_fwds matches string"), decl_cpp::print_cpp_declarations(native_bundle.type_fwds), string_bundle.type_fwds.join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_all_decl_parts_cpp type_defs matches string"), decl_cpp::print_cpp_declarations(native_bundle.type_defs), string_bundle.type_defs.join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_all_decl_parts_cpp fn_protos matches string"), decl_cpp::print_cpp_declarations(native_bundle.fn_protos), string_bundle.fn_protos.join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_all_decl_parts_cpp fn_protos add_decl matches string"), decl_cpp::print_cpp_declarations(decl_cpp::collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{add_decl}, context).fn_protos), decl::collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{add_decl}, context).fn_protos.join(mlc::String(""))));
std::shared_ptr<semantic_ir::SemanticDeclaration> exported_color = std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExported(color_sum));
mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> exported_extend_bundle = mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{exported_color, extend_display_for_color};
results.push_back(test_runner::assert_eq_str(mlc::String("extend_helper_protos_for_exported_types_cpp matches string"), decl_cpp::print_cpp_declarations(decl_cpp::extend_helper_protos_for_exported_types_cpp(exported_extend_bundle, context)), decl::extend_helper_protos_for_exported_types(exported_extend_bundle, context).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("extend_helper_protos_for_exported_types_cpp skips non-exported"), decl_cpp::print_cpp_declarations(decl_cpp::extend_helper_protos_for_exported_types_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_sum, extend_display_for_color}, context)), decl::extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{color_sum, extend_display_for_color}, context).join(mlc::String(""))));
std::shared_ptr<semantic_ir::SemanticDeclaration> extend_display_extern = extend_decl(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{fn_decl(mlc::String("Color_to_string"), mlc::Array<std::shared_ptr<ast::Param>>{color_self}, tstr(), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionExtern(tunit(), ast::span_unknown())))});
results.push_back(test_runner::assert_eq_str(mlc::String("extend_helper_protos_for_exported_types_cpp skips extern method"), decl_cpp::print_cpp_declarations(decl_cpp::extend_helper_protos_for_exported_types_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{exported_color, extend_display_extern}, context)), decl::extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{exported_color, extend_display_extern}, context).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_all_decl_parts_cpp fn_protos exported extend matches string"), decl_cpp::print_cpp_declarations(decl_cpp::collect_all_decl_parts_cpp(exported_extend_bundle, context).fn_protos), decl::collect_all_decl_parts(exported_extend_bundle, context).fn_protos.join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("collect_all_decl_parts_cpp fn_defs matches string"), decl_cpp::print_cpp_declarations(native_bundle.fn_defs), string_bundle.fn_defs.join(mlc::String(""))));
mlc::Array<std::shared_ptr<ast::TypeVariant>> point_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})};
mlc::Array<std::shared_ptr<ast::TypeVariant>> color_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))};
mlc::Array<std::shared_ptr<ast::TypeVariant>> point_hash_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("tag"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("flag"), std::make_shared<ast::TypeExpr>((ast::TyBool{}))), ast_builders::field_definition(mlc::String("label"), std::make_shared<ast::TypeExpr>((ast::TyString{})))})};
mlc::Array<std::shared_ptr<ast::TypeVariant>> hue_hash_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("North")), ast_builders::unit_variant(mlc::String("South"))};
mlc::Array<std::shared_ptr<ast::TypeVariant>> empty_hash_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Empty"), {})};
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_display_cpp record: Point_to_string"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_display_cpp(mlc::String("Point"), point_variants)), mlc::String("Point_to_string")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_display_cpp record: mlc::to_string field"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_display_cpp(mlc::String("Point"), point_variants)), mlc::String("mlc::to_string(self.x)")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_display_cpp sum: Color_to_string"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_display_cpp(mlc::String("Color"), color_variants)), mlc::String("Color_to_string")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_display_cpp sum: Red variant"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_display_cpp(mlc::String("Color"), color_variants)), mlc::String("holds_alternative<Red>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_eq_cpp record: operator=="), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_eq_cpp(mlc::String("Point"), point_variants)), mlc::String("operator==")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_eq_cpp record: field compare"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_eq_cpp(mlc::String("Point"), point_variants)), mlc::String("a.x == b.x")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_eq_cpp sum: operator=="), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_eq_cpp(mlc::String("Color"), color_variants)), mlc::String("operator==")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_eq_cpp sum: discriminant compare"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_eq_cpp(mlc::String("Color"), color_variants)), mlc::String("a._ == b._")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_ord_cpp record: operator<"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_ord_cpp(mlc::String("Point"), point_variants)), mlc::String("operator<")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_ord_cpp record: field compare"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_ord_cpp(mlc::String("Point"), point_variants)), mlc::String("a.x < b.x")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_ord_cpp sum: operator<"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_ord_cpp(mlc::String("Color"), color_variants)), mlc::String("operator<")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_ord_cpp sum: discriminant compare"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_ord_cpp(mlc::String("Color"), color_variants)), mlc::String("a._ < b._")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_hash_cpp record: struct hash"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_hash_cpp(mlc::String("Point"), point_hash_variants)), mlc::String("struct hash<Point>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_hash_cpp record: combines i32"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_hash_cpp(mlc::String("Point"), point_hash_variants)), mlc::String("std::hash<int>{}(self.tag)")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_hash_cpp sum: holds_alternative"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_hash_cpp(mlc::String("Hue"), hue_hash_variants)), mlc::String("std::holds_alternative<North>")));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> hash_record_native = derive_methods_cpp::gen_derive_hash_cpp(mlc::String("Point"), point_hash_variants);
results.push_back(test_runner::assert_true(mlc::String("gen_derive_hash_cpp record: CppStdHashSpecialization"), cpp_decl_is_std_hash_specialization(hash_record_native[0], mlc::String("Point"))));
results.push_back(test_runner::assert_true(mlc::String("gen_derive_hash_cpp record: not CppDeclarationFragment"), !cpp_decl_is_fragment(hash_record_native[0])));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> hash_sum_native = derive_methods_cpp::gen_derive_hash_cpp(mlc::String("Hue"), hue_hash_variants);
results.push_back(test_runner::assert_true(mlc::String("gen_derive_hash_cpp sum: CppStdHashSpecialization"), cpp_decl_is_std_hash_specialization(hash_sum_native[0], mlc::String("Hue"))));
results.push_back(test_runner::assert_true(mlc::String("gen_derive_hash_cpp sum: not CppDeclarationFragment"), !cpp_decl_is_fragment(hash_sum_native[0])));
results.push_back(test_runner::assert_true(mlc::String("gen_derive_hash_cpp record: body no CppStatementFragment"), !cpp_stmt_list_contains_fragment(cpp_std_hash_body_statements(hash_record_native[0]))));
results.push_back(test_runner::assert_true(mlc::String("gen_derive_hash_cpp sum: body no CppStatementFragment"), !cpp_stmt_list_contains_fragment(cpp_std_hash_body_statements(hash_sum_native[0]))));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> empty_hash_native = derive_methods_cpp::gen_derive_hash_cpp(mlc::String("Empty"), empty_hash_variants);
results.push_back(test_runner::assert_true(mlc::String("gen_derive_hash_cpp empty record: body no CppStatementFragment"), !cpp_stmt_list_contains_fragment(cpp_std_hash_body_statements(empty_hash_native[0]))));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> point_hash_body = decl_cpp::gen_type_decl_body_cpp(context, mlc::String("Point"), {}, point_hash_variants, mlc::Array<mlc::String>{mlc::String("Hash")});
results.push_back(test_runner::assert_true(mlc::String("gen_type_decl_body_cpp record Hash: CppStdHashSpecialization"), cpp_decl_list_contains_std_hash_specialization(point_hash_body, mlc::String("Point"))));
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> hue_hash_body = decl_cpp::gen_type_decl_body_cpp(context, mlc::String("Hue"), {}, hue_hash_variants, mlc::Array<mlc::String>{mlc::String("Hash")});
results.push_back(test_runner::assert_true(mlc::String("gen_type_decl_body_cpp sum Hash: CppStdHashSpecialization"), cpp_decl_list_contains_std_hash_specialization(hue_hash_body, mlc::String("Hue"))));
results.push_back(test_runner::assert_true(mlc::String("gen_type_decl_body_cpp record Hash: body no CppStatementFragment"), !cpp_decl_list_hash_body_contains_fragment(point_hash_body)));
results.push_back(test_runner::assert_true(mlc::String("gen_type_decl_body_cpp sum Hash: body no CppStatementFragment"), !cpp_decl_list_hash_body_contains_fragment(hue_hash_body)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_methods_cpp: Display and Eq"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_methods_cpp(mlc::String("Point"), point_variants, mlc::Array<mlc::String>{mlc::String("Display"), mlc::String("Eq")})), mlc::String("Point_to_string")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_derive_methods_cpp: Display and Eq operator"), decl_cpp::print_cpp_declarations(derive_methods_cpp::gen_derive_methods_cpp(mlc::String("Point"), point_variants, mlc::Array<mlc::String>{mlc::String("Display"), mlc::String("Eq")})), mlc::String("operator==")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_type_decl_body_cpp: native Hash wired"), decl_cpp::print_cpp_declarations(decl_cpp::gen_type_decl_body_cpp(context, mlc::String("Hue"), {}, hue_hash_variants, mlc::Array<mlc::String>{mlc::String("Hash")})), mlc::String("struct hash<Hue>")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp DeclFn native return"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(fn_decl(mlc::String("add"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), make_bin(mlc::String("+"), make_ident(mlc::String("left")), make_ident(mlc::String("right")))), context)), mlc::String("int add(int left, int right) noexcept{\n  return (left + right);\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp DeclFn matches string"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(fn_decl(mlc::String("add"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), make_bin(mlc::String("+"), make_ident(mlc::String("left")), make_ident(mlc::String("right")))), context)), decl::gen_proto(fn_decl(mlc::String("add"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), make_bin(mlc::String("+"), make_ident(mlc::String("left")), make_ident(mlc::String("right")))), context)));
results.push_back(test_runner::assert_eq_str(mlc::String("decl_segment_cpp phase 2 SemanticDeclarationFn matches string"), decl_cpp::print_cpp_declarations(decl_cpp::decl_segment_cpp(add_decl, context, 2)), decl::collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{add_decl}, context, 2).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("decl_segment_cpp phase 2 SemanticDeclarationExported fn matches string"), decl_cpp::print_cpp_declarations(decl_cpp::decl_segment_cpp(exported_fn, context, 2)), decl::collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{exported_fn}, context, 2).join(mlc::String(""))));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationType record matches native body print"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(point_type, context)), decl_cpp::print_cpp_declarations(decl_cpp::gen_type_decl_body_cpp(context, mlc::String("Point"), {}, point_variants, {}))));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationType sum matches string gen_decl"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(color_type, context)), decl::gen_decl(color_type, context)));
std::shared_ptr<cpp_ast::CppDeclaration> color_sum_native = decl_cpp::gen_decl_cpp(color_type, context);
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> color_sum_children = cpp_decl_sequence_children(color_sum_native);
results.push_back(test_runner::assert_eq_int(mlc::String("gen_decl_cpp SemanticDeclarationType sum: CppDeclarationSequence child count"), color_sum_children.size(), 7));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationType sum: CppDeclarationSequence print parity"), decl_cpp::print_cpp_declaration(color_sum_native), decl_cpp::print_cpp_declarations(color_sum_children)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationTrait matches string gen_decl"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(display_trait, context)), decl::gen_decl(display_trait, context)));
std::shared_ptr<semantic_ir::SemanticDeclaration> square_decl = fn_decl(mlc::String("square"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), make_bin(mlc::String("*"), make_ident(mlc::String("value")), make_ident(mlc::String("value"))));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationExported matches unwrapped SemanticDeclarationFn"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(exported_fn, context)), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(square_decl, context))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl_cpp SemanticDeclarationExported matches string gen_decl signature"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(exported_fn, context)), mlc::String("int square(int value)")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationImport empty"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport(mlc::String(""), {})), context)), mlc::String("")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationAssocBind empty"), decl_cpp::print_cpp_declaration(decl_cpp::gen_decl_cpp(std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationAssocBind(mlc::String("Item"), ti32(), ast::span_unknown())), context)), mlc::String("")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationType empty"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(point_type, context)), mlc::String("")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationTrait empty"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(display_trait, context)), mlc::String("")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationExtend matches string gen_proto"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(extend_display_for_color, context)), decl::gen_proto(extend_display_for_color, context)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationExported matches unwrapped SemanticDeclarationFn"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(exported_fn, context)), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(square_decl, context))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_proto_cpp SemanticDeclarationExported matches string gen_proto signature"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(exported_fn, context)), mlc::String("int square(int value)")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationImport empty"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport(mlc::String(""), {})), context)), mlc::String("")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationAssocBind empty"), decl_cpp::print_cpp_declaration(decl_cpp::gen_proto_cpp(std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationAssocBind(mlc::String("Item"), ti32(), ast::span_unknown())), context)), mlc::String("")));
std::shared_ptr<semantic_ir::SemanticDeclaration> extern_fn = fn_decl(mlc::String("external_helper"), {}, tunit(), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionExtern(tunit(), ast::span_unknown())));
std::shared_ptr<cpp_ast::CppDeclaration> extern_decl_native = decl_cpp::gen_decl_cpp(extern_fn, context);
results.push_back(test_runner::assert_true(mlc::String("gen_decl_cpp SemanticDeclarationFn extern: native CppDeclarationEmpty"), is_cpp_decl_empty(extern_decl_native)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_decl_cpp SemanticDeclarationFn extern empty print"), decl_cpp::print_cpp_declaration(extern_decl_native), mlc::String("")));
std::shared_ptr<cpp_ast::CppDeclaration> extern_proto_native = decl_cpp::gen_proto_cpp(extern_fn, context);
results.push_back(test_runner::assert_true(mlc::String("gen_proto_cpp SemanticDeclarationFn extern: native CppDeclarationEmpty"), is_cpp_decl_empty(extern_proto_native)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationFn extern empty print"), decl_cpp::print_cpp_declaration(extern_proto_native), mlc::String("")));
std::shared_ptr<cpp_ast::CppDeclaration> extend_proto_extern_native = decl_cpp::gen_proto_cpp(extend_display_extern, context);
results.push_back(test_runner::assert_true(mlc::String("gen_proto_cpp SemanticDeclarationExtend all-extern: native CppDeclarationEmpty"), is_cpp_decl_empty(extend_proto_extern_native)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto_cpp SemanticDeclarationExtend all-extern empty print"), decl_cpp::print_cpp_declaration(extend_proto_extern_native), mlc::String("")));
results.push_back(test_runner::assert_true(mlc::String("gen_decl_cpp SemanticDeclarationImport: native CppDeclarationEmpty"), is_cpp_decl_empty(decl_cpp::gen_decl_cpp(std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport(mlc::String(""), {})), context))));
results.push_back(test_runner::assert_true(mlc::String("gen_decl_cpp SemanticDeclarationAssocBind: native CppDeclarationEmpty"), is_cpp_decl_empty(decl_cpp::gen_decl_cpp(std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationAssocBind(mlc::String("Item"), ti32(), ast::span_unknown())), context))));
return results;
}

} // namespace test_decl_gen
