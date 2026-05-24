#include "expr_visitor_string.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "expr_visitor.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "mut_actual_argument.hpp"
#include "method_gen.hpp"
#include "match_gen.hpp"
#include "record_gen.hpp"
#include "type_gen.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace expr_visitor_string {

using namespace registry;
using namespace semantic_ir;
using namespace registry;
using namespace semantic_type_structure;
using namespace expr_visitor;
using namespace context;
using namespace literals;
using namespace identifiers;
using namespace cpp_naming;
using namespace expression_support;
using namespace mut_actual_argument;
using namespace method_gen;
using namespace match_gen;
using namespace record_gen;
using namespace type_gen;
using namespace statement_context;
using namespace expr;

struct StringExprVisitor {context::CodegenContext context;std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts;std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression;};

mlc::String evaluate_via_string_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> _gen_stmts) noexcept;

mlc::String ignored_gen_stmts(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> _statements, context::CodegenContext _context) noexcept;

mlc::String gen_binary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_unary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_call_via_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_method_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_if_else_code(std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SExpr> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_if_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_block_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_match_via_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_extern_via_visitor(std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String argument_list_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_field_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_index_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_while_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_for_via_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_record_via_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_record_update_via_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_array_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_tuple_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_question_via_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String cpp_lambda_header_from_semantic_function_type(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda, context::CodegenContext context) noexcept;

mlc::String gen_lambda_via_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_with_via_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String StringExprVisitor_visit_int(expr_visitor_string::StringExprVisitor self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_str(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_bool(expr_visitor_string::StringExprVisitor self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_unit(expr_visitor_string::StringExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_float(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_i64(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_u8(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_usize(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_char(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_extern(expr_visitor_string::StringExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_ident(expr_visitor_string::StringExprVisitor self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_bin(expr_visitor_string::StringExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_un(expr_visitor_string::StringExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_call(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_method(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_field(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_index(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_if(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

mlc::String StringExprVisitor_visit_block(expr_visitor_string::StringExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_while(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_for(expr_visitor_string::StringExprVisitor self, mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_match(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept;

mlc::String StringExprVisitor_visit_record(expr_visitor_string::StringExprVisitor self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept;

mlc::String StringExprVisitor_visit_record_update(expr_visitor_string::StringExprVisitor self, mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_array(expr_visitor_string::StringExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;

mlc::String StringExprVisitor_visit_tuple(expr_visitor_string::StringExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_question(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_lambda(expr_visitor_string::StringExprVisitor self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SExpr> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

mlc::String StringExprVisitor_visit_with(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String StringExprVisitor_visit_unsupported(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> _expression) noexcept;

mlc::String eval_expr_with_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_expr_via_string_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

mlc::String evaluate_via_string_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> _gen_stmts) noexcept{return gen_expr_via_string_visitor(expression, context);}

mlc::String ignored_gen_stmts(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> _statements, context::CodegenContext _context) noexcept{return mlc::String("");}

mlc::String gen_binary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String method = semantic_type_structure::operator_method_for(operation);
std::shared_ptr<registry::Type> left_type = semantic_ir::sexpr_type(left_expression);
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*left_type))) { auto _v_tnamed = std::get<registry::TNamed>((*left_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return method != mlc::String("") && type_name != mlc::String("") ? type_name + mlc::String("_") + method + mlc::String("(") + evaluate_expression(left_expression, context, gen_stmts) + mlc::String(", ") + evaluate_expression(right_expression, context, gen_stmts) + mlc::String(")") : expr::parenthesized_binary(evaluate_expression(left_expression, context, gen_stmts), operation, evaluate_expression(right_expression, context, gen_stmts));
}

mlc::String gen_unary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::parenthesized_unary(operation, evaluate_expression(inner_expression, context, gen_stmts));}

mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprIdent>((*function_expression)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*function_expression)._); auto [name, _w0, _w1] = _v_sexprident; return context::qualify_function_callee(cpp_naming::map_builtin(name), context); } return evaluate_expression(function_expression, context, gen_stmts); }();}

mlc::String gen_call_via_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String function_code = gen_call_function_code(function_expression, context, gen_stmts, evaluate_expression);
mut_actual_argument::MutActualArgumentMaterializationOutcome materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_maybe_holding_mut_references(arguments, call_parameter_mutability_flags, context, gen_stmts, evaluate_expression);
mlc::String argument_list = materialization_outcome.materialized_argument_fragments.join(mlc::String(", "));
mlc::String invoke_expression = expression_support::is_constructor_call(function_expression) ? expr::constructor_call_braces(function_code, argument_list) : expr::function_call_parentheses(function_code, argument_list);
return mut_actual_argument::wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(materialization_outcome.prelude_block, invoke_expression);
}

mlc::String gen_method_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return method_gen::gen_method_expr(object, method_name, arguments, receiver_and_parameter_mutability_flags, context, gen_stmts, evaluate_expression);}

mlc::String gen_if_else_code(std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SExpr> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprArray>((*else_expression)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*else_expression)._); auto [elements, _w0, array_span] = _v_sexprarray; return elements.size() > 0 ? evaluate_expression(else_expression, context, gen_stmts) : [&]() -> mlc::String { 
  std::shared_ptr<registry::Type> inner_preferred = semantic_type_structure::array_element_type_from_array_type(if_semantic_type);
  std::shared_ptr<registry::Type> inner_from_then_branch = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(then_expression));
  std::shared_ptr<registry::Type> inner_for_empty_array = !semantic_type_structure::type_is_unknown(inner_preferred) ? inner_preferred : inner_from_then_branch;
  return semantic_type_structure::type_is_unknown(inner_for_empty_array) ? evaluate_expression(else_expression, context, gen_stmts) : evaluate_expression(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(elements, std::make_shared<registry::Type>(registry::TArray(inner_for_empty_array)), array_span)), context, gen_stmts);
 }(); } return evaluate_expression(else_expression, context, gen_stmts); }();}

mlc::String gen_if_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String condition_code = evaluate_expression(condition, context, gen_stmts);
mlc::String then_code = evaluate_expression(then_expression, context, gen_stmts);
mlc::String else_code = gen_if_else_code(else_expression, if_semantic_type, then_expression, context, gen_stmts, evaluate_expression);
return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*if_semantic_type))) { auto _v_tnamed = std::get<registry::TNamed>((*if_semantic_type)); auto [type_name] = _v_tnamed; return [&]() -> mlc::String { 
  mlc::String type_cpp = context::CodegenContext_resolve(context, type_name);
  return type_cpp == mlc::String("auto") ? expr::ternary_conditional(condition_code, then_code, else_code) : mlc::String("[&]() -> ") + type_cpp + mlc::String(" {\nif (") + condition_code + mlc::String(") {\nreturn ") + then_code + mlc::String(";\n} else {\nreturn ") + else_code + mlc::String(";\n}\n}()");
 }(); } return expr::ternary_conditional(condition_code, then_code, else_code); }();
}

mlc::String gen_block_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return statements.size() == 0 ? evaluate_expression(result_expression, context, gen_stmts) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts(statements, context) + expr::block_immediate_function_return_fragment(evaluate_expression(result_expression, statement_context::stmts_final_ctx(statements, context), gen_stmts));
  return expr::block_as_immediate_invoked_function_expression(body);
 }();}

mlc::String gen_match_via_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return match_gen::gen_match(subject, arms, match_semantic_type, context, gen_stmts, evaluate_expression);}

mlc::String gen_extern_via_visitor(std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_extern_placeholder();}

mlc::String argument_list_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expressions.map([evaluate_expression, context, gen_stmts](std::shared_ptr<semantic_ir::SExpr> expression) mutable { return evaluate_expression(expression, context, gen_stmts); }).join(mlc::String(", "));}

mlc::String gen_field_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String object_code = evaluate_expression(object, context, gen_stmts);
mlc::String operator_ = expression_support::field_access_operator(object, context);
return expr::field_access(object_code, operator_, cpp_naming::cpp_safe(field_name));
}

mlc::String gen_index_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::index_subscript(evaluate_expression(object, context, gen_stmts), evaluate_expression(index_expression, context, gen_stmts));}

mlc::String gen_while_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::while_loop_immediate_invoked_function_expression(evaluate_expression(condition, context, gen_stmts), gen_stmts(statements, context));}

mlc::String gen_for_via_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::for_loop_immediate_invoked_function_expression(cpp_naming::cpp_safe(variable), evaluate_expression(iterator, context, gen_stmts), gen_stmts(statements, context));}

mlc::String gen_record_via_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return record_gen::gen_record_expr(type_name, field_values, expression_type, context, gen_stmts, evaluate_expression);}

mlc::String gen_record_update_via_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return record_gen::gen_record_update_expr(type_name, base_expression, overrides, context, gen_stmts, evaluate_expression);}

mlc::String gen_array_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*element_container_semantic_type))) { auto _v_tarray = std::get<registry::TArray>((*element_container_semantic_type)); auto [inner_type] = _v_tarray; return [&]() -> mlc::String { 
  mlc::String element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
  return elements.size() == 0 ? expr::typed_array_empty_or_untyped_empty(element_cpp) : element_cpp == mlc::String("auto") ? expr::array_literal(argument_list_via_visitor(elements, context, gen_stmts, evaluate_expression)) : expr::typed_array_braced_initializer(element_cpp, argument_list_via_visitor(elements, context, gen_stmts, evaluate_expression));
 }(); } return expr::array_literal(argument_list_via_visitor(elements, context, gen_stmts, evaluate_expression)); }();}

