#include "expr_visitor_cpp.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "semantic_type_structure.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "expr_fragment_codegen.hpp"
#include "expression_support.hpp"
#include "cpp_naming.hpp"
#include "expr_visitor.hpp"
#include "context.hpp"

namespace expr_visitor_cpp {

using namespace registry;
using namespace semantic_ir;
using namespace cpp_ast;
using namespace cpp_printer;
using namespace emit_helpers;
using namespace semantic_type_structure;
using namespace literals;
using namespace identifiers;
using namespace expr_fragment_codegen;
using namespace expression_support;
using namespace cpp_naming;
using namespace expr_visitor;
using namespace context;

struct CppExprVisitor {context::CodegenContext context;std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts;std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression;};

std::shared_ptr<cpp_ast::CppExpr> unsupported_cpp_expression() noexcept;

std::shared_ptr<cpp_ast::CppExpr> evaluate_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> _gen_stmts) noexcept;

mlc::String ignored_gen_stmts(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> _statements, context::CodegenContext _context) noexcept;

std::shared_ptr<cpp_ast::CppExpr> cpp_expr_from_rendered_fragment(mlc::String fragment) noexcept;

mlc::String eval_expr_cpp_as_string(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> eval_expr_cpp(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_call_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_method_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_if_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_block_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_match_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_record_via_cpp_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_array_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_lambda_via_cpp_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_field_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_index_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_while_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_for_via_cpp_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_record_update_via_cpp_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_tuple_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_question_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_with_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_binary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_unary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_int(expr_visitor_cpp::CppExprVisitor self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_str(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_bool(expr_visitor_cpp::CppExprVisitor self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_unit(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_float(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_i64(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_u8(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_usize(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_char(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_extern(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_ident(expr_visitor_cpp::CppExprVisitor self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_bin(expr_visitor_cpp::CppExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_un(expr_visitor_cpp::CppExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_call(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_method(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_field(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_index(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_if(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_block(expr_visitor_cpp::CppExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_while(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_for(expr_visitor_cpp::CppExprVisitor self, mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_match(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_record(expr_visitor_cpp::CppExprVisitor self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_record_update(expr_visitor_cpp::CppExprVisitor self, mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_array(expr_visitor_cpp::CppExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_tuple(expr_visitor_cpp::CppExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_question(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_lambda(expr_visitor_cpp::CppExprVisitor self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SExpr> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_with(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_unsupported(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> _expression) noexcept;

std::shared_ptr<cpp_ast::CppExpr> eval_expr_with_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_expr_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpr> unsupported_cpp_expression() noexcept{return emit_helpers::make_identifier_cpp_expression(mlc::String("/* unsupported expr */"));}

std::shared_ptr<cpp_ast::CppExpr> evaluate_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> _gen_stmts) noexcept{return gen_expr_via_cpp_visitor(expression, context);}

mlc::String ignored_gen_stmts(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> _statements, context::CodegenContext _context) noexcept{return mlc::String("");}

std::shared_ptr<cpp_ast::CppExpr> cpp_expr_from_rendered_fragment(mlc::String fragment) noexcept{return emit_helpers::make_identifier_cpp_expression(fragment);}

mlc::String eval_expr_cpp_as_string(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_printer::print_expr(eval_expr_cpp(expression, context, gen_stmts));}

std::shared_ptr<cpp_ast::CppExpr> eval_expr_cpp(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return eval_expr_with_cpp_visitor(expression, context, gen_stmts, eval_expr_cpp);}

std::shared_ptr<cpp_ast::CppExpr> gen_call_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_call_via_visitor(function_expression, arguments, call_parameter_mutability_flags, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_method_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_method_via_visitor(object, method_name, arguments, receiver_and_parameter_mutability_flags, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_if_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_if_via_visitor(condition, then_expression, else_expression, if_semantic_type, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_block_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_block_via_visitor(statements, result_expression, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_match_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_match_via_visitor(subject, arms, match_semantic_type, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_record_via_cpp_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_record_via_visitor(type_name, field_values, expression_type, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_array_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_array_via_visitor(elements, element_container_semantic_type, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_lambda_via_cpp_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_lambda_via_visitor(parameter_binding_names, body_expression_under_lambda, semantic_function_type_for_lambda_expression, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_field_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{
std::shared_ptr<cpp_ast::CppExpr> object_expression = eval_expr_cpp(object, context, gen_stmts);
bool is_pointer = expression_support::field_access_operator(object, context) == mlc::String("->");
return std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppMember(object_expression, cpp_naming::cpp_safe(field_name), is_pointer));
}

std::shared_ptr<cpp_ast::CppExpr> gen_index_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIndex(eval_expr_cpp(object, context, gen_stmts), eval_expr_cpp(index_expression, context, gen_stmts)));}

std::shared_ptr<cpp_ast::CppExpr> gen_while_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_while_via_visitor(condition, statements, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_for_via_cpp_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_for_via_visitor(variable, iterator, statements, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_record_update_via_cpp_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_record_update_via_visitor(type_name, base_expression, overrides, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_tuple_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_tuple_via_visitor(elements, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_question_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_question_via_visitor(inner_expression, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_with_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_with_via_visitor(resource, binder, statements, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> gen_binary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String method = semantic_type_structure::operator_method_for(operation);
std::shared_ptr<registry::Type> left_type = semantic_ir::sexpr_type(left_expression);
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*left_type))) { auto _v_tnamed = std::get<registry::TNamed>((*left_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return method != mlc::String("") && type_name != mlc::String("") ? std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(type_name + mlc::String("_") + method), mlc::Array<std::shared_ptr<cpp_ast::CppExpr>>{evaluate_expression(left_expression, context, gen_stmts), evaluate_expression(right_expression, context, gen_stmts)})) : cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_binary_via_visitor(operation, left_expression, right_expression, context, gen_stmts, eval_expr_cpp_as_string));
}

std::shared_ptr<cpp_ast::CppExpr> gen_unary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return cpp_expr_from_rendered_fragment(expr_fragment_codegen::gen_unary_via_visitor(operation, inner_expression, context, gen_stmts, eval_expr_cpp_as_string));}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_int(expr_visitor_cpp::CppExprVisitor self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_integer_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_str(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_string_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_bool(expr_visitor_cpp::CppExprVisitor self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_boolean_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_unit(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_unit_literal_cpp();}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_float(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_float_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_i64(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_i64_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_u8(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_u8_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_usize(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_usize_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_char(expr_visitor_cpp::CppExprVisitor self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_char_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_extern(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_extern_placeholder_cpp();}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_ident(expr_visitor_cpp::CppExprVisitor self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return identifiers::gen_identifier_cpp(name, self.context);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_bin(expr_visitor_cpp::CppExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_binary_via_cpp_visitor(operation, left_expression, right_expression, self.context, self.gen_stmts, self.evaluate_expression);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_un(expr_visitor_cpp::CppExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_unary_via_cpp_visitor(operation, inner_expression, self.context, self.gen_stmts, self.evaluate_expression);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_call(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_call_via_cpp_visitor(function_expression, arguments, mutability_flags, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_method(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_method_via_cpp_visitor(object, method_name, arguments, mutability_flags, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_field(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_field_via_cpp_visitor(object, field_name, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_index(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_index_via_cpp_visitor(object, index_expression, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_if(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_if_via_cpp_visitor(condition, then_expression, else_expression, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_block(expr_visitor_cpp::CppExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_block_via_cpp_visitor(statements, result_expression, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_while(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_while_via_cpp_visitor(condition, statements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_for(expr_visitor_cpp::CppExprVisitor self, mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_for_via_cpp_visitor(variable, iterator, statements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_match(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_match_via_cpp_visitor(subject, arms, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_record(expr_visitor_cpp::CppExprVisitor self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_record_via_cpp_visitor(type_name, field_values, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_record_update(expr_visitor_cpp::CppExprVisitor self, mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_record_update_via_cpp_visitor(type_name, base_expression, overrides, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_array(expr_visitor_cpp::CppExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_array_via_cpp_visitor(elements, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_tuple(expr_visitor_cpp::CppExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_tuple_via_cpp_visitor(elements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_question(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_question_via_cpp_visitor(inner_expression, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_lambda(expr_visitor_cpp::CppExprVisitor self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SExpr> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_lambda_via_cpp_visitor(parameter_names, body_expression, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_with(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_with_via_cpp_visitor(resource, binder, statements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpr> CppExprVisitor_visit_unsupported(expr_visitor_cpp::CppExprVisitor self, std::shared_ptr<semantic_ir::SExpr> _expression) noexcept{return unsupported_cpp_expression();}

std::shared_ptr<cpp_ast::CppExpr> eval_expr_with_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept{return std::visit(overloaded{
  [&](const SExprInt& sexprint) -> std::shared_ptr<cpp_ast::CppExpr> { auto [integer_value, _w0, _w1] = sexprint; return literals::gen_integer_literal_cpp(integer_value); },
  [&](const SExprStr& sexprstr) -> std::shared_ptr<cpp_ast::CppExpr> { auto [string_value, _w0, _w1] = sexprstr; return literals::gen_string_literal_cpp(string_value); },
  [&](const SExprBool& sexprbool) -> std::shared_ptr<cpp_ast::CppExpr> { auto [boolean_value, _w0, _w1] = sexprbool; return literals::gen_boolean_literal_cpp(boolean_value); },
  [&](const SExprUnit& sexprunit) -> std::shared_ptr<cpp_ast::CppExpr> { auto [_w0, _w1] = sexprunit; return literals::gen_unit_literal_cpp(); },
  [&](const SExprFloat& sexprfloat) -> std::shared_ptr<cpp_ast::CppExpr> { auto [float_value, _w0, _w1] = sexprfloat; return literals::gen_float_literal_cpp(float_value); },
  [&](const SExprI64& sexpri64) -> std::shared_ptr<cpp_ast::CppExpr> { auto [integer_value, _w0, _w1] = sexpri64; return literals::gen_i64_literal_cpp(integer_value); },
  [&](const SExprU8& sexpru8) -> std::shared_ptr<cpp_ast::CppExpr> { auto [byte_value, _w0, _w1] = sexpru8; return literals::gen_u8_literal_cpp(byte_value); },
  [&](const SExprUsize& sexprusize) -> std::shared_ptr<cpp_ast::CppExpr> { auto [size_value, _w0, _w1] = sexprusize; return literals::gen_usize_literal_cpp(size_value); },
  [&](const SExprChar& sexprchar) -> std::shared_ptr<cpp_ast::CppExpr> { auto [character_value, _w0, _w1] = sexprchar; return literals::gen_char_literal_cpp(character_value); },
  [&](const SExprExtern& sexprextern) -> std::shared_ptr<cpp_ast::CppExpr> { auto [_w0, _w1] = sexprextern; return literals::gen_extern_placeholder_cpp(); },
  [&](const SExprIdent& sexprident) -> std::shared_ptr<cpp_ast::CppExpr> { auto [name_value, _w0, _w1] = sexprident; return identifiers::gen_identifier_cpp(name_value, context); },
  [&](const SExprBin& sexprbin) -> std::shared_ptr<cpp_ast::CppExpr> { auto [operation, left_expression, right_expression, _w0, _w1] = sexprbin; return gen_binary_via_cpp_visitor(operation, left_expression, right_expression, context, gen_stmts, evaluate_expression); },
  [&](const SExprUn& sexprun) -> std::shared_ptr<cpp_ast::CppExpr> { auto [operation, inner_expression, _w0, _w1] = sexprun; return gen_unary_via_cpp_visitor(operation, inner_expression, context, gen_stmts, evaluate_expression); },
  [&](const SExprCall& sexprcall) -> std::shared_ptr<cpp_ast::CppExpr> { auto [function_expression, arguments, mutability_flags, _w0, _w1] = sexprcall; return gen_call_via_cpp_visitor(function_expression, arguments, mutability_flags, context, gen_stmts); },
  [&](const SExprMethod& sexprmethod) -> std::shared_ptr<cpp_ast::CppExpr> { auto [object, method_name, arguments, mutability_flags, _w0, _w1] = sexprmethod; return gen_method_via_cpp_visitor(object, method_name, arguments, mutability_flags, context, gen_stmts); },
  [&](const SExprField& sexprfield) -> std::shared_ptr<cpp_ast::CppExpr> { auto [object, field_name, _w0, _w1] = sexprfield; return gen_field_via_cpp_visitor(object, field_name, context, gen_stmts); },
  [&](const SExprIndex& sexprindex) -> std::shared_ptr<cpp_ast::CppExpr> { auto [object, index_expression, _w0, _w1] = sexprindex; return gen_index_via_cpp_visitor(object, index_expression, context, gen_stmts); },
  [&](const SExprIf& sexprif) -> std::shared_ptr<cpp_ast::CppExpr> { auto [condition, then_expression, else_expression, if_semantic_type, _w0] = sexprif; return gen_if_via_cpp_visitor(condition, then_expression, else_expression, if_semantic_type, context, gen_stmts); },
  [&](const SExprBlock& sexprblock) -> std::shared_ptr<cpp_ast::CppExpr> { auto [statements, result_expression, _w0, _w1] = sexprblock; return gen_block_via_cpp_visitor(statements, result_expression, context, gen_stmts); },
  [&](const SExprWhile& sexprwhile) -> std::shared_ptr<cpp_ast::CppExpr> { auto [condition, statements, _w0, _w1] = sexprwhile; return gen_while_via_cpp_visitor(condition, statements, context, gen_stmts); },
  [&](const SExprFor& sexprfor) -> std::shared_ptr<cpp_ast::CppExpr> { auto [variable, iterator, statements, _w0, _w1] = sexprfor; return gen_for_via_cpp_visitor(variable, iterator, statements, context, gen_stmts); },
  [&](const SExprMatch& sexprmatch) -> std::shared_ptr<cpp_ast::CppExpr> { auto [subject, arms, match_semantic_type, _w0] = sexprmatch; return gen_match_via_cpp_visitor(subject, arms, match_semantic_type, context, gen_stmts); },
  [&](const SExprRecord& sexprrecord) -> std::shared_ptr<cpp_ast::CppExpr> { auto [type_name, field_values, semantic_type, _w0] = sexprrecord; return gen_record_via_cpp_visitor(type_name, field_values, semantic_type, context, gen_stmts); },
  [&](const SExprRecordUpdate& sexprrecordupdate) -> std::shared_ptr<cpp_ast::CppExpr> { auto [type_name, base_expression, overrides, _w0, _w1] = sexprrecordupdate; return gen_record_update_via_cpp_visitor(type_name, base_expression, overrides, context, gen_stmts); },
  [&](const SExprArray& sexprarray) -> std::shared_ptr<cpp_ast::CppExpr> { auto [elements, semantic_type, _w0] = sexprarray; return gen_array_via_cpp_visitor(elements, semantic_type, context, gen_stmts); },
  [&](const SExprTuple& sexprtuple) -> std::shared_ptr<cpp_ast::CppExpr> { auto [elements, _w0, _w1] = sexprtuple; return gen_tuple_via_cpp_visitor(elements, context, gen_stmts); },
  [&](const SExprQuestion& sexprquestion) -> std::shared_ptr<cpp_ast::CppExpr> { auto [inner_expression, _w0, _w1] = sexprquestion; return gen_question_via_cpp_visitor(inner_expression, context, gen_stmts); },
  [&](const SExprLambda& sexprlambda) -> std::shared_ptr<cpp_ast::CppExpr> { auto [parameter_names, body_expression, semantic_type, _w0] = sexprlambda; return gen_lambda_via_cpp_visitor(parameter_names, body_expression, semantic_type, context, gen_stmts); },
  [&](const SExprWith& sexprwith) -> std::shared_ptr<cpp_ast::CppExpr> { auto [resource, binder, statements, _w0, _w1] = sexprwith; return gen_with_via_cpp_visitor(resource, binder, statements, context, gen_stmts); }
}, (*expression)._);}

std::shared_ptr<cpp_ast::CppExpr> gen_expr_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept{return eval_expr_with_cpp_visitor(expression, context, ignored_gen_stmts, evaluate_via_cpp_visitor);}

} // namespace expr_visitor_cpp
