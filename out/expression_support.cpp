#define main mlc_user_main
#include "expression_support.hpp"

namespace expression_support {

mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept{
auto capture = ((parameters.length() == 0) ? mlc::String("[]", 2) : mlc::String("[=]", 3));
auto parameter_list = ((parameters.length() == 0) ? mlc::String("", 0) : [&]() {
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < parameters.length())) {
parts.push_back((mlc::String("auto ", 5) + cpp_naming::cpp_safe(parameters[index])));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}());
return (((capture + mlc::String("(", 1)) + parameter_list) + mlc::String(")", 1));
}
std::shared_ptr<semantic_ir::SExpr> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::String field_name) noexcept{
auto result = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{std::make_shared<registry::Type>(registry::TUnit{}), ast::span_unknown()});
auto index = 0;
while ((index < field_values.length())) {
if ((field_values[index]->name == field_name)) {
result = field_values[index]->val;
}
index = (index + 1);
}
return result;
}
bool is_constructor_call(std::shared_ptr<semantic_ir::SExpr> function_expr) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprIdent& sExprIdent) { auto [name, __1, __2] = sExprIdent; return (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1))); },
[&](const auto& __v) { return false; }
}, (*function_expr));
}
mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept{
auto self_fields = decl_index::lookup_fields(context.field_orders, context.self_type);
auto is_known_field = (((((((object_name == mlc::String("errors", 6)) || (object_name == mlc::String("kind", 4))) || (object_name == mlc::String("tokens", 6))) || (object_name == mlc::String("line", 4))) || (object_name == mlc::String("col", 3))) || (object_name == mlc::String("inferred_type", 13))) || (object_name == mlc::String("type_env", 8)));
if ((decl_index::list_contains(self_fields, object_name) || is_known_field)) {
return (mlc::String("self.", 5) + cpp_naming::cpp_safe(object_name));
} else {
return context::context_resolve(context, cpp_naming::map_builtin(object_name));
}
}
mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SExpr> argument, context::CodegenContext context) noexcept{
auto type_name = std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*inner)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*semantic_ir::sexpr_type(argument)));
if ((type_name.length() > 0)) {
return context::context_resolve(context, type_name);
} else {
return mlc::String("auto", 4);
}
}
mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept{
if ((method_name == mlc::String("read", 4))) {
return mlc::String("mlc::file::read_to_string", 25);
} else if ((method_name == mlc::String("write", 5))) {
return mlc::String("mlc::file::write_string", 23);
} else {
return (mlc::String("mlc::file::", 11) + method_name);
}

}
mlc::String field_access_operator(std::shared_ptr<semantic_ir::SExpr> object, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("->", 2); },
[&](const auto& __v) { return mlc::String(".", 1); }
}, (*semantic_ir::sexpr_type(object)));
}

} // namespace expression_support