mlc::String gen_tuple_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return mlc::String("std::make_tuple(") + elements.map([evaluate_expression, context, gen_stmts](std::shared_ptr<semantic_ir::SExpr> element) mutable { return evaluate_expression(element, context, gen_stmts); }).join(mlc::String(", ")) + mlc::String(")");}

mlc::String gen_question_via_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::question_try_result(evaluate_expression(inner_expression, context, gen_stmts));}

mlc::String cpp_lambda_header_from_semantic_function_type(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda, context::CodegenContext context) noexcept{
mlc::String capture_clause = parameter_binding_names.size() == 0 ? mlc::String("[]") : mlc::String("[&]");
return [&]() -> mlc::String { if (std::holds_alternative<registry::TFn>((*semantic_function_type_for_lambda))) { auto _v_tfn = std::get<registry::TFn>((*semantic_function_type_for_lambda)); auto [lambda_parameter_semantic_types, _w0] = _v_tfn; return lambda_parameter_semantic_types.size() != parameter_binding_names.size() ? expression_support::cpp_lambda_header_prefix(parameter_binding_names) : [&]() -> mlc::String { 
  mlc::Array<mlc::String> formatted_parameter_declarations = {};
  int lambda_parameter_index = 0;
  while (lambda_parameter_index < parameter_binding_names.size()){
{
mlc::String raw_param_type = type_gen::sem_type_to_cpp(context, lambda_parameter_semantic_types[lambda_parameter_index]);
mlc::String param_type_cpp = raw_param_type == mlc::String("void") ? mlc::String("std::tuple<>") : raw_param_type;
formatted_parameter_declarations.push_back(expr::parameter_declaration_item(param_type_cpp, cpp_naming::cpp_safe(parameter_binding_names[lambda_parameter_index])));
lambda_parameter_index = lambda_parameter_index + 1;
}
}
  return capture_clause + mlc::String("(") + formatted_parameter_declarations.join(mlc::String(", ")) + mlc::String(") mutable");
 }(); } return expression_support::cpp_lambda_header_prefix(parameter_binding_names); }();
}

