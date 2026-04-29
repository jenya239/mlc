#define main mlc_user_main
#include "test_decl_gen.hpp"

namespace test_decl_gen {

std::shared_ptr<registry::Type> ti32() noexcept{
return std::make_shared<registry::Type>(registry::TI32{});
}
std::shared_ptr<registry::Type> tstr() noexcept{
return std::make_shared<registry::Type>(registry::TString{});
}
std::shared_ptr<registry::Type> tunit() noexcept{
return std::make_shared<registry::Type>(registry::TUnit{});
}
std::shared_ptr<registry::Type> unk() noexcept{
return std::make_shared<registry::Type>(registry::TUnknown{});
}
std::shared_ptr<semantic_ir::SExpr> make_ident(mlc::String name) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent{name, unk(), ast::span_unknown()});
}
std::shared_ptr<semantic_ir::SExpr> make_bin(mlc::String op, std::shared_ptr<semantic_ir::SExpr> left, std::shared_ptr<semantic_ir::SExpr> right) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin{op, left, right, unk(), ast::span_unknown()});
}
std::shared_ptr<semantic_ir::SExpr> make_unit_expr() noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{tunit(), ast::span_unknown()});
}
std::shared_ptr<ast::Param> iparam(mlc::String name) noexcept{
return std::make_shared<ast::Param>(ast::Param{name, false, std::make_shared<ast::TypeExpr>(ast::TyI32{}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})});
}
std::shared_ptr<semantic_ir::SDecl> fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body) noexcept{
return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclFn{name, {}, {}, params, return_type, body});
}
std::shared_ptr<semantic_ir::SExpr> make_str_expr(mlc::String text) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr{text, tstr(), ast::span_unknown()});
}
mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept{
auto results = mlc::Array<test_runner::TestResult>{};
auto context = codegen_test_helpers::empty_codegen_context();
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: zero params", 25), decl::gen_fn_proto(mlc::String("compute", 7), {}, {}, {}, tunit(), context), mlc::String("void compute() noexcept;\n", 25)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: single i32 param", 30), decl::gen_fn_proto(mlc::String("double_it", 9), {}, {}, mlc::Array{iparam(mlc::String("value", 5))}, ti32(), context), mlc::String("int double_it(int value) noexcept;\n", 35)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: two params", 24), decl::gen_fn_proto(mlc::String("add", 3), {}, {}, mlc::Array{iparam(mlc::String("left", 4)), iparam(mlc::String("right", 5))}, ti32(), context), mlc::String("int add(int left, int right) noexcept;\n", 39)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: string return type", 32), decl::gen_fn_proto(mlc::String("greet", 5), {}, {}, mlc::Array{std::make_shared<ast::Param>(ast::Param{mlc::String("name", 4), false, std::make_shared<ast::TypeExpr>(ast::TyString{}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})})}, tstr(), context), mlc::String("mlc::String greet(mlc::String name) noexcept;\n", 46)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: default param literal", 35), decl::gen_fn_proto(mlc::String("f", 1), {}, {}, mlc::Array{std::make_shared<ast::Param>(ast::Param{mlc::String("a", 1), false, std::make_shared<ast::TypeExpr>(ast::TyI32{}), true, std::make_shared<ast::Expr>(ast::ExprInt{2, ast::span_unknown()})})}, ti32(), context), mlc::String("int f(int a = 2) noexcept;\n", 27)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: body omits default value", 37), decl::gen_fn_decl(mlc::String("f", 1), {}, {}, mlc::Array{std::make_shared<ast::Param>(ast::Param{mlc::String("a", 1), false, std::make_shared<ast::TypeExpr>(ast::TyI32{}), true, std::make_shared<ast::Expr>(ast::ExprInt{2, ast::span_unknown()})})}, ti32(), make_ident(mlc::String("a", 1)), context), mlc::String("int f(int a) noexcept{\nreturn a;\n}\n", 35)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: identity function", 30), decl::gen_fn_decl(mlc::String("identity", 8), {}, {}, mlc::Array{iparam(mlc::String("value", 5))}, ti32(), make_ident(mlc::String("value", 5)), context), mlc::String("int identity(int value) noexcept{\nreturn value;\n}\n", 50)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: addition", 21), decl::gen_fn_decl(mlc::String("add", 3), {}, {}, mlc::Array{iparam(mlc::String("left", 4)), iparam(mlc::String("right", 5))}, ti32(), make_bin(mlc::String("+", 1), make_ident(mlc::String("left", 4)), make_ident(mlc::String("right", 5))), context), mlc::String("int add(int left, int right) noexcept{\nreturn (left + right);\n}\n", 64)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: unit return", 24), decl::gen_fn_decl(mlc::String("nothing", 7), {}, {}, {}, tunit(), make_unit_expr(), context), mlc::String("void nothing() noexcept{\nreturn /* unit */;\n}\n", 46)));
auto add_decl = fn_decl(mlc::String("add", 3), mlc::Array{iparam(mlc::String("left", 4)), iparam(mlc::String("right", 5))}, ti32(), make_bin(mlc::String("+", 1), make_ident(mlc::String("left", 4)), make_ident(mlc::String("right", 5))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains return type", 37), decl::gen_decl(add_decl, context), mlc::String("int add", 7)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains body", 30), decl::gen_decl(add_decl, context), mlc::String("return (left + right)", 21)));
auto point_type = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType{mlc::String("Point", 5), {}, mlc::Array{ast_builders::record_variant(mlc::String("Point", 5), mlc::Array{ast_builders::field_definition(mlc::String("x", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{})), ast_builders::field_definition(mlc::String("y", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{}))})}, {}});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: record struct", 32), decl::gen_decl(point_type, context), mlc::String("Point", 5)));
auto color_type = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType{mlc::String("Color", 5), {}, mlc::Array{ast_builders::unit_variant(mlc::String("Red", 3)), ast_builders::unit_variant(mlc::String("Green", 5)), ast_builders::unit_variant(mlc::String("Blue", 4))}, {}});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: unit variants", 32), decl::gen_decl(color_type, context), mlc::String("Red", 3)));
auto point_display = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType{mlc::String("Point", 5), {}, mlc::Array{ast_builders::record_variant(mlc::String("Point", 5), mlc::Array{ast_builders::field_definition(mlc::String("x", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{})), ast_builders::field_definition(mlc::String("y", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{}))})}, mlc::Array{mlc::String("Display", 7)}});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display record: generates to_string function", 51), decl::gen_decl(point_display, context), mlc::String("Point_to_string", 15)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display record: uses mlc::to_string for i32 field", 56), decl::gen_decl(point_display, context), mlc::String("mlc::to_string(self.x)", 22)));
auto point_eq = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType{mlc::String("Point", 5), {}, mlc::Array{ast_builders::record_variant(mlc::String("Point", 5), mlc::Array{ast_builders::field_definition(mlc::String("x", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{})), ast_builders::field_definition(mlc::String("y", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{}))})}, mlc::Array{mlc::String("Eq", 2)}});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Eq record: generates operator==", 38), decl::gen_decl(point_eq, context), mlc::String("operator==", 10)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Eq record: compares fields", 33), decl::gen_decl(point_eq, context), mlc::String("a.x == b.x", 10)));
auto color_display = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType{mlc::String("Color", 5), {}, mlc::Array{ast_builders::unit_variant(mlc::String("Red", 3)), ast_builders::unit_variant(mlc::String("Green", 5)), ast_builders::unit_variant(mlc::String("Blue", 4))}, mlc::Array{mlc::String("Display", 7)}});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display sum type: generates to_string", 44), decl::gen_decl(color_display, context), mlc::String("Color_to_string", 15)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("derive Display sum type: handles Red variant", 44), decl::gen_decl(color_display, context), mlc::String("\"Red\"", 5)));
auto display_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed{mlc::String("Display", 7)}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})});
auto display_method = fn_decl(mlc::String("Display_to_string", 17), mlc::Array{display_self}, tstr(), make_unit_expr());
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_decl: concept keyword", 31), decl_extend::gen_trait_decl(context, mlc::String("Display", 7), {}, mlc::Array{display_method}), mlc::String("concept", 7)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_trait_decl: trait identifier", 32), decl_extend::gen_trait_decl(context, mlc::String("Display", 7), {}, mlc::Array{display_method}), mlc::String("Display", 7)));
auto color_self = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed{mlc::String("Color", 5)}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})});
auto color_to_string = fn_decl(mlc::String("Color_to_string", 15), mlc::Array{color_self}, tstr(), make_str_expr(mlc::String("red", 3)));
auto extend_display_for_color = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExtend{mlc::String("Color", 5), mlc::String("Display", 7), mlc::Array{color_to_string}});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExtend: static_assert implements trait", 52), decl::gen_decl(extend_display_for_color, context), mlc::String("static_assert", 13)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExtend: trait name in assert", 42), decl::gen_decl(extend_display_for_color, context), mlc::String("Display", 7)));
auto exported_fn = std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExported{fn_decl(mlc::String("square", 6), mlc::Array{iparam(mlc::String("value", 5))}, ti32(), make_bin(mlc::String("*", 1), make_ident(mlc::String("value", 5)), make_ident(mlc::String("value", 5))))});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExported: generates body", 38), decl::gen_decl(exported_fn, context), mlc::String("int square", 10)));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl SDeclExported: contains multiplication", 47), decl::gen_decl(exported_fn, context), mlc::String("value * value", 13)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto SDeclFn: produces prototype", 37), decl::gen_proto(add_decl, context), mlc::String("int add(int left, int right) noexcept;\n", 39)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto SDeclType: empty string", 33), decl::gen_proto(point_type, context), mlc::String("", 0)));
auto main_decl = fn_decl(mlc::String("main", 4), {}, tunit(), make_unit_expr());
auto other_decl = fn_decl(mlc::String("other", 5), {}, tunit(), make_unit_expr());
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: finds main", 27), decl::decls_have_main(mlc::Array{main_decl, other_decl})));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: no main", 24), (!decl::decls_have_main(mlc::Array{other_decl}))));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: empty list", 27), (!decl::decls_have_main({}))));
return results;
}

} // namespace test_decl_gen
