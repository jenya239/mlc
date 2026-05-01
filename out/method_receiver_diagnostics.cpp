#define main mlc_user_main
#include "method_receiver_diagnostics.hpp"

namespace method_receiver_diagnostics {

mlc::Array<ast::Diagnostic> infer_method_receiver_push(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if ((method_name == mlc::String("push", 4))) {
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_array(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("method push expects an array receiver, got ", 43) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_set(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if ((method_name == mlc::String("set", 3))) {
if ((((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_array(receiver_type))) && (!semantic_type_structure::receiver_type_is_map(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("method set expects an array or Map receiver, got ", 49) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_length_or_size(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if (((method_name == mlc::String("length", 6)) || (method_name == mlc::String("size", 4)))) {
if ((((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_string(receiver_type))) && (!semantic_type_structure::type_is_array(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("methods length and size apply to string or array; receiver is ", 62) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_to_integer_string_only(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if ((method_name == mlc::String("to_i", 4))) {
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_string(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("method to_i expects a string receiver, got ", 43) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_string_only_methods(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if ((((method_name == mlc::String("char_at", 7)) || (method_name == mlc::String("substring", 9))) || (method_name == mlc::String("to_lower", 8)))) {
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_string(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("method ", 7) + method_name) + mlc::String(" expects a string receiver, got ", 32)) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_join(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if ((method_name == mlc::String("join", 4))) {
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_array(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("method join expects an array receiver, got ", 43) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_has(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if ((method_name == mlc::String("has", 3))) {
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::receiver_type_is_map(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("method has expects a Map receiver, got ", 39) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_get_remove_keys_values(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if (((((method_name == mlc::String("get", 3)) || (method_name == mlc::String("remove", 6))) || (method_name == mlc::String("keys", 4))) || (method_name == mlc::String("values", 6)))) {
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::receiver_type_is_map(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("method ", 7) + method_name) + mlc::String(" expects a Map receiver, got ", 29)) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_method_receiver_array_hof(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
if (result_option_method_types::should_skip_array_hof_for_receiver(receiver_type)) {
return {};
} else if (array_method_types::is_array_hof_method(method_name)) {
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_array(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("method ", 7) + method_name) + mlc::String(" expects an array receiver, got ", 32)) + semantic_type_structure::type_description(receiver_type)), method_span)};
} else {
return {};
}
} else {
return {};
}

}
mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept{
auto from_hof = infer_method_receiver_array_hof(method_name, receiver_type, method_span);
if ((from_hof.length() > 0)) {
return from_hof;
} else {
auto from_push = infer_method_receiver_push(method_name, receiver_type, method_span);
if ((from_push.length() > 0)) {
return from_push;
} else {
auto from_set = infer_method_receiver_set(method_name, receiver_type, method_span);
if ((from_set.length() > 0)) {
return from_set;
} else {
auto from_length = infer_method_receiver_length_or_size(method_name, receiver_type, method_span);
if ((from_length.length() > 0)) {
return from_length;
} else {
auto from_to_i = infer_method_receiver_to_integer_string_only(method_name, receiver_type, method_span);
if ((from_to_i.length() > 0)) {
return from_to_i;
} else {
auto from_string_ops = infer_method_receiver_string_only_methods(method_name, receiver_type, method_span);
if ((from_string_ops.length() > 0)) {
return from_string_ops;
} else {
auto from_join = infer_method_receiver_join(method_name, receiver_type, method_span);
if ((from_join.length() > 0)) {
return from_join;
} else {
auto from_map_access = infer_method_receiver_get_remove_keys_values(method_name, receiver_type, method_span);
if ((from_map_access.length() > 0)) {
return from_map_access;
} else {
return infer_method_receiver_has(method_name, receiver_type, method_span);
}
}
}
}
}
}
}
}
}

} // namespace method_receiver_diagnostics
