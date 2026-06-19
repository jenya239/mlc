#include "expr_visitor_cpp.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "semantic_type_structure.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "expression_support.hpp"
#include "match_analysis.hpp"
#include "match_gen.hpp"
#include "mut_actual_argument.hpp"
#include "method_gen.hpp"
#include "record_gen.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "context.hpp"
#include "statement_context.hpp"
#include "expr_visitor.hpp"

namespace expr_visitor_cpp {

using namespace registry;
using namespace semantic_ir;
using namespace cpp_ast;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace semantic_type_structure;
using namespace literals;
using namespace identifiers;
using namespace expression_support;
using namespace match_analysis;
using namespace match_gen;
using namespace mut_actual_argument;
using namespace method_gen;
using namespace record_gen;
using namespace cpp_naming;
using namespace type_gen;
using namespace context;
using namespace statement_context;
using namespace expr_visitor;

struct CodegenPass {context::CodegenContext context;std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts;std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression;int seed;};

expr_visitor_cpp::CodegenPass codegen_pass_new(context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> unsupported_cpp_expression() noexcept;

std::shared_ptr<cpp_ast::CppExpression> evaluate_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> _gen_stmts) noexcept;

mlc::String ignored_gen_stmts(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> _statements, context::CodegenContext _context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> eval_expr_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_call_function_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_constructor_call_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> argument_expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_call_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_immediate_lambda() noexcept;

std::shared_ptr<cpp_ast::CppCapture> value_capture_for_lambda() noexcept;

std::shared_ptr<cpp_ast::CppExpression> invoke_immediate_lambda_cpp(std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_if_else_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String unit_if_block_trailing(std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppStatement> gen_if_cpp_statement(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_if_unit_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_if_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_block_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_match_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> record_field_expressions(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String type_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_record_via_cpp_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_array_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> lambda_capture_list(mlc::Array<mlc::String> parameter_binding_names) noexcept;

std::shared_ptr<cpp_ast::CppType> lambda_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_parameter_type) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppParam>> lambda_auto_parameters_cpp(mlc::Array<mlc::String> parameter_binding_names) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppParam>> lambda_parameters_cpp(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_lambda_via_cpp_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_field_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_index_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_while_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_for_via_cpp_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_record_update_via_cpp_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_tuple_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_question_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_with_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_binary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_unary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> dispatch_codegen_pass(expr_visitor_cpp::CodegenPass codegen_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_int(expr_visitor_cpp::CodegenPass self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_str(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bool(expr_visitor_cpp::CodegenPass self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unit(expr_visitor_cpp::CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_float(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_i64(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_u8(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_usize(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_char(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_extern(expr_visitor_cpp::CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_ident(expr_visitor_cpp::CodegenPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bin(expr_visitor_cpp::CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_un(expr_visitor_cpp::CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_call(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_method(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_field(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_index(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_if(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_block(expr_visitor_cpp::CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_while(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_for(expr_visitor_cpp::CodegenPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_match(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record(expr_visitor_cpp::CodegenPass self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record_update(expr_visitor_cpp::CodegenPass self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_array(expr_visitor_cpp::CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_tuple(expr_visitor_cpp::CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_question(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_lambda(expr_visitor_cpp::CodegenPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_with(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unsupported(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> eval_expr_with_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_expr_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

expr_visitor_cpp::CodegenPass codegen_pass_new(context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return expr_visitor_cpp::CodegenPass{context, gen_stmts, evaluate_expression, 0};}

std::shared_ptr<cpp_ast::CppExpression> unsupported_cpp_expression() noexcept{return emit_helpers::make_identifier_cpp_expression(mlc::String("/* unsupported expr */"));}

std::shared_ptr<cpp_ast::CppExpression> evaluate_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> _gen_stmts) noexcept{return gen_expr_via_cpp_visitor(expression, context);}

mlc::String ignored_gen_stmts(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> _statements, context::CodegenContext _context) noexcept{return mlc::String("");}

std::shared_ptr<cpp_ast::CppExpression> eval_expr_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return eval_expr_with_cpp_visitor(expression, context, gen_stmts, eval_expr_cpp);}

std::shared_ptr<cpp_ast::CppExpression> gen_call_function_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*function_expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*function_expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return emit_helpers::make_identifier_cpp_expression(context::qualify_function_callee(cpp_naming::map_builtin(name), context)); } return eval_expr_cpp(function_expression, context, gen_stmts); }();}

std::shared_ptr<cpp_ast::CppExpression> gen_constructor_call_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> argument_expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*function_expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*function_expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String callee_code = context::qualify_function_callee(cpp_naming::map_builtin(name), context);
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit(callee_code, argument_expressions));
 }(); } return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(gen_call_function_cpp_expression(function_expression, context, gen_stmts), argument_expressions)); }();}

std::shared_ptr<cpp_ast::CppExpression> gen_call_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
mut_actual_argument::MutActualArgumentMaterializationOutcomeCpp materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_cpp(arguments, call_parameter_mutability_flags, context, gen_stmts, eval_expr_cpp);
std::shared_ptr<cpp_ast::CppExpression> invoke_expression = expression_support::is_constructor_call(function_expression) ? gen_constructor_call_cpp_expression(function_expression, materialization_outcome.materialized_argument_expressions, context, gen_stmts) : std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(gen_call_function_cpp_expression(function_expression, context, gen_stmts), materialization_outcome.materialized_argument_expressions));
return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, invoke_expression);
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return method_gen::gen_method_expr_cpp(object, method_name, arguments, receiver_and_parameter_mutability_flags, context, gen_stmts, eval_expr_cpp);}

std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_immediate_lambda() noexcept{return std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String(""), true});}

std::shared_ptr<cpp_ast::CppCapture> value_capture_for_lambda() noexcept{return std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String("="), false});}

std::shared_ptr<cpp_ast::CppExpression> invoke_immediate_lambda_cpp(std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppLambda(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{reference_capture_for_immediate_lambda()}, {}, return_type, body_statements)), {}));}

std::shared_ptr<cpp_ast::CppExpression> gen_if_else_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*else_expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*else_expression)._); auto [elements, _w0, array_span] = _v_semanticexpressionarray; return elements.size() > 0 ? eval_expr_cpp(else_expression, context, gen_stmts) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  std::shared_ptr<registry::Type> inner_preferred = semantic_type_structure::array_element_type_from_array_type(if_semantic_type);
  std::shared_ptr<registry::Type> inner_from_then_branch = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(then_expression));
  std::shared_ptr<registry::Type> inner_for_empty_array = !semantic_type_structure::type_is_unknown(inner_preferred) ? inner_preferred : inner_from_then_branch;
  return semantic_type_structure::type_is_unknown(inner_for_empty_array) ? eval_expr_cpp(else_expression, context, gen_stmts) : eval_expr_cpp(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(elements, std::make_shared<registry::Type>(registry::TArray(inner_for_empty_array)), array_span)), context, gen_stmts);
 }(); } return eval_expr_cpp(else_expression, context, gen_stmts); }();}

mlc::String unit_if_block_trailing(std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*result_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*result_expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return cpp_ast::print_statement(gen_if_cpp_statement(condition, then_expression, else_expression, semantic_type, final_context, gen_stmts)); } return cpp_ast::print_expr(eval_expr_cpp(result_expression, final_context, gen_stmts)) + mlc::String(";\n"); }();}

std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*branch_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*branch_expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { 
  mlc::String body_fragment = gen_stmts(statements, context);
  context::CodegenContext final_context = statement_context::stmts_final_ctx(statements, context);
  mlc::String trailing = unit_if_block_trailing(result_expression, final_context, gen_stmts);
  return emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_fragment_cpp_statement(body_fragment + trailing)});
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*branch_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*branch_expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_expression_cpp_statement(gen_if_via_cpp_visitor(condition, then_expression, else_expression, semantic_type, context, gen_stmts))}); } return emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_expression_cpp_statement(eval_expr_cpp(branch_expression, context, gen_stmts))}); }();}

