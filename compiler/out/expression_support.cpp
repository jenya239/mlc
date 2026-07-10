#define main mlc_user_main
#include "expression_support.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "result_option_method_types.hpp"
#include "semantic_type_structure.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"

namespace expression_support {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace result_option_method_types;
using namespace semantic_type_structure;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::String named_or_described_error_type_name(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
return semantic_type_structure::type_description(type_value);
std::abort();
}();
}
mlc::String question_from_converter_name(context::CodegenContext context, std::shared_ptr<registry::Type> inner_result_type) noexcept{
  if ((!result_option_method_types::is_result_generic(inner_result_type)))   {
    return mlc::String("", 0);
  }
  if ((!result_option_method_types::is_result_generic(context.enclosing_function_return_type)))   {
    return mlc::String("", 0);
  }
  auto inner_error_type = result_option_method_types::result_err_type(inner_result_type);
  auto expected_error_type = result_option_method_types::result_err_type(context.enclosing_function_return_type);
  if ((semantic_type_structure::type_is_unknown(inner_error_type) || semantic_type_structure::type_is_unknown(expected_error_type)))   {
    return mlc::String("", 0);
  }
  if (semantic_type_structure::types_structurally_equal(inner_error_type, expected_error_type))   {
    return mlc::String("", 0);
  }
  auto expected_error_name = named_or_described_error_type_name(expected_error_type);
  if ((expected_error_name.length() == 0))   {
    return mlc::String("", 0);
  }
  return cpp_naming::cpp_safe((expected_error_name + mlc::String("_from", 5)));
}
mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept{
  auto capture = mlc::String("[=]", 3);
  auto parameter_list = [&]() -> mlc::String {
  if ((parameters.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < parameters.length())) {
  parts.push_back((mlc::String("auto ", 5) + cpp_naming::cpp_safe(parameters[index])));
  (index = mlc::arith::checked_add(index, 1));
}
return parts.join(mlc::String(", ", 2));
}();
  }
}();
  return (((capture + mlc::String("(", 1)) + parameter_list) + mlc::String(") mutable", 9));
}
std::shared_ptr<semantic_ir::SemanticExpression> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String field_name) noexcept{
  auto fallback_span = ((field_values.length() > 0) ? (semantic_ir::sexpr_span(field_values[0]->value)) : (ast::span_unknown()));
  auto result = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit{std::make_shared<registry::Type>(registry::TUnit{}), fallback_span});
  auto index = 0;
  while ((index < field_values.length()))   {
    if ((field_values[index]->name == field_name))     {
      (result = field_values[index]->value);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
bool is_constructor_call(std::shared_ptr<semantic_ir::SemanticExpression> function_expr) noexcept{
  return [&]() -> bool {
auto __match_subject = function_expr;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return ((((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1))) && (!name.contains(mlc::String("_", 1))));
}
return false;
std::abort();
}();
}
mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept{
  auto self_fields = context::lookup_fields_for_context(context, context.self_type);
  auto is_known_field = (((((((object_name == mlc::String("errors", 6)) || (object_name == mlc::String("kind", 4))) || (object_name == mlc::String("tokens", 6))) || (object_name == mlc::String("line", 4))) || (object_name == mlc::String("col", 3))) || (object_name == mlc::String("inferred_type", 13))) || (object_name == mlc::String("type_env", 8)));
  if ((decl_index::list_contains(self_fields, object_name) || is_known_field))   {
    return (mlc::String("self.", 5) + cpp_naming::cpp_safe(object_name));
  } else   {
    return context::CodegenContext_resolve(context, cpp_naming::map_builtin_identifier_reference(object_name));
  }
}
mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context) noexcept{
  auto type_name = [&]() -> mlc::String {
auto __match_subject = semantic_ir::sexpr_type(argument);
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [inner] = tShared; return [&]() -> mlc::String {
auto __match_subject = inner;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
return mlc::String("", 0);
std::abort();
}();
  if ((type_name.length() > 0))   {
    return context::CodegenContext_resolve(context, type_name);
  } else   {
    return mlc::String("auto", 4);
  }
}
mlc::String semantic_type_to_cpp_type_name(std::shared_ptr<registry::Type> type_value, context::CodegenContext context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return mlc::String("int", 3);
}
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return mlc::String("mlc::String", 11);
}
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return mlc::String("bool", 4);
}
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return mlc::String("void", 4);
}
if (std::holds_alternative<registry::TI64>((*__match_subject))) {
const registry::TI64& tI64 = std::get<registry::TI64>((*__match_subject));
return mlc::String("int64_t", 7);
}
if (std::holds_alternative<registry::TF64>((*__match_subject))) {
const registry::TF64& tF64 = std::get<registry::TF64>((*__match_subject));
return mlc::String("double", 6);
}
if (std::holds_alternative<registry::TU8>((*__match_subject))) {
const registry::TU8& tU8 = std::get<registry::TU8>((*__match_subject));
return mlc::String("uint8_t", 7);
}
if (std::holds_alternative<registry::TUsize>((*__match_subject))) {
const registry::TUsize& tUsize = std::get<registry::TUsize>((*__match_subject));
return mlc::String("size_t", 6);
}
if (std::holds_alternative<registry::TChar>((*__match_subject))) {
const registry::TChar& tChar = std::get<registry::TChar>((*__match_subject));
return mlc::String("char32_t", 8);
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return context::CodegenContext_resolve(context, name);
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [inner] = tShared; return semantic_type_to_cpp_type_name(inner, context);
}
return mlc::String("auto", 4);
std::abort();
}();
}
mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept{
  if ((method_name == mlc::String("read", 4)))   {
    return mlc::String("mlc::file::read_to_string", 25);
  } else if ((method_name == mlc::String("write", 5)))   {
    return mlc::String("mlc::file::write_string", 23);
  } else if ((method_name == mlc::String("make_temp_directory", 19)))   {
    return mlc::String("mlc::file::make_temp_directory", 30);
  } else if ((method_name == mlc::String("temp_directory_base", 19)))   {
    return mlc::String("mlc::file::temp_directory_base", 30);
  } else   {
    return (mlc::String("mlc::file::", 11) + method_name);
  }
}
mlc::String cpp_function_name_for_profile_method(mlc::String method_name) noexcept{
  if ((method_name == mlc::String("scope_begin", 11)))   {
    return mlc::String("mlc::profile::scope_begin", 25);
  } else if ((method_name == mlc::String("scope_end", 9)))   {
    return mlc::String("mlc::profile::scope_end", 23);
  } else if ((method_name == mlc::String("monotonic_nanos", 15)))   {
    return mlc::String("mlc::profile::monotonic_nanos", 29);
  } else if ((method_name == mlc::String("peak_rss_kib", 12)))   {
    return mlc::String("mlc::profile::peak_rss_kib", 26);
  } else if ((method_name == mlc::String("print_report", 12)))   {
    return mlc::String("mlc::profile::print_report", 26);
  } else if ((method_name == mlc::String("reset", 5)))   {
    return mlc::String("mlc::profile::reset", 19);
  } else   {
    return (mlc::String("mlc::profile::", 14) + method_name);
  }
}
mlc::String field_access_operator(std::shared_ptr<semantic_ir::SemanticExpression> object, context::CodegenContext context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = semantic_ir::sexpr_type(object);
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [__0] = tShared; return mlc::String("->", 2);
}
return mlc::String(".", 1);
std::abort();
}();
}

} // namespace expression_support
