#include "ast_builders.hpp"

#include "ast.hpp"

namespace ast_builders {

using namespace ast;
using namespace ast_tokens;

ast::Program empty_program() noexcept;

std::shared_ptr<ast::TypeExpr> integer_type() noexcept;

std::shared_ptr<ast::TypeExpr> string_type() noexcept;

std::shared_ptr<ast::TypeExpr> boolean_type() noexcept;

std::shared_ptr<ast::TypeExpr> unit_type() noexcept;

std::shared_ptr<ast::TypeExpr> named_type(mlc::String type_name) noexcept;

std::shared_ptr<ast::TypeExpr> array_type(std::shared_ptr<ast::TypeExpr> element_type) noexcept;

std::shared_ptr<ast::TypeExpr> shared_type(std::shared_ptr<ast::TypeExpr> inner_type) noexcept;

std::shared_ptr<ast::TypeExpr> generic_type(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept;

std::shared_ptr<ast::TypeExpr> function_type(mlc::Array<std::shared_ptr<ast::TypeExpr>> parameter_types, std::shared_ptr<ast::TypeExpr> return_type) noexcept;

std::shared_ptr<ast::Expr> integer_expr(int value) noexcept;

std::shared_ptr<ast::Expr> string_expr(mlc::String value) noexcept;

std::shared_ptr<ast::Expr> boolean_expr(bool value) noexcept;

std::shared_ptr<ast::Expr> unit_expr() noexcept;

std::shared_ptr<ast::Expr> identifier_expr(mlc::String name) noexcept;

std::shared_ptr<ast::Expr> binary_expr(mlc::String operation, std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;

std::shared_ptr<ast::Expr> unary_expr(mlc::String operation, std::shared_ptr<ast::Expr> inner_expr) noexcept;

std::shared_ptr<ast::Expr> call_expr(mlc::String function_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

std::shared_ptr<ast::Expr> method_call_expr(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

std::shared_ptr<ast::Expr> field_expr(std::shared_ptr<ast::Expr> object, mlc::String field_name) noexcept;

std::shared_ptr<ast::Expr> index_expr(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index) noexcept;

std::shared_ptr<ast::Expr> if_expr(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expr, std::shared_ptr<ast::Expr> else_expr) noexcept;

std::shared_ptr<ast::Expr> block_expr(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expr) noexcept;

std::shared_ptr<ast::Expr> while_expr(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;

std::shared_ptr<ast::Expr> array_expr(mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;

std::shared_ptr<ast::Expr> record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;

std::shared_ptr<ast::Expr> question_expr(std::shared_ptr<ast::Expr> inner_expr) noexcept;

std::shared_ptr<ast::Expr> lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<ast::Expr> body_expr) noexcept;

std::shared_ptr<ast::FieldVal> field_value(mlc::String field_name, std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Pat> wildcard_pattern() noexcept;

std::shared_ptr<ast::Pat> integer_pattern(int value) noexcept;

std::shared_ptr<ast::Pat> identifier_pattern(mlc::String name) noexcept;

std::shared_ptr<ast::Pat> ctor_pattern(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept;

std::shared_ptr<ast::MatchArm> match_arm(std::shared_ptr<ast::Pat> pattern, std::shared_ptr<ast::Expr> body_expr) noexcept;

std::shared_ptr<ast::Stmt> let_statement(mlc::String binding_name, std::shared_ptr<ast::TypeExpr> type_expr, std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Stmt> let_mut_statement(mlc::String binding_name, std::shared_ptr<ast::TypeExpr> type_expr, std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Stmt> expression_statement(std::shared_ptr<ast::Expr> expr) noexcept;

std::shared_ptr<ast::Stmt> return_statement(std::shared_ptr<ast::Expr> value_expr) noexcept;

std::shared_ptr<ast::Param> value_param(mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> parameter_type) noexcept;

std::shared_ptr<ast::Decl> function_decl(mlc::String function_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body_expr) noexcept;

std::shared_ptr<ast::Decl> type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

std::shared_ptr<ast::Decl> exported_decl(std::shared_ptr<ast::Decl> inner_decl) noexcept;

std::shared_ptr<ast::TypeVariant> record_variant(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

std::shared_ptr<ast::TypeVariant> tuple_variant(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept;

std::shared_ptr<ast::TypeVariant> unit_variant(mlc::String variant_name) noexcept;

std::shared_ptr<ast::FieldDef> field_definition(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;

ast::Program empty_program() noexcept{return ast::Program{{}};}

std::shared_ptr<ast::TypeExpr> integer_type() noexcept{return std::make_shared<ast::TypeExpr>((ast::TyI32{}));}

std::shared_ptr<ast::TypeExpr> string_type() noexcept{return std::make_shared<ast::TypeExpr>((ast::TyString{}));}

std::shared_ptr<ast::TypeExpr> boolean_type() noexcept{return std::make_shared<ast::TypeExpr>((ast::TyBool{}));}

std::shared_ptr<ast::TypeExpr> unit_type() noexcept{return std::make_shared<ast::TypeExpr>((ast::TyUnit{}));}

std::shared_ptr<ast::TypeExpr> named_type(mlc::String type_name) noexcept{return std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name));}

std::shared_ptr<ast::TypeExpr> array_type(std::shared_ptr<ast::TypeExpr> element_type) noexcept{return std::make_shared<ast::TypeExpr>(ast::TyArray(element_type));}

std::shared_ptr<ast::TypeExpr> shared_type(std::shared_ptr<ast::TypeExpr> inner_type) noexcept{return std::make_shared<ast::TypeExpr>(ast::TyShared(inner_type));}

std::shared_ptr<ast::TypeExpr> generic_type(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept{return std::make_shared<ast::TypeExpr>(ast::TyGeneric(type_name, type_arguments));}

std::shared_ptr<ast::TypeExpr> function_type(mlc::Array<std::shared_ptr<ast::TypeExpr>> parameter_types, std::shared_ptr<ast::TypeExpr> return_type) noexcept{return std::make_shared<ast::TypeExpr>(ast::TyFn(parameter_types, return_type));}

std::shared_ptr<ast::Expr> integer_expr(int value) noexcept{return std::make_shared<ast::Expr>(ast::ExprInt(value, ast::span_unknown()));}

std::shared_ptr<ast::Expr> string_expr(mlc::String value) noexcept{return std::make_shared<ast::Expr>(ast::ExprStr(value, ast::span_unknown()));}

std::shared_ptr<ast::Expr> boolean_expr(bool value) noexcept{return std::make_shared<ast::Expr>(ast::ExprBool(value, ast::span_unknown()));}

std::shared_ptr<ast::Expr> unit_expr() noexcept{return std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()));}

std::shared_ptr<ast::Expr> identifier_expr(mlc::String name) noexcept{return std::make_shared<ast::Expr>(ast::ExprIdent(name, ast::span_unknown()));}

std::shared_ptr<ast::Expr> binary_expr(mlc::String operation, std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept{return std::make_shared<ast::Expr>(ast::ExprBin(operation, left_expr, right_expr, ast::span_unknown()));}

std::shared_ptr<ast::Expr> unary_expr(mlc::String operation, std::shared_ptr<ast::Expr> inner_expr) noexcept{return std::make_shared<ast::Expr>(ast::ExprUn(operation, inner_expr, ast::span_unknown()));}

std::shared_ptr<ast::Expr> call_expr(mlc::String function_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{return std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(function_name, ast::span_unknown())), arguments, ast::span_unknown()));}

std::shared_ptr<ast::Expr> method_call_expr(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{return std::make_shared<ast::Expr>(ast::ExprMethod(object, method_name, arguments, ast::span_unknown()));}

std::shared_ptr<ast::Expr> field_expr(std::shared_ptr<ast::Expr> object, mlc::String field_name) noexcept{return std::make_shared<ast::Expr>(ast::ExprField(object, field_name, ast::span_unknown()));}

std::shared_ptr<ast::Expr> index_expr(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index) noexcept{return std::make_shared<ast::Expr>(ast::ExprIndex(object, index, ast::span_unknown()));}

std::shared_ptr<ast::Expr> if_expr(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expr, std::shared_ptr<ast::Expr> else_expr) noexcept{return std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_expr, ast::span_unknown()));}

std::shared_ptr<ast::Expr> block_expr(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expr) noexcept{return std::make_shared<ast::Expr>(ast::ExprBlock(statements, result_expr, ast::span_unknown()));}

std::shared_ptr<ast::Expr> while_expr(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{return std::make_shared<ast::Expr>(ast::ExprWhile(condition, statements, ast::span_unknown()));}

std::shared_ptr<ast::Expr> array_expr(mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept{return std::make_shared<ast::Expr>(ast::ExprArray(elements, ast::span_unknown()));}

std::shared_ptr<ast::Expr> record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{return std::make_shared<ast::Expr>(ast::ExprRecord(type_name, field_values, ast::span_unknown()));}

std::shared_ptr<ast::Expr> question_expr(std::shared_ptr<ast::Expr> inner_expr) noexcept{return std::make_shared<ast::Expr>(ast::ExprQuestion(inner_expr, ast::span_unknown()));}

std::shared_ptr<ast::Expr> lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<ast::Expr> body_expr) noexcept{return std::make_shared<ast::Expr>(ast::ExprLambda(parameters, body_expr, ast::span_unknown()));}

std::shared_ptr<ast::FieldVal> field_value(mlc::String field_name, std::shared_ptr<ast::Expr> value_expr) noexcept{return std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, value_expr});}

std::shared_ptr<ast::Pat> wildcard_pattern() noexcept{return std::make_shared<ast::Pat>(ast::PatWild(ast::span_unknown()));}

std::shared_ptr<ast::Pat> integer_pattern(int value) noexcept{return std::make_shared<ast::Pat>(ast::PatInt(value, ast::span_unknown()));}

std::shared_ptr<ast::Pat> identifier_pattern(mlc::String name) noexcept{return std::make_shared<ast::Pat>(ast::PatIdent(name, ast::span_unknown()));}

std::shared_ptr<ast::Pat> ctor_pattern(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns) noexcept{return std::make_shared<ast::Pat>(ast::PatCtor(ctor_name, sub_patterns, ast::span_unknown()));}

std::shared_ptr<ast::MatchArm> match_arm(std::shared_ptr<ast::Pat> pattern, std::shared_ptr<ast::Expr> body_expr) noexcept{return std::make_shared<ast::MatchArm>(ast::MatchArm{pattern, body_expr});}

std::shared_ptr<ast::Stmt> let_statement(mlc::String binding_name, std::shared_ptr<ast::TypeExpr> type_expr, std::shared_ptr<ast::Expr> value_expr) noexcept{return std::make_shared<ast::Stmt>(ast::StmtLet(binding_name, false, type_expr, value_expr, ast::span_unknown()));}

std::shared_ptr<ast::Stmt> let_mut_statement(mlc::String binding_name, std::shared_ptr<ast::TypeExpr> type_expr, std::shared_ptr<ast::Expr> value_expr) noexcept{return std::make_shared<ast::Stmt>(ast::StmtLet(binding_name, true, type_expr, value_expr, ast::span_unknown()));}

std::shared_ptr<ast::Stmt> expression_statement(std::shared_ptr<ast::Expr> expr) noexcept{return std::make_shared<ast::Stmt>(ast::StmtExpr(expr, ast::span_unknown()));}

std::shared_ptr<ast::Stmt> return_statement(std::shared_ptr<ast::Expr> value_expr) noexcept{return std::make_shared<ast::Stmt>(ast::StmtReturn(value_expr, ast::span_unknown()));}

std::shared_ptr<ast::Param> value_param(mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> parameter_type) noexcept{return std::make_shared<ast::Param>(ast::Param{parameter_name, false, parameter_type, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))});}

std::shared_ptr<ast::Decl> function_decl(mlc::String function_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body_expr) noexcept{return std::make_shared<ast::Decl>(ast::DeclFn(function_name, {}, {}, parameters, return_type, body_expr));}

std::shared_ptr<ast::Decl> type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return std::make_shared<ast::Decl>(ast::DeclType(type_name, {}, variants, {}));}

std::shared_ptr<ast::Decl> exported_decl(std::shared_ptr<ast::Decl> inner_decl) noexcept{return std::make_shared<ast::Decl>(ast::DeclExported(inner_decl));}

std::shared_ptr<ast::TypeVariant> record_variant(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return std::make_shared<ast::TypeVariant>(ast::VarRecord(variant_name, field_definitions));}

std::shared_ptr<ast::TypeVariant> tuple_variant(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept{return std::make_shared<ast::TypeVariant>(ast::VarTuple(variant_name, field_types));}

std::shared_ptr<ast::TypeVariant> unit_variant(mlc::String variant_name) noexcept{return std::make_shared<ast::TypeVariant>(ast::VarUnit(variant_name));}

std::shared_ptr<ast::FieldDef> field_definition(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{return std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, field_type});}

} // namespace ast_builders