std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return emit_helpers::make_block_cpp_statement({}); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*else_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*else_expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return gen_if_cpp_statement(condition, then_expression, else_expression, semantic_type, context, gen_stmts); } return unit_if_branch_block(else_expression, context, gen_stmts); }();}

std::shared_ptr<cpp_ast::CppStatement> gen_if_cpp_statement(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return emit_helpers::make_if_cpp_statement(eval_expr_cpp(condition, context, gen_stmts), unit_if_branch_block(then_expression, context, gen_stmts), unit_if_else_branch(else_expression, context, gen_stmts));}

std::shared_ptr<cpp_ast::CppExpression> gen_if_unit_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return invoke_immediate_lambda_cpp(std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("void"))), mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{gen_if_cpp_statement(condition, then_expression, else_expression, if_semantic_type, context, gen_stmts)});}

std::shared_ptr<cpp_ast::CppExpression> gen_if_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
std::shared_ptr<cpp_ast::CppExpression> condition_expression = eval_expr_cpp(condition, context, gen_stmts);
std::shared_ptr<cpp_ast::CppExpression> then_expression_cpp = eval_expr_cpp(then_expression, context, gen_stmts);
std::shared_ptr<cpp_ast::CppExpression> else_expression_cpp = gen_if_else_cpp_expression(else_expression, if_semantic_type, then_expression, context, gen_stmts);
return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<registry::TNamed>((*if_semantic_type))) { auto _v_tnamed = std::get<registry::TNamed>((*if_semantic_type)); auto [type_name] = _v_tnamed; return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String type_cpp = context::CodegenContext_resolve(context, type_name);
  return type_cpp == mlc::String("auto") ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppTernary(condition_expression, then_expression_cpp, else_expression_cpp)) : invoke_immediate_lambda_cpp(std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(type_cpp)), mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_if_cpp_statement(condition_expression, emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(then_expression_cpp)}), emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_return_cpp_statement(else_expression_cpp)}))});
 }(); } if (std::holds_alternative<registry::TUnit>((*if_semantic_type))) {  return gen_if_unit_via_cpp_visitor(condition, then_expression, else_expression, if_semantic_type, context, gen_stmts); } return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppTernary(condition_expression, then_expression_cpp, else_expression_cpp)); }();
}

