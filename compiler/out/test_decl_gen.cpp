#include "test_decl_gen.hpp"

#include "test_runner.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "ast_builders.hpp"
#include "codegen_test_helpers.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"

namespace test_decl_gen {

using namespace test_runner;
using namespace ast;
using namespace context;
using namespace decl;
using namespace ast_builders;
using namespace codegen_test_helpers;
using namespace semantic_ir;
using namespace registry;
using namespace ast_tokens;

std::shared_ptr<registry::Type> ti32() noexcept;

std::shared_ptr<registry::Type> tstr() noexcept;

std::shared_ptr<registry::Type> tunit() noexcept;

std::shared_ptr<registry::Type> unk() noexcept;

std::shared_ptr<semantic_ir::SExpr> make_ident(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SExpr> make_bin(mlc::String op, std::shared_ptr<semantic_ir::SExpr> left, std::shared_ptr<semantic_ir::SExpr> right) noexcept;

std::shared_ptr<semantic_ir::SExpr> make_unit_expr() noexcept;

std::shared_ptr<ast::Param> iparam(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SDecl> fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body) noexcept;

std::shared_ptr<semantic_ir::SExpr> make_str_expr(mlc::String text) noexcept;

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept;

std::shared_ptr<registry::Type> ti32() noexcept{return std::make_shared<registry::Type>((registry::TI32{}));}

std::shared_ptr<registry::Type> tstr() noexcept{return std::make_shared<registry::Type>((registry::TString{}));}

std::shared_ptr<registry::Type> tunit() noexcept{return std::make_shared<registry::Type>((registry::TUnit{}));}

std::shared_ptr<registry::Type> unk() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

std::shared_ptr<semantic_ir::SExpr> make_ident(mlc::String name) noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent(name, unk(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SExpr> make_bin(mlc::String op, std::shared_ptr<semantic_ir::SExpr> left, std::shared_ptr<semantic_ir::SExpr> right) noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(op, left, right, unk(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SExpr> make_unit_expr() noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(tunit(), ast::span_unknown()));}

std::shared_ptr<ast::Param> iparam(mlc::String name) noexcept{return std::make_shared<ast::Param>(ast::Param{name, false, std::make_shared<ast::TypeExpr>((ast::TyI32{}))});}

std::shared_ptr<semantic_ir::SDecl> fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body) noexcept{return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclFn(name, {}, {}, params, return_type, body));}

std::shared_ptr<semantic_ir::SExpr> make_str_expr(mlc::String text) noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr(text, tstr(), ast::span_unknown()));}

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
context::CodegenContext context = codegen_test_helpers::empty_codegen_context();
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: zero params"), decl::gen_fn_proto(mlc::String("compute"), {}, {}, {}, tunit(), context), mlc::String("void compute() noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: single i32 param"), decl::gen_fn_proto(mlc::String("double_it"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), context), mlc::String("int double_it(int value) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: two params"), decl::gen_fn_proto(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), context), mlc::String("int add(int left, int right) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: string return type"), decl::gen_fn_proto(mlc::String("greet"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("name"), false, std::make_shared<ast::TypeExpr>((ast::TyString{}))})}, tstr(), context), mlc::String("mlc::String greet(mlc::String name) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: identity function"), decl::gen_fn_decl(mlc::String("identity"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), make_ident(mlc::String("value")), context), mlc::String("int identity(int value) noexcept{\nreturn value;\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: addition"), decl::gen_fn_decl(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), make_bin(mlc::String("+"), make_ident(mlc::String("left")), make_ident(mlc::String("right"))), context), mlc::String("int add(int left, int right) noexcept{\nreturn (left + right);\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: unit return"), decl::gen_fn_decl(mlc::String("nothing"), {}, {}, {}, tunit(), make_unit_expr(), context), mlc::String("void nothing() noexcept{\nreturn /* unit */;\n}\n")));
std::shared_ptr<semantic_ir::SDecl> add_decl = fn_decl(mlc::String("add"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("left")), iparam(mlc::String("right"))}, ti32(), make_bin(mlc::String("+"), make_ident(mlc::String("left")), make_ident(mlc::String("right"))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains return type"), decl::gen_decl(add_decl, context), mlc::String("int add")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains body"), decl::gen_decl(add_decl, context), mlc::String("return (left + right)")));
std::shared_ptr<semantic_ir::SDecl> point_type = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType(mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))), ast_builders::field_definition(mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})))})}));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: record struct"), decl::gen_decl(point_type, context), mlc::String("Point")));
std::shared_ptr<semantic_ir::SDecl> color_type = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType(mlc::String("Color"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))}));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: unit variants"), decl::gen_decl(color_type, context), mlc::String("Red")));
std::shared_ptr<ast::Param> display_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("Display")))});
std::shared_ptr<semantic_ir::SDecl> display_method = fn_decl(mlc::String("Display_to_string"), mlc::Array<std::shared_ptr<ast::Param>>{display_self}, tstr(), make_unit_expr());
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_decl: concept keyword"), decl::gen_trait_decl(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SDecl>>{display_method}), mlc::String("concept")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_decl: trait identifier"), decl::gen_trait_decl(context, mlc::String("Display"), {}, mlc::Array<std::shared_ptr<semantic_ir::SDecl>>{display_method}), mlc::String("Display")));
std::shared_ptr<ast::Param> color_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("Color")))});
std::shared_ptr<semantic_ir::SDecl> color_to_string = fn_decl(mlc::String("Color_to_string"), mlc::Array<std::shared_ptr<ast::Param>>{color_self}, tstr(), make_str_expr(mlc::String("red")));
std::shared_ptr<semantic_ir::SDecl> extend_display_for_color = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExtend(mlc::String("Color"), mlc::String("Display"), mlc::Array<std::shared_ptr<semantic_ir::SDecl>>{color_to_string}));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExtend: static_assert implements trait"), decl::gen_decl(extend_display_for_color, context), mlc::String("static_assert")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExtend: trait name in assert"), decl::gen_decl(extend_display_for_color, context), mlc::String("Display")));
std::shared_ptr<semantic_ir::SDecl> exported_fn = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExported(fn_decl(mlc::String("square"), mlc::Array<std::shared_ptr<ast::Param>>{iparam(mlc::String("value"))}, ti32(), make_bin(mlc::String("*"), make_ident(mlc::String("value")), make_ident(mlc::String("value"))))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExported: generates body"), decl::gen_decl(exported_fn, context), mlc::String("int square")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExported: contains multiplication"), decl::gen_decl(exported_fn, context), mlc::String("value * value")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto SDeclFn: produces prototype"), decl::gen_proto(add_decl, context), mlc::String("int add(int left, int right) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto SDeclType: empty string"), decl::gen_proto(point_type, context), mlc::String("")));
std::shared_ptr<semantic_ir::SDecl> main_decl = fn_decl(mlc::String("main"), {}, tunit(), make_unit_expr());
std::shared_ptr<semantic_ir::SDecl> other_decl = fn_decl(mlc::String("other"), {}, tunit(), make_unit_expr());
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: finds main"), decl::decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>>{main_decl, other_decl})));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: no main"), !decl::decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>>{other_decl})));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: empty list"), !decl::decls_have_main({})));
return results;
}

} // namespace test_decl_gen