mlc::String gen_lambda_via_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr::lambda_with_return(cpp_lambda_header_from_semantic_function_type(parameter_binding_names, semantic_function_type_for_lambda_expression, context), evaluate_expression(body_expression_under_lambda, context, gen_stmts));}

mlc::String gen_with_via_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String resource_code = evaluate_expression(resource, context, gen_stmts);
mlc::String body_code = gen_stmts(statements, context);
return expr::with_block_statement(resource_code, binder, body_code);
}

mlc::String StringExprVisitor_visit_int(expr_visitor_string::StringExprVisitor self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_integer_literal(value);}

mlc::String StringExprVisitor_visit_str(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_string_literal(value);}

mlc::String StringExprVisitor_visit_bool(expr_visitor_string::StringExprVisitor self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_boolean_literal(value);}

mlc::String StringExprVisitor_visit_unit(expr_visitor_string::StringExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_unit_literal();}

mlc::String StringExprVisitor_visit_float(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_float_literal(value);}

mlc::String StringExprVisitor_visit_i64(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_i64_literal(value);}

mlc::String StringExprVisitor_visit_u8(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_u8_literal(value);}

mlc::String StringExprVisitor_visit_usize(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_usize_literal(value);}

mlc::String StringExprVisitor_visit_char(expr_visitor_string::StringExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_char_literal(value);}

