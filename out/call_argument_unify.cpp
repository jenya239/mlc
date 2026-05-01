#define main mlc_user_main
#include "call_argument_unify.hpp"

namespace call_argument_unify {

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
auto empty_list = mlc::Array<ast::Diagnostic>{};
return empty_list;
}
bool list_contains_string(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
auto index = 0;
auto found = false;
while (((index < haystack.length()) && (!found))) {
if ((haystack[index] == needle)) {
found = true;
}
index = (index + 1);
}
return found;
}
mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span) noexcept{
if (semantic_type_structure::type_is_unknown(actual_type)) {
return empty_diagnostic_list();
} else if (semantic_type_structure::type_is_unknown(expected_type)) {
return empty_diagnostic_list();
} else {
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_expected] = tArray; return (semantic_type_structure::type_is_array(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::array_element_type_from_array_type(actual_type), type_parameter_names, substitution, diagnostic_span) : mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)}); },
[&](const registry::TShared& tShared) { auto [inner_expected] = tShared; return (semantic_type_structure::type_is_shared_pointer(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::shared_pointer_inner_type(actual_type), type_parameter_names, substitution, diagnostic_span) : mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)}); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return (list_contains_string(type_parameter_names, name) ? (substitution.has(name) ? unify_expected_and_actual_argument_types(substitution.get(name), actual_type, type_parameter_names, substitution, diagnostic_span) : [&]() {
substitution.set(name, actual_type);
return empty_diagnostic_list();
}()) : (semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)})); },
[&](const registry::TGeneric& tGeneric) { auto [expected_name, expected_args] = tGeneric; return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [actual_name, actual_args] = tGeneric; return (((expected_name != actual_name) || (expected_args.length() != actual_args.length())) ? mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)} : [&]() {
auto out = empty_diagnostic_list();
auto gi = 0;
while ((gi < expected_args.length())) {
out = ast::diagnostics_append(out, unify_expected_and_actual_argument_types(expected_args[gi], actual_args[gi], type_parameter_names, substitution, diagnostic_span));
gi = (gi + 1);
}
return out;
}()); },
[&](const auto& __v) { return mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)}; }
}, (*actual_type)); },
[&](const registry::TTuple& tTuple) { auto [expected_elts] = tTuple; return std::visit(overloaded{[&](const registry::TTuple& tTuple) { auto [actual_elts] = tTuple; return ((expected_elts.length() != actual_elts.length()) ? mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)} : [&]() {
auto out_t = empty_diagnostic_list();
auto ti = 0;
while ((ti < expected_elts.length())) {
out_t = ast::diagnostics_append(out_t, unify_expected_and_actual_argument_types(expected_elts[ti], actual_elts[ti], type_parameter_names, substitution, diagnostic_span));
ti = (ti + 1);
}
return out_t;
}()); },
[&](const registry::TPair& tPair) { auto [a2, b2] = tPair; return ((expected_elts.length() != 2) ? mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)} : ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_elts[0], a2, type_parameter_names, substitution, diagnostic_span), unify_expected_and_actual_argument_types(expected_elts[1], b2, type_parameter_names, substitution, diagnostic_span))); },
[&](const auto& __v) { return mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)}; }
}, (*actual_type)); },
[&](const registry::TPair& tPair) { auto [expected_a, expected_b] = tPair; return std::visit(overloaded{[&](const registry::TPair& tPair) { auto [actual_a, actual_b] = tPair; return ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, actual_a, type_parameter_names, substitution, diagnostic_span), unify_expected_and_actual_argument_types(expected_b, actual_b, type_parameter_names, substitution, diagnostic_span)); },
[&](const registry::TTuple& tTuple) { auto [ats] = tTuple; return ((ats.length() != 2) ? mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)} : ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, ats[0], type_parameter_names, substitution, diagnostic_span), unify_expected_and_actual_argument_types(expected_b, ats[1], type_parameter_names, substitution, diagnostic_span))); },
[&](const auto& __v) { return mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)}; }
}, (*actual_type)); },
[&](const registry::TFn& tFn) { auto [expected_params, expected_ret] = tFn; return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [actual_params, actual_ret] = tFn; return ((expected_params.length() != actual_params.length()) ? mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)} : [&]() {
auto out_fn = empty_diagnostic_list();
auto pi = 0;
while ((pi < expected_params.length())) {
out_fn = ast::diagnostics_append(out_fn, unify_expected_and_actual_argument_types(expected_params[pi], actual_params[pi], type_parameter_names, substitution, diagnostic_span));
pi = (pi + 1);
}
return ast::diagnostics_append(out_fn, unify_expected_and_actual_argument_types(expected_ret, actual_ret, type_parameter_names, substitution, diagnostic_span));
}()); },
[&](const auto& __v) { return mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)}; }
}, (*actual_type)); },
[&](const auto& __v) { return (semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array{ast::diagnostic_error((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span)}); }
}, (*expected_type));
}

}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_call_type_substitution(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names) noexcept{
auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto index = 0;
while (((index < expected_parameter_types.length()) && (index < actual_argument_types.length()))) {
unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, ast::span_unknown());
index = (index + 1);
}
return substitution;
}
mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept{
auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto collected = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while (((index < expected_parameter_types.length()) && (index < actual_argument_types.length()))) {
collected = ast::diagnostics_append(collected, unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, ast::expr_span(argument_expressions[index])));
index = (index + 1);
}
return collected;
}

} // namespace call_argument_unify
