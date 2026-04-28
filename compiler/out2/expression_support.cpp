#include "expression_support.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"

namespace expression_support {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept;

std::shared_ptr<semantic_ir::SExpr> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::String field_name) noexcept;

bool is_constructor_call(std::shared_ptr<semantic_ir::SExpr> function_expr) noexcept;

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept;

mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context) noexcept;

mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept;

mlc::String field_access_operator(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context) noexcept;

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
return capture + mlc::String("(") + parameter_list + mlc::String(")");
}

std::shared_ptr<semantic_ir::SExpr> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::String field_name) noexcept{
std::shared_ptr<semantic_ir::SExpr> result = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), ast::span_unknown()));
int index = 0;
while (index < field_values.size()){
{
if (field_values[index]->name == field_name){
{
result = field_values[index]->val;
}
}
index = index + 1;
}
}
return result;
}

bool is_constructor_call(std::shared_ptr<semantic_ir::SExpr> function_expr) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SExprIdent>((*function_expr)._)) { auto _v_sexprident = std::get<semantic_ir::SExprIdent>((*function_expr)._); auto [name, _w0, _w1] = _v_sexprident; return name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z"); } return false; }();}

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> self_fields = decl_index::lookup_fields(context.field_orders, context.self_type);
bool is_known_field = object_name == mlc::String("errors") || object_name == mlc::String("kind") || object_name == mlc::String("tokens") || object_name == mlc::String("line") || object_name == mlc::String("col") || object_name == mlc::String("inferred_type") || object_name == mlc::String("type_env");
return decl_index::list_contains(self_fields, object_name) || is_known_field ? mlc::String("self.") + cpp_naming::cpp_safe(object_name) : context::context_resolve(context, cpp_naming::map_builtin(object_name));
}

mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context) noexcept{
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*semantic_ir::sexpr_type(argument)))) { auto _v_tnamed = std::get<registry::TNamed>((*semantic_ir::sexpr_type(argument))); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TShared>((*semantic_ir::sexpr_type(argument)))) { auto _v_tshared = std::get<registry::TShared>((*semantic_ir::sexpr_type(argument))); auto [inner] = _v_tshared; return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner))) { auto _v_tnamed = std::get<registry::TNamed>((*inner)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }(); } return mlc::String(""); }();
return type_name.length() > 0 ? context::context_resolve(context, type_name) : mlc::String("auto");
}

mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept{return method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : method_name == mlc::String("write") ? mlc::String("mlc::file::write_string") : mlc::String("mlc::file::") + method_name;}

mlc::String field_access_operator(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TShared>((*semantic_ir::sexpr_type(object)))) { auto _v_tshared = std::get<registry::TShared>((*semantic_ir::sexpr_type(object))); auto [_w0] = _v_tshared; return mlc::String("->"); } return mlc::String("."); }();}

} // namespace expression_support