mlc::String StringExprVisitor_visit_extern(expr_visitor_string::StringExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_extern_via_visitor(_semantic_type);}

mlc::String StringExprVisitor_visit_ident(expr_visitor_string::StringExprVisitor self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return identifiers::gen_identifier(name, self.context);}

mlc::String StringExprVisitor_visit_bin(expr_visitor_string::StringExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_binary_via_visitor(operation, left_expression, right_expression, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_un(expr_visitor_string::StringExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_unary_via_visitor(operation, inner_expression, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_call(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_call_via_visitor(function_expression, arguments, mutability_flags, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_method(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_method_via_visitor(object, method_name, arguments, mutability_flags, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_field(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_field_via_visitor(object, field_name, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_index(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_index_via_visitor(object, index_expression, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_if(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_if_via_visitor(condition, then_expression, else_expression, semantic_type, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_block(expr_visitor_string::StringExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_block_via_visitor(statements, result_expression, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_while(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_while_via_visitor(condition, statements, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_for(expr_visitor_string::StringExprVisitor self, mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_for_via_visitor(variable, iterator, statements, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_match(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_match_via_visitor(subject, arms, semantic_type, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_record(expr_visitor_string::StringExprVisitor self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_record_via_visitor(type_name, field_values, semantic_type, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_record_update(expr_visitor_string::StringExprVisitor self, mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_record_update_via_visitor(type_name, base_expression, overrides, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_array(expr_visitor_string::StringExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_array_via_visitor(elements, semantic_type, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_tuple(expr_visitor_string::StringExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_tuple_via_visitor(elements, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_question(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_question_via_visitor(inner_expression, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_lambda(expr_visitor_string::StringExprVisitor self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SExpr> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_lambda_via_visitor(parameter_names, body_expression, semantic_type, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_with(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_with_via_visitor(resource, binder, statements, self.context, self.gen_stmts, self.evaluate_expression);}

mlc::String StringExprVisitor_visit_unsupported(expr_visitor_string::StringExprVisitor self, std::shared_ptr<semantic_ir::SExpr> _expression) noexcept{return mlc::String("/* unsupported expr */");}

mlc::String eval_expr_with_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
expr_visitor_string::StringExprVisitor expression_visitor = expr_visitor_string::StringExprVisitor{context, gen_stmts, evaluate_expression};
return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> mlc::String { auto [integer_value, type_value, _w0] = sexprint; return StringExprVisitor_visit_int(expression_visitor, integer_value, type_value); },
  [&](const SExprStr& sexprstr) -> mlc::String { auto [string_value, type_value, _w0] = sexprstr; return StringExprVisitor_visit_str(expression_visitor, string_value, type_value); },
  [&](const SExprBool& sexprbool) -> mlc::String { auto [boolean_value, type_value, _w0] = sexprbool; return StringExprVisitor_visit_bool(expression_visitor, boolean_value, type_value); },
  [&](const SExprUnit& sexprunit) -> mlc::String { auto [type_value, _w0] = sexprunit; return StringExprVisitor_visit_unit(expression_visitor, type_value); },
  [&](const SExprFloat& sexprfloat) -> mlc::String { auto [float_value, type_value, _w0] = sexprfloat; return StringExprVisitor_visit_float(expression_visitor, float_value, type_value); },
  [&](const SExprI64& sexpri64) -> mlc::String { auto [integer_value, type_value, _w0] = sexpri64; return StringExprVisitor_visit_i64(expression_visitor, integer_value, type_value); },
  [&](const SExprU8& sexpru8) -> mlc::String { auto [byte_value, type_value, _w0] = sexpru8; return StringExprVisitor_visit_u8(expression_visitor, byte_value, type_value); },
  [&](const SExprUsize& sexprusize) -> mlc::String { auto [size_value, type_value, _w0] = sexprusize; return StringExprVisitor_visit_usize(expression_visitor, size_value, type_value); },
  [&](const SExprChar& sexprchar) -> mlc::String { auto [character_value, type_value, _w0] = sexprchar; return StringExprVisitor_visit_char(expression_visitor, character_value, type_value); },
  [&](const SExprExtern& sexprextern) -> mlc::String { auto [type_value, _w0] = sexprextern; return StringExprVisitor_visit_extern(expression_visitor, type_value); },
  [&](const SExprIdent& sexprident) -> mlc::String { auto [name_value, type_value, _w0] = sexprident; return StringExprVisitor_visit_ident(expression_visitor, name_value, type_value); },
  [&](const SExprBin& sexprbin) -> mlc::String { auto [operation, left_expression, right_expression, type_value, _w0] = sexprbin; return StringExprVisitor_visit_bin(expression_visitor, operation, left_expression, right_expression, type_value); },
  [&](const SExprUn& sexprun) -> mlc::String { auto [operation, inner_expression, type_value, _w0] = sexprun; return StringExprVisitor_visit_un(expression_visitor, operation, inner_expression, type_value); },
  [&](const SExprCall& sexprcall) -> mlc::String { auto [function_expression, arguments, mutability_flags, type_value, _w0] = sexprcall; return StringExprVisitor_visit_call(expression_visitor, function_expression, arguments, mutability_flags, type_value); },
  [&](const SExprMethod& sexprmethod) -> mlc::String { auto [object, method_name, arguments, mutability_flags, type_value, _w0] = sexprmethod; return StringExprVisitor_visit_method(expression_visitor, object, method_name, arguments, mutability_flags, type_value); },
  [&](const SExprField& sexprfield) -> mlc::String { auto [object, field_name, type_value, _w0] = sexprfield; return StringExprVisitor_visit_field(expression_visitor, object, field_name, type_value); },
  [&](const SExprIndex& sexprindex) -> mlc::String { auto [object, index_expression, type_value, _w0] = sexprindex; return StringExprVisitor_visit_index(expression_visitor, object, index_expression, type_value); },
  [&](const SExprIf& sexprif) -> mlc::String { auto [condition, then_expression, else_expression, type_value, _w0] = sexprif; return StringExprVisitor_visit_if(expression_visitor, condition, then_expression, else_expression, type_value); },
  [&](const SExprBlock& sexprblock) -> mlc::String { auto [statements, result_expression, type_value, _w0] = sexprblock; return StringExprVisitor_visit_block(expression_visitor, statements, result_expression, type_value); },
  [&](const SExprWhile& sexprwhile) -> mlc::String { auto [condition, statements, type_value, _w0] = sexprwhile; return StringExprVisitor_visit_while(expression_visitor, condition, statements, type_value); },
  [&](const SExprFor& sexprfor) -> mlc::String { auto [variable, iterator, statements, type_value, _w0] = sexprfor; return StringExprVisitor_visit_for(expression_visitor, variable, iterator, statements, type_value); },
  [&](const SExprMatch& sexprmatch) -> mlc::String { auto [subject, arms, type_value, _w0] = sexprmatch; return StringExprVisitor_visit_match(expression_visitor, subject, arms, type_value); },
  [&](const SExprRecord& sexprrecord) -> mlc::String { auto [type_name, field_values, type_value, _w0] = sexprrecord; return StringExprVisitor_visit_record(expression_visitor, type_name, field_values, type_value); },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> mlc::String { auto [type_name, base_expression, overrides, type_value, _w0] = sexprrecordupdate; return StringExprVisitor_visit_record_update(expression_visitor, type_name, base_expression, overrides, type_value); },
  [&](const SExprArray& sexprarray) -> mlc::String { auto [elements, type_value, _w0] = sexprarray; return StringExprVisitor_visit_array(expression_visitor, elements, type_value); },
  [&](const SExprTuple& sexprtuple) -> mlc::String { auto [elements, type_value, _w0] = sexprtuple; return StringExprVisitor_visit_tuple(expression_visitor, elements, type_value); },
  [&](const SExprQuestion& sexprquestion) -> mlc::String { auto [inner_expression, type_value, _w0] = sexprquestion; return StringExprVisitor_visit_question(expression_visitor, inner_expression, type_value); },
  [&](const SExprLambda& sexprlambda) -> mlc::String { auto [parameter_names, body_expression, type_value, _w0] = sexprlambda; return StringExprVisitor_visit_lambda(expression_visitor, parameter_names, body_expression, type_value); },
  [&](const SExprWith& sexprwith) -> mlc::String { auto [resource, binder, statements, type_value, _w0] = sexprwith; return StringExprVisitor_visit_with(expression_visitor, resource, binder, statements, type_value); }
}, (*expression)._);
}

mlc::String gen_expr_via_string_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept{return eval_expr_with_visitor(expression, context, ignored_gen_stmts, evaluate_via_string_visitor);}

} // namespace expr_visitor_string
