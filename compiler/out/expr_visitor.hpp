#ifndef EXPR_VISITOR_HPP
#define EXPR_VISITOR_HPP

#include "mlc.hpp"
#include <variant>
#include "registry.hpp"
#include "semantic_ir.hpp"
namespace expr_visitor {

template<typename VisitorResult, typename Self>
concept ExprVisitor = requires(const Self& self) { { ExprVisitor_visit_int(self, std::declval<int>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_str(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_bool(self, std::declval<bool>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_unit(self, std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_float(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_i64(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_u8(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_usize(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_char(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_extern(self, std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_ident(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_bin(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_un(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_call(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>>(), std::declval<mlc::Array<int>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_method(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::String>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>>(), std::declval<mlc::Array<int>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_field(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::String>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_index(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_if(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_block(self, std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_while(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_for(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_match(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_record(self, std::declval<mlc::String>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_record_update(self, std::declval<mlc::String>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_array(self, std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_tuple(self, std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_question(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_lambda(self, std::declval<mlc::Array<mlc::String>>(), std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_with(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>(), std::declval<mlc::String>(), std::declval<mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>>(), std::declval<std::shared_ptr<registry::Type>>()) } -> std::convertible_to<VisitorResult>; { ExprVisitor_visit_unsupported(self, std::declval<std::shared_ptr<semantic_ir::SemanticExpression>>()) } -> std::convertible_to<VisitorResult>; };
template<typename VisitorResult, typename Visitor>
requires ExprVisitor<VisitorResult, Visitor>
VisitorResult dispatch_expr(Visitor expression_visitor, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
template<typename VisitorResult, typename Visitor>
requires ExprVisitor<VisitorResult, Visitor>
VisitorResult dispatch_expr(Visitor expression_visitor, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> VisitorResult {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*__match_subject))) {
const semantic_ir::SemanticExpressionInt& semanticExpressionInt = std::get<semantic_ir::SemanticExpressionInt>((*__match_subject));
auto [integer_value, type_value, __2] = semanticExpressionInt; return expression_visitor.visit_int(integer_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*__match_subject))) {
const semantic_ir::SemanticExpressionStr& semanticExpressionStr = std::get<semantic_ir::SemanticExpressionStr>((*__match_subject));
auto [string_value, type_value, __2] = semanticExpressionStr; return expression_visitor.visit_str(string_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*__match_subject))) {
const semantic_ir::SemanticExpressionBool& semanticExpressionBool = std::get<semantic_ir::SemanticExpressionBool>((*__match_subject));
auto [boolean_value, type_value, __2] = semanticExpressionBool; return expression_visitor.visit_bool(boolean_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [type_value, __1] = semanticExpressionUnit; return expression_visitor.visit_unit(type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFloat>((*__match_subject))) {
const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat = std::get<semantic_ir::SemanticExpressionFloat>((*__match_subject));
auto [float_value, type_value, __2] = semanticExpressionFloat; return expression_visitor.visit_float(float_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionI64>((*__match_subject))) {
const semantic_ir::SemanticExpressionI64& semanticExpressionI64 = std::get<semantic_ir::SemanticExpressionI64>((*__match_subject));
auto [integer_value, type_value, __2] = semanticExpressionI64; return expression_visitor.visit_i64(integer_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionU8>((*__match_subject))) {
const semantic_ir::SemanticExpressionU8& semanticExpressionU8 = std::get<semantic_ir::SemanticExpressionU8>((*__match_subject));
auto [byte_value, type_value, __2] = semanticExpressionU8; return expression_visitor.visit_u8(byte_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUsize>((*__match_subject))) {
const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize = std::get<semantic_ir::SemanticExpressionUsize>((*__match_subject));
auto [size_value, type_value, __2] = semanticExpressionUsize; return expression_visitor.visit_usize(size_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionChar>((*__match_subject))) {
const semantic_ir::SemanticExpressionChar& semanticExpressionChar = std::get<semantic_ir::SemanticExpressionChar>((*__match_subject));
auto [character_value, type_value, __2] = semanticExpressionChar; return expression_visitor.visit_char(character_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [type_value, __1, __2, __3, __4] = semanticExpressionExtern; return expression_visitor.visit_extern(type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name_value, type_value, __2] = semanticExpressionIdent; return expression_visitor.visit_ident(name_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*__match_subject))) {
const semantic_ir::SemanticExpressionBin& semanticExpressionBin = std::get<semantic_ir::SemanticExpressionBin>((*__match_subject));
auto [operation, left_expression, right_expression, type_value, __4] = semanticExpressionBin; return expression_visitor.visit_bin(operation, left_expression, right_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*__match_subject))) {
const semantic_ir::SemanticExpressionUn& semanticExpressionUn = std::get<semantic_ir::SemanticExpressionUn>((*__match_subject));
auto [operation, inner_expression, type_value, __3] = semanticExpressionUn; return expression_visitor.visit_un(operation, inner_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [function_expression, arguments, mutability_flags, type_value, __4] = semanticExpressionCall; return expression_visitor.visit_call(function_expression, arguments, mutability_flags, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [object, method_name, arguments, mutability_flags, type_value, __5] = semanticExpressionMethod; return expression_visitor.visit_method(object, method_name, arguments, mutability_flags, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [object, field_name, type_value, __3] = semanticExpressionField; return expression_visitor.visit_field(object, field_name, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [object, index_expression, type_value, __3] = semanticExpressionIndex; return expression_visitor.visit_index(object, index_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, type_value, __4] = semanticExpressionIf; return expression_visitor.visit_if(condition, then_expression, else_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, type_value, __3] = semanticExpressionBlock; return expression_visitor.visit_block(statements, result_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*__match_subject))) {
const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile = std::get<semantic_ir::SemanticExpressionWhile>((*__match_subject));
auto [condition, statements, type_value, __3] = semanticExpressionWhile; return expression_visitor.visit_while(condition, statements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*__match_subject))) {
const semantic_ir::SemanticExpressionFor& semanticExpressionFor = std::get<semantic_ir::SemanticExpressionFor>((*__match_subject));
auto [variable, iterator, statements, type_value, __4] = semanticExpressionFor; return expression_visitor.visit_for(variable, iterator, statements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*__match_subject))) {
const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch = std::get<semantic_ir::SemanticExpressionMatch>((*__match_subject));
auto [subject, arms, type_value, __3] = semanticExpressionMatch; return expression_visitor.visit_match(subject, arms, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord = std::get<semantic_ir::SemanticExpressionRecord>((*__match_subject));
auto [type_name, field_values, type_value, __3] = semanticExpressionRecord; return expression_visitor.visit_record(type_name, field_values, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject));
auto [type_name, base_expression, overrides, type_value, __4] = semanticExpressionRecordUpdate; return expression_visitor.visit_record_update(type_name, base_expression, overrides, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [elements, type_value, __2] = semanticExpressionArray; return expression_visitor.visit_array(elements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*__match_subject))) {
const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple = std::get<semantic_ir::SemanticExpressionTuple>((*__match_subject));
auto [elements, type_value, __2] = semanticExpressionTuple; return expression_visitor.visit_tuple(elements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [inner_expression, type_value, __2] = semanticExpressionQuestion; return expression_visitor.visit_question(inner_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [parameter_names, body_expression, type_value, __3] = semanticExpressionLambda; return expression_visitor.visit_lambda(parameter_names, body_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*__match_subject))) {
const semantic_ir::SemanticExpressionWith& semanticExpressionWith = std::get<semantic_ir::SemanticExpressionWith>((*__match_subject));
auto [resource, binder, statements, type_value, __4] = semanticExpressionWith; return expression_visitor.visit_with(resource, binder, statements, type_value);
}
return expression_visitor.visit_unsupported(expression);
std::abort();
}();
}

} // namespace expr_visitor

#endif // EXPR_VISITOR_HPP
