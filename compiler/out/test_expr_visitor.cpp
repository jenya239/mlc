#include "test_expr_visitor.hpp"

#include "test_runner.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"

namespace test_expr_visitor {

using namespace test_runner;
using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace expr_visitor;
using namespace ast_tokens;

struct TagExprVisitor {int seed;};

std::shared_ptr<registry::Type> unknown_type() noexcept;

std::shared_ptr<registry::Type> integer_type() noexcept;

std::shared_ptr<registry::Type> string_type() noexcept;

std::shared_ptr<registry::Type> unit_type() noexcept;

int TagExprVisitor_visit_int(test_expr_visitor::TagExprVisitor self, int integer_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_str(test_expr_visitor::TagExprVisitor self, mlc::String string_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_bool(test_expr_visitor::TagExprVisitor self, bool boolean_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_unit(test_expr_visitor::TagExprVisitor self, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_float(test_expr_visitor::TagExprVisitor self, mlc::String float_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_i64(test_expr_visitor::TagExprVisitor self, mlc::String integer_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_u8(test_expr_visitor::TagExprVisitor self, mlc::String byte_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_usize(test_expr_visitor::TagExprVisitor self, mlc::String size_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_char(test_expr_visitor::TagExprVisitor self, mlc::String character_value, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_extern(test_expr_visitor::TagExprVisitor self, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_ident(test_expr_visitor::TagExprVisitor self, mlc::String name, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_bin(test_expr_visitor::TagExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_un(test_expr_visitor::TagExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_call(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_method(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_field(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_index(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_if(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_block(test_expr_visitor::TagExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_while(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_for(test_expr_visitor::TagExprVisitor self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_match(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_record(test_expr_visitor::TagExprVisitor self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_record_update(test_expr_visitor::TagExprVisitor self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_array(test_expr_visitor::TagExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_tuple(test_expr_visitor::TagExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_question(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_lambda(test_expr_visitor::TagExprVisitor self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_with(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> semantic_type) noexcept;

int TagExprVisitor_visit_unsupported(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

mlc::Array<test_runner::TestResult> expr_visitor_tests() noexcept;

std::shared_ptr<registry::Type> unknown_type() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

std::shared_ptr<registry::Type> integer_type() noexcept{return std::make_shared<registry::Type>((registry::TI32{}));}

std::shared_ptr<registry::Type> string_type() noexcept{return std::make_shared<registry::Type>((registry::TString{}));}

std::shared_ptr<registry::Type> unit_type() noexcept{return std::make_shared<registry::Type>((registry::TUnit{}));}

int TagExprVisitor_visit_int(test_expr_visitor::TagExprVisitor self, int integer_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 1;}

int TagExprVisitor_visit_str(test_expr_visitor::TagExprVisitor self, mlc::String string_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 2;}

int TagExprVisitor_visit_bool(test_expr_visitor::TagExprVisitor self, bool boolean_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 3;}

int TagExprVisitor_visit_unit(test_expr_visitor::TagExprVisitor self, std::shared_ptr<registry::Type> semantic_type) noexcept{return 4;}

int TagExprVisitor_visit_float(test_expr_visitor::TagExprVisitor self, mlc::String float_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 5;}

int TagExprVisitor_visit_i64(test_expr_visitor::TagExprVisitor self, mlc::String integer_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 6;}

int TagExprVisitor_visit_u8(test_expr_visitor::TagExprVisitor self, mlc::String byte_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 7;}

int TagExprVisitor_visit_usize(test_expr_visitor::TagExprVisitor self, mlc::String size_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 8;}

int TagExprVisitor_visit_char(test_expr_visitor::TagExprVisitor self, mlc::String character_value, std::shared_ptr<registry::Type> semantic_type) noexcept{return 9;}

int TagExprVisitor_visit_extern(test_expr_visitor::TagExprVisitor self, std::shared_ptr<registry::Type> semantic_type) noexcept{return 10;}

int TagExprVisitor_visit_ident(test_expr_visitor::TagExprVisitor self, mlc::String name, std::shared_ptr<registry::Type> semantic_type) noexcept{return 11;}

int TagExprVisitor_visit_bin(test_expr_visitor::TagExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return 12;}

int TagExprVisitor_visit_un(test_expr_visitor::TagExprVisitor self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return 13;}

int TagExprVisitor_visit_call(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept{return 14;}

int TagExprVisitor_visit_method(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept{return 15;}

int TagExprVisitor_visit_field(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> semantic_type) noexcept{return 16;}

int TagExprVisitor_visit_index(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return 17;}

int TagExprVisitor_visit_if(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return 18;}

int TagExprVisitor_visit_block(test_expr_visitor::TagExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return 19;}

int TagExprVisitor_visit_while(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> semantic_type) noexcept{return 20;}

int TagExprVisitor_visit_for(test_expr_visitor::TagExprVisitor self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> semantic_type) noexcept{return 21;}

int TagExprVisitor_visit_match(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept{return 22;}

int TagExprVisitor_visit_record(test_expr_visitor::TagExprVisitor self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept{return 23;}

int TagExprVisitor_visit_record_update(test_expr_visitor::TagExprVisitor self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> semantic_type) noexcept{return 24;}

int TagExprVisitor_visit_array(test_expr_visitor::TagExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept{return 25;}

int TagExprVisitor_visit_tuple(test_expr_visitor::TagExprVisitor self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept{return 26;}

int TagExprVisitor_visit_question(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return 27;}

int TagExprVisitor_visit_lambda(test_expr_visitor::TagExprVisitor self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{return 28;}

int TagExprVisitor_visit_with(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> semantic_type) noexcept{return 29;}

int TagExprVisitor_visit_unsupported(test_expr_visitor::TagExprVisitor self, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return 0;}

mlc::Array<test_runner::TestResult> expr_visitor_tests() noexcept{
test_expr_visitor::TagExprVisitor tag_state = test_expr_visitor::TagExprVisitor{0};
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("ExprVisitor stub: visit_int"), TagExprVisitor_visit_int(tag_state, 7, integer_type()), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("ExprVisitor stub: visit_str"), TagExprVisitor_visit_str(tag_state, mlc::String("text"), string_type()), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("ExprVisitor stub: visit_ident"), TagExprVisitor_visit_ident(tag_state, mlc::String("value"), unknown_type()), 11));
results.push_back(test_runner::assert_eq_int(mlc::String("ExprVisitor stub: visit_unit"), TagExprVisitor_visit_unit(tag_state, unit_type()), 4));
results.push_back(test_runner::assert_eq_int(mlc::String("ExprVisitor stub: visit_extern"), TagExprVisitor_visit_extern(tag_state, unit_type()), 10));
std::shared_ptr<semantic_ir::SemanticExpression> binary_expression = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(1, integer_type(), ast::span_unknown())), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(2, integer_type(), ast::span_unknown())), integer_type(), ast::span_unknown()));
results.push_back(test_runner::assert_eq_int(mlc::String("ExprVisitor stub: visit_bin"), TagExprVisitor_visit_bin(tag_state, mlc::String("+"), binary_expression, binary_expression, integer_type()), 12));
return results;
}

} // namespace test_expr_visitor