std::shared_ptr<cpp_ast::CppExpression> gen_block_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return statements.size() == 0 ? eval_expr_cpp(result_expression, context, gen_stmts) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  context::CodegenContext final_context = statement_context::stmts_final_ctx(statements, context);
  mlc::String body_fragment = gen_stmts(statements, context) + mlc::String("return ") + cpp_ast::print_expr(eval_expr_cpp(result_expression, final_context, gen_stmts)) + mlc::String(";\n");
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock(body_fragment));
 }();}

std::shared_ptr<cpp_ast::CppExpression> gen_match_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms = match_gen::expand_or_arms(arms);
return match_gen::expanded_any_guard(expanded_arms) ? match_gen::gen_match_guarded_expression_cpp(subject, expanded_arms, match_semantic_type, context, gen_stmts, eval_expr_cpp) : match_analysis::should_use_string_match_if_chain(subject, expanded_arms) ? match_gen::gen_match_string_literal_expression_cpp(subject, expanded_arms, match_semantic_type, context, gen_stmts, eval_expr_cpp) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  bool visit_void = match_gen::match_visit_uses_void_lambdas(match_semantic_type);
  mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> handlers = expanded_arms.map([context, gen_stmts, visit_void](std::shared_ptr<semantic_ir::SemanticMatchArm> arm) mutable { return match_gen::gen_arm_cpp(arm, context, gen_stmts, eval_expr_cpp, visit_void); });
  std::shared_ptr<cpp_ast::CppExpression> subject_expression = eval_expr_cpp(subject, context, gen_stmts);
  bool needs_dereference = expanded_arms.size() > 0 ? match_analysis::first_arm_needs_deref(expanded_arms[0], subject, context) : true;
  std::shared_ptr<cpp_ast::CppExpression> visit_subject = needs_dereference ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppUnary(mlc::String("*"), subject_expression)) : subject_expression;
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppStdVisit(visit_subject, handlers));
 }();
}

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> record_field_expressions(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String type_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<mlc::String> field_order = context::lookup_fields_for_context(context, type_name);
return field_order.size() > 0 ? field_order.map([field_values, context, gen_stmts](mlc::String field_name) mutable { return eval_expr_cpp(expression_support::find_field_value(field_values, field_name), context, gen_stmts); }) : field_values.map([context, gen_stmts](std::shared_ptr<semantic_ir::SemanticFieldVal> field_value) mutable { return eval_expr_cpp(field_value->value, context, gen_stmts); });
}

