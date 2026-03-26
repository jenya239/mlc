#include "test_decl_gen.hpp"

#include "test_runner.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "ast_builders.hpp"
#include "codegen_test_helpers.hpp"

namespace test_decl_gen {

using namespace test_runner;
using namespace ast;
using namespace context;
using namespace decl;
using namespace ast_builders;
using namespace codegen_test_helpers;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept;

mlc::Array<test_runner::TestResult> decl_gen_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
context::CodegenContext context = codegen_test_helpers::empty_codegen_context();
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: zero params"), decl::gen_fn_proto(mlc::String("compute"), {}, {}, {}, ast_builders::unit_type(), context), mlc::String("void compute() noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: single i32 param"), decl::gen_fn_proto(mlc::String("double_it"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{ast_builders::value_param(mlc::String("value"), ast_builders::integer_type())}, ast_builders::integer_type(), context), mlc::String("int double_it(int value) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: two params"), decl::gen_fn_proto(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{ast_builders::value_param(mlc::String("left"), ast_builders::integer_type()), ast_builders::value_param(mlc::String("right"), ast_builders::integer_type())}, ast_builders::integer_type(), context), mlc::String("int add(int left, int right) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_proto: string return type"), decl::gen_fn_proto(mlc::String("greet"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{ast_builders::value_param(mlc::String("name"), ast_builders::string_type())}, ast_builders::string_type(), context), mlc::String("mlc::String greet(mlc::String name) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: identity function"), decl::gen_fn_decl(mlc::String("identity"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{ast_builders::value_param(mlc::String("value"), ast_builders::integer_type())}, ast_builders::integer_type(), ast_builders::identifier_expr(mlc::String("value")), context), mlc::String("int identity(int value) noexcept{\nreturn value;\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: addition"), decl::gen_fn_decl(mlc::String("add"), {}, {}, mlc::Array<std::shared_ptr<ast::Param>>{ast_builders::value_param(mlc::String("left"), ast_builders::integer_type()), ast_builders::value_param(mlc::String("right"), ast_builders::integer_type())}, ast_builders::integer_type(), ast_builders::binary_expr(mlc::String("+"), ast_builders::identifier_expr(mlc::String("left")), ast_builders::identifier_expr(mlc::String("right"))), context), mlc::String("int add(int left, int right) noexcept{\nreturn (left + right);\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_fn_decl: unit return"), decl::gen_fn_decl(mlc::String("nothing"), {}, {}, {}, ast_builders::unit_type(), ast_builders::unit_expr(), context), mlc::String("void nothing() noexcept{\nreturn /* unit */;\n}\n")));
std::shared_ptr<ast::Decl> add_decl = ast_builders::function_decl(mlc::String("add"), mlc::Array<std::shared_ptr<ast::Param>>{ast_builders::value_param(mlc::String("left"), ast_builders::integer_type()), ast_builders::value_param(mlc::String("right"), ast_builders::integer_type())}, ast_builders::integer_type(), ast_builders::binary_expr(mlc::String("+"), ast_builders::identifier_expr(mlc::String("left")), ast_builders::identifier_expr(mlc::String("right"))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains return type"), decl::gen_decl(add_decl, context), mlc::String("int add")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclFn: contains body"), decl::gen_decl(add_decl, context), mlc::String("return (left + right)")));
std::shared_ptr<ast::Decl> point_type = ast_builders::type_decl(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::record_variant(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{ast_builders::field_definition(mlc::String("x"), ast_builders::integer_type()), ast_builders::field_definition(mlc::String("y"), ast_builders::integer_type())})});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: record struct"), decl::gen_decl(point_type, context), mlc::String("Point")));
std::shared_ptr<ast::Decl> color_type = ast_builders::type_decl(mlc::String("Color"), mlc::Array<std::shared_ptr<ast::TypeVariant>>{ast_builders::unit_variant(mlc::String("Red")), ast_builders::unit_variant(mlc::String("Green")), ast_builders::unit_variant(mlc::String("Blue"))});
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclType: unit variants"), decl::gen_decl(color_type, context), mlc::String("Red")));
std::shared_ptr<ast::Decl> exported_fn = ast_builders::exported_decl(ast_builders::function_decl(mlc::String("square"), mlc::Array<std::shared_ptr<ast::Param>>{ast_builders::value_param(mlc::String("value"), ast_builders::integer_type())}, ast_builders::integer_type(), ast_builders::binary_expr(mlc::String("*"), ast_builders::identifier_expr(mlc::String("value")), ast_builders::identifier_expr(mlc::String("value")))));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclExported: generates body"), decl::gen_decl(exported_fn, context), mlc::String("int square")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_decl DeclExported: contains multiplication"), decl::gen_decl(exported_fn, context), mlc::String("value * value")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto DeclFn: produces prototype"), decl::gen_proto(add_decl, context), mlc::String("int add(int left, int right) noexcept;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_proto DeclType: empty string"), decl::gen_proto(point_type, context), mlc::String("")));
std::shared_ptr<ast::Decl> main_decl = ast_builders::function_decl(mlc::String("main"), {}, ast_builders::unit_type(), ast_builders::unit_expr());
std::shared_ptr<ast::Decl> other_decl = ast_builders::function_decl(mlc::String("other"), {}, ast_builders::unit_type(), ast_builders::unit_expr());
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: finds main"), decl::decls_have_main(mlc::Array<std::shared_ptr<ast::Decl>>{main_decl, other_decl})));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: no main"), !decl::decls_have_main(mlc::Array<std::shared_ptr<ast::Decl>>{other_decl})));
results.push_back(test_runner::assert_true(mlc::String("decls_have_main: empty list"), !decl::decls_have_main({})));
return results;
}

} // namespace test_decl_gen
