#include "test_visitor_pass_parity.hpp"

#include "test_runner.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "names.hpp"
#include "check_mutations.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"
#include "eval.hpp"
#include "context.hpp"
#include "registry.hpp"
#include "expr_visitor_cpp.hpp"
#include "print.hpp"
#include "ast_builders.hpp"

namespace test_visitor_pass_parity {

using namespace test_runner;
using namespace ast;
using namespace semantic_ir;
using namespace names;
using namespace check_mutations;
using namespace transform;
using namespace transform_stmts;
using namespace eval;
using namespace context;
using namespace registry;
using namespace expr_visitor_cpp;
using namespace print;
using namespace ast_builders;
using namespace ast_tokens;

std::shared_ptr<ast::TypeExpr> unit_type_for_let() noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression(std::shared_ptr<ast::Expr> expression) noexcept;

int names_parity_count(std::shared_ptr<ast::Expr> parser_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

int names_visitor_count(std::shared_ptr<ast::Expr> parser_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

int mutations_parity_count(std::shared_ptr<ast::Expr> parser_body, mlc::Array<mlc::String> mutable_locals) noexcept;

int mutations_visitor_count(std::shared_ptr<ast::Expr> parser_body, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::String codegen_visitor_parity_string(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::Array<test_runner::TestResult> visitor_pass_parity_tests() noexcept;

std::shared_ptr<ast::TypeExpr> unit_type_for_let() noexcept{return std::make_shared<ast::TypeExpr>((ast::TyUnit{}));}

std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression(std::shared_ptr<ast::Expr> expression) noexcept{return transform::transform_expr(expression, transform::empty_transform_context(), [](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); });}

int names_parity_count(std::shared_ptr<ast::Expr> parser_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return names::check_names_expr(parser_expression, locals, globals).size();}

int names_visitor_count(std::shared_ptr<ast::Expr> parser_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return names::check_names_semantic_expression(transform_parser_expression(parser_expression), locals, globals).size();}

int mutations_parity_count(std::shared_ptr<ast::Expr> parser_body, mlc::Array<mlc::String> mutable_locals) noexcept{return check_mutations::check_fn_body_mutations({}, parser_body).size();}

int mutations_visitor_count(std::shared_ptr<ast::Expr> parser_body, mlc::Array<mlc::String> mutable_locals) noexcept{return check_mutations::check_mutation_semantic_expression(transform_parser_expression(parser_body), mutable_locals).size();}

mlc::String codegen_visitor_parity_string(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return print::print_expr(expr_visitor_cpp::eval_expr_cpp(expression, context, gen_stmts_str));}

mlc::Array<test_runner::TestResult> visitor_pass_parity_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
mlc::Array<mlc::String> empty_locals = {};
mlc::HashMap<mlc::String, bool> empty_globals = mlc::HashMap<mlc::String, bool>();
context::CodegenContext codegen_context = context::create_codegen_context(ast_builders::empty_program());
auto registry_integer_type = std::make_shared<registry::Type>((registry::TI32{}));
auto registry_unknown_type = std::make_shared<registry::Type>((registry::TUnknown{}));
results.push_back(test_runner::assert_eq_int(mlc::String("names parity: literal"), names_visitor_count(ast_builders::integer_expr(1), empty_locals, empty_globals), names_parity_count(ast_builders::integer_expr(1), empty_locals, empty_globals)));
results.push_back(test_runner::assert_eq_int(mlc::String("names parity: unknown ident"), names_visitor_count(ast_builders::identifier_expr(mlc::String("missing_name")), empty_locals, empty_globals), names_parity_count(ast_builders::identifier_expr(mlc::String("missing_name")), empty_locals, empty_globals)));
std::shared_ptr<ast::Expr> binary_parser = ast_builders::binary_expr(mlc::String("+"), ast_builders::identifier_expr(mlc::String("left_name")), ast_builders::identifier_expr(mlc::String("right_name")));
results.push_back(test_runner::assert_eq_int(mlc::String("names parity: binary unknown operands"), names_visitor_count(binary_parser, empty_locals, empty_globals), names_parity_count(binary_parser, empty_locals, empty_globals)));
std::shared_ptr<ast::Expr> immutable_assign_body = ast_builders::block_expr(mlc::Array<std::shared_ptr<ast::Stmt>>{ast_builders::let_statement(mlc::String("counter"), unit_type_for_let(), ast_builders::integer_expr(1))}, ast_builders::binary_expr(mlc::String("="), ast_builders::identifier_expr(mlc::String("counter")), ast_builders::integer_expr(2)));
results.push_back(test_runner::assert_eq_int(mlc::String("mutations parity: assign immutable"), mutations_visitor_count(immutable_assign_body, {}), mutations_parity_count(immutable_assign_body, {})));
std::shared_ptr<ast::Expr> mutable_assign_body = ast_builders::block_expr(mlc::Array<std::shared_ptr<ast::Stmt>>{ast_builders::let_mut_statement(mlc::String("counter"), unit_type_for_let(), ast_builders::integer_expr(1))}, ast_builders::binary_expr(mlc::String("="), ast_builders::identifier_expr(mlc::String("counter")), ast_builders::integer_expr(2)));
results.push_back(test_runner::assert_eq_int(mlc::String("mutations parity: assign let mut"), mutations_visitor_count(mutable_assign_body, {}), mutations_parity_count(mutable_assign_body, {})));
std::shared_ptr<semantic_ir::SemanticExpression> semantic_integer = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(42, registry_integer_type, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("codegen parity: integer literal"), eval::gen_expr(semantic_integer, codegen_context), codegen_visitor_parity_string(semantic_integer, codegen_context)));
std::shared_ptr<semantic_ir::SemanticExpression> semantic_binary = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(1, registry_integer_type, ast::span_unknown())), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(2, registry_integer_type, ast::span_unknown())), registry_integer_type, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("codegen parity: binary +"), eval::gen_expr(semantic_binary, codegen_context), codegen_visitor_parity_string(semantic_binary, codegen_context)));
std::shared_ptr<semantic_ir::SemanticExpression> semantic_ident = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mlc::String("value_name"), registry_unknown_type, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("codegen parity: ident"), eval::gen_expr(semantic_ident, codegen_context), codegen_visitor_parity_string(semantic_ident, codegen_context)));
return results;
}

} // namespace test_visitor_pass_parity