std::shared_ptr<cpp_ast::CppExpression> gen_record_via_cpp_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::String cpp_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*expression_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*expression_type)); auto [_w0, _w1] = _v_tgeneric; return type_gen::sem_type_to_cpp(context, expression_type); } return context::CodegenContext_resolve(context, type_name); }();
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit(cpp_name, record_field_expressions(field_values, type_name, context, gen_stmts)));
}

std::shared_ptr<cpp_ast::CppExpression> gen_array_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> element_expressions = elements.map([context, gen_stmts](std::shared_ptr<semantic_ir::SemanticExpression> element) mutable { return eval_expr_cpp(element, context, gen_stmts); });
return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<registry::TArray>((*element_container_semantic_type))) { auto _v_tarray = std::get<registry::TArray>((*element_container_semantic_type)); auto [inner_type] = _v_tarray; return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
  return elements.size() == 0 ? element_cpp == mlc::String("auto") ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList({})) : std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit(mlc::String("mlc::Array<") + element_cpp + mlc::String(">"), {})) : element_cpp == mlc::String("auto") ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit(mlc::String("mlc::Array"), element_expressions)) : std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit(mlc::String("mlc::Array<") + element_cpp + mlc::String(">"), element_expressions));
 }(); } return elements.size() == 0 ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList({})) : std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit(mlc::String("mlc::Array"), element_expressions)); }();
}

mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> lambda_capture_list(mlc::Array<mlc::String> parameter_binding_names) noexcept{return parameter_binding_names.size() == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{} : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures = {};
  captures.push_back(value_capture_for_lambda());
  return captures;
 }();}

std::shared_ptr<cpp_ast::CppType> lambda_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_parameter_type) noexcept{
mlc::String parameter_type_cpp = type_gen::sem_type_to_cpp(context, semantic_parameter_type);
return parameter_type_cpp == mlc::String("void") ? std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("std::tuple<>"))) : std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(parameter_type_cpp));
}

mlc::Array<std::shared_ptr<cpp_ast::CppParam>> lambda_auto_parameters_cpp(mlc::Array<mlc::String> parameter_binding_names) noexcept{return parameter_binding_names.map([](mlc::String parameter_name) mutable { return std::make_shared<cpp_ast::CppParam>(cpp_ast::CppParam{cpp_naming::cpp_safe(parameter_name), std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mlc::String("auto")))}); });}

mlc::Array<std::shared_ptr<cpp_ast::CppParam>> lambda_parameters_cpp(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppParam>> { if (std::holds_alternative<registry::TFn>((*semantic_function_type_for_lambda_expression))) { auto _v_tfn = std::get<registry::TFn>((*semantic_function_type_for_lambda_expression)); auto [lambda_parameter_semantic_types, _w0] = _v_tfn; return lambda_parameter_semantic_types.size() != parameter_binding_names.size() ? lambda_auto_parameters_cpp(parameter_binding_names) : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppParam>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters = {};
  int lambda_parameter_index = 0;
  while (lambda_parameter_index < parameter_binding_names.size()){
{
parameters.push_back(std::make_shared<cpp_ast::CppParam>(cpp_ast::CppParam{cpp_naming::cpp_safe(parameter_binding_names[lambda_parameter_index]), lambda_parameter_type_cpp(context, lambda_parameter_semantic_types[lambda_parameter_index])}));
lambda_parameter_index = lambda_parameter_index + 1;
}
}
  return parameters;
 }(); } return lambda_auto_parameters_cpp(parameter_binding_names); }();}

