#include "expression_support.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"

namespace expression_support {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String field_name) noexcept;

bool is_constructor_call(std::shared_ptr<semantic_ir::SemanticExpression> function_expr) noexcept;

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept;

mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context) noexcept;

mlc::String semantic_type_to_cpp_type_name(std::shared_ptr<registry::Type> type_value, context::CodegenContext context) noexcept;

mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept;

mlc::String cpp_function_name_for_profile_method(mlc::String method_name) noexcept;

mlc::String field_access_operator(std::shared_ptr<semantic_ir::SemanticExpression> object, context::CodegenContext context) noexcept;

mlc::String generate_conditional_else_with_empty_array_coercion(std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> preferred_array_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> evaluate_expression) noexcept;

mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept{
mlc::String capture = parameters.size() == 0 ? mlc::String("[]") : mlc::String("[=]");
mlc::String parameter_list = parameters.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < parameters.size()){
{
parts.push_back(mlc::String("auto ") + cpp_naming::cpp_safe(parameters[index]));
index = index + 1;
}
}
  return parts.join(mlc::String(", "));
 }();
return capture + mlc::String("(") + parameter_list + mlc::String(") mutable");
}

std::shared_ptr<semantic_ir::SemanticExpression> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String field_name) noexcept{
ast::Span fallback_span = field_values.size() > 0 ? semantic_ir::sexpr_span(field_values[0]->value) : ast::span_unknown();
std::shared_ptr<semantic_ir::SemanticExpression> result = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit(std::make_shared<registry::Type>((registry::TUnit{})), fallback_span));
int index = 0;
while (index < field_values.size()){
{
if (field_values[index]->name == field_name){
{
result = field_values[index]->value;
}
}
index = index + 1;
}
}
return result;
}

bool is_constructor_call(std::shared_ptr<semantic_ir::SemanticExpression> function_expr) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*function_expr)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*function_expr)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z") && !name.contains(mlc::String("_")); } return false; }();}

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> self_fields = context::lookup_fields_for_context(context, context.self_type);
bool is_known_field = object_name == mlc::String("errors") || object_name == mlc::String("kind") || object_name == mlc::String("tokens") || object_name == mlc::String("line") || object_name == mlc::String("col") || object_name == mlc::String("inferred_type") || object_name == mlc::String("type_env");
return decl_index::list_contains(self_fields, object_name) || is_known_field ? mlc::String("self.") + cpp_naming::cpp_safe(object_name) : context::CodegenContext_resolve(context, cpp_naming::map_builtin_identifier_reference(object_name));
}

mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context) noexcept{
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*semantic_ir::sexpr_type(argument)))) { auto _v_tnamed = std::get<registry::TNamed>((*semantic_ir::sexpr_type(argument))); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TShared>((*semantic_ir::sexpr_type(argument)))) { auto _v_tshared = std::get<registry::TShared>((*semantic_ir::sexpr_type(argument))); auto [inner] = _v_tshared; return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner))) { auto _v_tnamed = std::get<registry::TNamed>((*inner)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }(); } return mlc::String(""); }();
return type_name.length() > 0 ? context::CodegenContext_resolve(context, type_name) : mlc::String("auto");
}

mlc::String semantic_type_to_cpp_type_name(std::shared_ptr<registry::Type> type_value, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TI32>((*type_value))) {  return mlc::String("int"); } if (std::holds_alternative<registry::TString>((*type_value))) {  return mlc::String("mlc::String"); } if (std::holds_alternative<registry::TBool>((*type_value))) {  return mlc::String("bool"); } if (std::holds_alternative<registry::TUnit>((*type_value))) {  return mlc::String("void"); } if (std::holds_alternative<registry::TI64>((*type_value))) {  return mlc::String("int64_t"); } if (std::holds_alternative<registry::TF64>((*type_value))) {  return mlc::String("double"); } if (std::holds_alternative<registry::TU8>((*type_value))) {  return mlc::String("uint8_t"); } if (std::holds_alternative<registry::TUsize>((*type_value))) {  return mlc::String("size_t"); } if (std::holds_alternative<registry::TChar>((*type_value))) {  return mlc::String("char32_t"); } if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return context::CodegenContext_resolve(context, name); } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return semantic_type_to_cpp_type_name(inner, context); } return mlc::String("auto"); }();}

mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept{return [&]() -> mlc::String { if (method_name == mlc::String("read")) { return mlc::String("mlc::file::read_to_string"); } if (method_name == mlc::String("write")) { return mlc::String("mlc::file::write_string"); } if (method_name == mlc::String("make_temp_directory")) { return mlc::String("mlc::file::make_temp_directory"); } if (method_name == mlc::String("temp_directory_base")) { return mlc::String("mlc::file::temp_directory_base"); } return mlc::String("mlc::file::") + method_name; }();}

mlc::String cpp_function_name_for_profile_method(mlc::String method_name) noexcept{return [&]() -> mlc::String { if (method_name == mlc::String("scope_begin")) { return mlc::String("mlc::profile::scope_begin"); } if (method_name == mlc::String("scope_end")) { return mlc::String("mlc::profile::scope_end"); } if (method_name == mlc::String("monotonic_nanos")) { return mlc::String("mlc::profile::monotonic_nanos"); } if (method_name == mlc::String("peak_rss_kib")) { return mlc::String("mlc::profile::peak_rss_kib"); } if (method_name == mlc::String("print_report")) { return mlc::String("mlc::profile::print_report"); } if (method_name == mlc::String("reset")) { return mlc::String("mlc::profile::reset"); } return mlc::String("mlc::profile::") + method_name; }();}

mlc::String field_access_operator(std::shared_ptr<semantic_ir::SemanticExpression> object, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TShared>((*semantic_ir::sexpr_type(object)))) { auto _v_tshared = std::get<registry::TShared>((*semantic_ir::sexpr_type(object))); auto [_w0] = _v_tshared; return mlc::String("->"); } return mlc::String("."); }();}

mlc::String generate_conditional_else_with_empty_array_coercion(std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> preferred_array_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> evaluate_expression) noexcept{
return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*else_expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*else_expression)._); auto [elements, _w0, array_span] = _v_semanticexpressionarray; return elements.size() > 0 ? evaluate_expression(else_expression, context) : [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*preferred_array_semantic_type))) { auto _v_tarray = std::get<registry::TArray>((*preferred_array_semantic_type)); auto [inner_from_preferred] = _v_tarray; return evaluate_expression(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(elements, std::make_shared<registry::Type>(registry::TArray(inner_from_preferred)), array_span)), context); } return [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(then_expression)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(then_expression))); auto [inner_then] = _v_tarray; return evaluate_expression(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(elements, std::make_shared<registry::Type>(registry::TArray(inner_then)), array_span)), context); } return evaluate_expression(else_expression, context); }(); }(); } return evaluate_expression(else_expression, context); }();
}

} // namespace expression_support
