#include "expression_support.hpp"

#include "ast.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"

namespace expression_support {

using namespace ast;
using namespace context;
using namespace cpp_naming;
using namespace ast_tokens;

mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept;

std::shared_ptr<ast::Expr> find_field_value(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String field_name) noexcept;

bool is_constructor_call(std::shared_ptr<ast::Expr> function_expr) noexcept;

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept;

mlc::String infer_shared_new_type_name(std::shared_ptr<ast::Expr> argument, context::CodegenContext context) noexcept;

mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept;

mlc::String field_access_operator(std::shared_ptr<ast::Expr> object, context::CodegenContext context) noexcept;

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

std::shared_ptr<ast::Expr> find_field_value(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String field_name) noexcept{
std::shared_ptr<ast::Expr> result = std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()));
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

bool is_constructor_call(std::shared_ptr<ast::Expr> function_expr) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*function_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function_expr)._); auto [name, _w0] = _v_exprident; return name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z"); } return false; }();}

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> self_fields = decl_index::lookup_fields(context.field_orders, context.self_type);
bool is_known_field = object_name == mlc::String("errors") || object_name == mlc::String("kind") || object_name == mlc::String("tokens") || object_name == mlc::String("line") || object_name == mlc::String("col") || object_name == mlc::String("inferred_type") || object_name == mlc::String("type_env");
return decl_index::list_contains(self_fields, object_name) || is_known_field ? mlc::String("self.") + cpp_naming::cpp_safe(object_name) : context::context_resolve(context, cpp_naming::map_builtin(object_name));
}

mlc::String infer_shared_new_type_name(std::shared_ptr<ast::Expr> argument, context::CodegenContext context) noexcept{
mlc::String raw_type_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprRecord>((*argument)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*argument)._); auto [name, _w0, _w1] = _v_exprrecord; return name; } if (std::holds_alternative<ast::ExprCall>((*argument)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*argument)._); auto [function_expr, _w0, _w1] = _v_exprcall; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function_expr)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }(); } if (std::holds_alternative<ast::ExprIdent>((*argument)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*argument)._); auto [name, _w0] = _v_exprident; return name; } if (std::holds_alternative<ast::ExprIndex>((*argument)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*argument)._); auto [array_expr, _w0, _w1] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*array_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*array_expr)._); auto [array_name, _w0] = _v_exprident; return context.array_elem_types.has(array_name) ? context.array_elem_types.get(array_name) : mlc::String(""); } return mlc::String(""); }(); } return mlc::String(""); }();
mlc::String parent_type = raw_type_name.length() > 0 && context.variant_types.has(raw_type_name) ? context.variant_types.get(raw_type_name) : raw_type_name;
return parent_type.length() > 0 ? context::context_resolve(context, parent_type) : mlc::String("auto");
}

mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept{return method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : method_name == mlc::String("write") ? mlc::String("mlc::file::write_string") : mlc::String("mlc::file::") + method_name;}

mlc::String field_access_operator(std::shared_ptr<ast::Expr> object, context::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIndex>((*object)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*object)._); auto [inner_expr, _w0, _w1] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*inner_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*inner_expr)._); auto [array_name, _w0] = _v_exprident; return context.shared_array_params.contains(array_name) ? mlc::String("->") : mlc::String("."); } return mlc::String("."); }(); } if (std::holds_alternative<ast::ExprIdent>((*object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*object)._); auto [name, _w0] = _v_exprident; return context.shared_params.contains(name) ? mlc::String("->") : mlc::String("."); } if (std::holds_alternative<ast::ExprMethod>((*object)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*object)._); auto [map_object, method_name, _w0, _w1] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*map_object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_object)._); auto [map_name, _w0] = _v_exprident; return context.shared_map_params.contains(map_name) ? mlc::String("->") : mlc::String("."); } return mlc::String("."); }() : mlc::String("."); } return mlc::String("."); }();}

} // namespace expression_support