std::shared_ptr<cpp_ast::CppExpression> gen_lambda_via_cpp_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMutableLambda(lambda_capture_list(parameter_binding_names), lambda_parameters_cpp(parameter_binding_names, semantic_function_type_for_lambda_expression, context), eval_expr_cpp(body_expression_under_lambda, context, gen_stmts)));}

std::shared_ptr<cpp_ast::CppExpression> gen_field_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
std::shared_ptr<cpp_ast::CppExpression> object_expression = eval_expr_cpp(object, context, gen_stmts);
bool is_pointer = expression_support::field_access_operator(object, context) == mlc::String("->");
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(object_expression, cpp_naming::cpp_safe(field_name), is_pointer));
}

std::shared_ptr<cpp_ast::CppExpression> gen_index_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIndex(eval_expr_cpp(object, context, gen_stmts), eval_expr_cpp(index_expression, context, gen_stmts)));}

std::shared_ptr<cpp_ast::CppExpression> gen_while_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedWhile(eval_expr_cpp(condition, context, gen_stmts), gen_stmts(statements, context)));}

std::shared_ptr<cpp_ast::CppExpression> gen_for_via_cpp_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedFor(cpp_naming::cpp_safe(variable), eval_expr_cpp(iterator, context, gen_stmts), gen_stmts(statements, context)));}

std::shared_ptr<cpp_ast::CppExpression> gen_record_update_via_cpp_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return record_gen::gen_record_update_expr_cpp(type_name, base_expression, overrides, context, gen_stmts, eval_expr_cpp);}

std::shared_ptr<cpp_ast::CppExpression> gen_tuple_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("std::make_tuple")), elements.map([context, gen_stmts](std::shared_ptr<semantic_ir::SemanticExpression> element) mutable { return eval_expr_cpp(element, context, gen_stmts); })));}

std::shared_ptr<cpp_ast::CppExpression> gen_question_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppQuestionTry(eval_expr_cpp(inner_expression, context, gen_stmts)));}

std::shared_ptr<cpp_ast::CppExpression> gen_with_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppWithBlock(eval_expr_cpp(resource, context, gen_stmts), binder, gen_stmts(statements, context)));}

std::shared_ptr<cpp_ast::CppExpression> gen_binary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
mlc::String method = semantic_type_structure::operator_method_for(operation);
std::shared_ptr<registry::Type> left_type = semantic_ir::sexpr_type(left_expression);
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*left_type))) { auto _v_tnamed = std::get<registry::TNamed>((*left_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return method != mlc::String("") && type_name != mlc::String("") ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(type_name + mlc::String("_") + method), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{evaluate_expression(left_expression, context, gen_stmts), evaluate_expression(right_expression, context, gen_stmts)})) : std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(operation, evaluate_expression(left_expression, context, gen_stmts), evaluate_expression(right_expression, context, gen_stmts)));
}

std::shared_ptr<cpp_ast::CppExpression> gen_unary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppUnary(operation, evaluate_expression(inner_expression, context, gen_stmts)));}

std::shared_ptr<cpp_ast::CppExpression> dispatch_codegen_pass(expr_visitor_cpp::CodegenPass codegen_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppExpression> { if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*expression)._)) { auto _v_semanticexpressionint = std::get<semantic_ir::SemanticExpressionInt>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressionint; return CodegenPass_visit_int(codegen_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*expression)._)) { auto _v_semanticexpressionstr = std::get<semantic_ir::SemanticExpressionStr>((*expression)._); auto [string_value, type_value, _w0] = _v_semanticexpressionstr; return CodegenPass_visit_str(codegen_pass, string_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*expression)._)) { auto _v_semanticexpressionbool = std::get<semantic_ir::SemanticExpressionBool>((*expression)._); auto [boolean_value, type_value, _w0] = _v_semanticexpressionbool; return CodegenPass_visit_bool(codegen_pass, boolean_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionunit; return CodegenPass_visit_unit(codegen_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFloat>((*expression)._)) { auto _v_semanticexpressionfloat = std::get<semantic_ir::SemanticExpressionFloat>((*expression)._); auto [float_value, type_value, _w0] = _v_semanticexpressionfloat; return CodegenPass_visit_float(codegen_pass, float_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionI64>((*expression)._)) { auto _v_semanticexpressioni64 = std::get<semantic_ir::SemanticExpressionI64>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressioni64; return CodegenPass_visit_i64(codegen_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionU8>((*expression)._)) { auto _v_semanticexpressionu8 = std::get<semantic_ir::SemanticExpressionU8>((*expression)._); auto [byte_value, type_value, _w0] = _v_semanticexpressionu8; return CodegenPass_visit_u8(codegen_pass, byte_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUsize>((*expression)._)) { auto _v_semanticexpressionusize = std::get<semantic_ir::SemanticExpressionUsize>((*expression)._); auto [size_value, type_value, _w0] = _v_semanticexpressionusize; return CodegenPass_visit_usize(codegen_pass, size_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionChar>((*expression)._)) { auto _v_semanticexpressionchar = std::get<semantic_ir::SemanticExpressionChar>((*expression)._); auto [character_value, type_value, _w0] = _v_semanticexpressionchar; return CodegenPass_visit_char(codegen_pass, character_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*expression)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionextern; return CodegenPass_visit_extern(codegen_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name_value, type_value, _w0] = _v_semanticexpressionident; return CodegenPass_visit_ident(codegen_pass, name_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, left_expression, right_expression, type_value, _w0] = _v_semanticexpressionbin; return CodegenPass_visit_bin(codegen_pass, operation, left_expression, right_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*expression)._)) { auto _v_semanticexpressionun = std::get<semantic_ir::SemanticExpressionUn>((*expression)._); auto [operation, inner_expression, type_value, _w0] = _v_semanticexpressionun; return CodegenPass_visit_un(codegen_pass, operation, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [function_expression, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressioncall; return CodegenPass_visit_call(codegen_pass, function_expression, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [object, method_name, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressionmethod; return CodegenPass_visit_method(codegen_pass, object, method_name, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*expression)._)) { auto _v_semanticexpressionfield = std::get<semantic_ir::SemanticExpressionField>((*expression)._); auto [object, field_name, type_value, _w0] = _v_semanticexpressionfield; return CodegenPass_visit_field(codegen_pass, object, field_name, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [object, index_expression, type_value, _w0] = _v_semanticexpressionindex; return CodegenPass_visit_index(codegen_pass, object, index_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, type_value, _w0] = _v_semanticexpressionif; return CodegenPass_visit_if(codegen_pass, condition, then_expression, else_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, type_value, _w0] = _v_semanticexpressionblock; return CodegenPass_visit_block(codegen_pass, statements, result_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, statements, type_value, _w0] = _v_semanticexpressionwhile; return CodegenPass_visit_while(codegen_pass, condition, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*expression)._)) { auto _v_semanticexpressionfor = std::get<semantic_ir::SemanticExpressionFor>((*expression)._); auto [variable, iterator, statements, type_value, _w0] = _v_semanticexpressionfor; return CodegenPass_visit_for(codegen_pass, variable, iterator, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*expression)._); auto [subject, arms, type_value, _w0] = _v_semanticexpressionmatch; return CodegenPass_visit_match(codegen_pass, subject, arms, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*expression)._)) { auto _v_semanticexpressionrecord = std::get<semantic_ir::SemanticExpressionRecord>((*expression)._); auto [type_name, field_values, type_value, _w0] = _v_semanticexpressionrecord; return CodegenPass_visit_record(codegen_pass, type_name, field_values, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._)) { auto _v_semanticexpressionrecordupdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._); auto [type_name, base_expression, overrides, type_value, _w0] = _v_semanticexpressionrecordupdate; return CodegenPass_visit_record_update(codegen_pass, type_name, base_expression, overrides, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressionarray; return CodegenPass_visit_array(codegen_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*expression)._)) { auto _v_semanticexpressiontuple = std::get<semantic_ir::SemanticExpressionTuple>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressiontuple; return CodegenPass_visit_tuple(codegen_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner_expression, type_value, _w0] = _v_semanticexpressionquestion; return CodegenPass_visit_question(codegen_pass, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*expression)._)) { auto _v_semanticexpressionlambda = std::get<semantic_ir::SemanticExpressionLambda>((*expression)._); auto [parameter_names, body_expression, type_value, _w0] = _v_semanticexpressionlambda; return CodegenPass_visit_lambda(codegen_pass, parameter_names, body_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*expression)._)) { auto _v_semanticexpressionwith = std::get<semantic_ir::SemanticExpressionWith>((*expression)._); auto [resource, binder, statements, type_value, _w0] = _v_semanticexpressionwith; return CodegenPass_visit_with(codegen_pass, resource, binder, statements, type_value); } return CodegenPass_visit_unsupported(codegen_pass, expression); }();}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_int(expr_visitor_cpp::CodegenPass self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_integer_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_str(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_string_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bool(expr_visitor_cpp::CodegenPass self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_boolean_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unit(expr_visitor_cpp::CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_unit_literal_cpp();}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_float(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_float_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_i64(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_i64_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_u8(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_u8_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_usize(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_usize_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_char(expr_visitor_cpp::CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_char_literal_cpp(value);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_extern(expr_visitor_cpp::CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return literals::gen_extern_placeholder_cpp();}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_ident(expr_visitor_cpp::CodegenPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return identifiers::gen_identifier_cpp(name, self.context);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bin(expr_visitor_cpp::CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_binary_via_cpp_visitor(operation, left_expression, right_expression, self.context, self.gen_stmts, self.evaluate_expression);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_un(expr_visitor_cpp::CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_unary_via_cpp_visitor(operation, inner_expression, self.context, self.gen_stmts, self.evaluate_expression);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_call(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_call_via_cpp_visitor(function_expression, arguments, mutability_flags, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_method(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_method_via_cpp_visitor(object, method_name, arguments, mutability_flags, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_field(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_field_via_cpp_visitor(object, field_name, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_index(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_index_via_cpp_visitor(object, index_expression, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_if(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_if_via_cpp_visitor(condition, then_expression, else_expression, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_block(expr_visitor_cpp::CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_block_via_cpp_visitor(statements, result_expression, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_while(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_while_via_cpp_visitor(condition, statements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_for(expr_visitor_cpp::CodegenPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_for_via_cpp_visitor(variable, iterator, statements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_match(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_match_via_cpp_visitor(subject, arms, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record(expr_visitor_cpp::CodegenPass self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_record_via_cpp_visitor(type_name, field_values, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record_update(expr_visitor_cpp::CodegenPass self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_record_update_via_cpp_visitor(type_name, base_expression, overrides, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_array(expr_visitor_cpp::CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_array_via_cpp_visitor(elements, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_tuple(expr_visitor_cpp::CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_tuple_via_cpp_visitor(elements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_question(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_question_via_cpp_visitor(inner_expression, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_lambda(expr_visitor_cpp::CodegenPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return gen_lambda_via_cpp_visitor(parameter_names, body_expression, semantic_type, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_with(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return gen_with_via_cpp_visitor(resource, binder, statements, self.context, self.gen_stmts);}

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unsupported(expr_visitor_cpp::CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept{return unsupported_cpp_expression();}

std::shared_ptr<cpp_ast::CppExpression> eval_expr_with_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{return dispatch_codegen_pass(codegen_pass_new(context, gen_stmts, evaluate_expression), expression);}

std::shared_ptr<cpp_ast::CppExpression> gen_expr_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return eval_expr_with_cpp_visitor(expression, context, ignored_gen_stmts, evaluate_via_cpp_visitor);}

} // namespace expr_visitor_cpp
