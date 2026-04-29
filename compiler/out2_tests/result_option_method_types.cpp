#define main mlc_user_main
#include "result_option_method_types.hpp"

namespace result_option_method_types {

bool is_result_generic(std::shared_ptr<registry::Type> t) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, type_args] = tGeneric; return ((name == mlc::String("Result", 6)) && (type_args.length() == 2)); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return (name == mlc::String("Result", 6)); },
[&](const auto& __v) { return false; }
}, (*t));
}
bool is_option_generic(std::shared_ptr<registry::Type> t) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, type_args] = tGeneric; return ((name == mlc::String("Option", 6)) && (type_args.length() == 1)); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return (name == mlc::String("Option", 6)); },
[&](const auto& __v) { return false; }
}, (*t));
}
bool should_skip_array_hof_for_receiver(std::shared_ptr<registry::Type> t) noexcept{
return (is_result_generic(t) || is_option_generic(t));
}
bool is_result_combinator_method(mlc::String method_name) noexcept{
return (((((((method_name == mlc::String("map", 3)) || (method_name == mlc::String("map_err", 7))) || (method_name == mlc::String("and_then", 8))) || (method_name == mlc::String("or_else", 7))) || (method_name == mlc::String("unwrap_or", 9))) || (method_name == mlc::String("unwrap_or_else", 14))) || (method_name == mlc::String("ok", 2)));
}
bool is_option_combinator_method(mlc::String method_name) noexcept{
return ((((((method_name == mlc::String("map", 3)) || (method_name == mlc::String("and_then", 8))) || (method_name == mlc::String("or_else", 7))) || (method_name == mlc::String("unwrap_or", 9))) || (method_name == mlc::String("filter", 6))) || (method_name == mlc::String("ok_or", 5)));
}
bool should_infer_result_option_combinator(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
return ((is_result_generic(receiver_type) && is_result_combinator_method(method_name)) || (is_option_generic(receiver_type) && is_option_combinator_method(method_name)));
}
int result_option_hof_expected_argument_count(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
if ((method_name == mlc::String("ok", 2))) {
return 0;
} else {
return 1;
}
}
std::shared_ptr<registry::Type> result_ok_type(std::shared_ptr<registry::Type> result_type) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, a] = tGeneric; return (((name == mlc::String("Result", 6)) && (a.length() >= 1)) ? a[0] : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return ((name == mlc::String("Result", 6)) ? std::make_shared<registry::Type>(registry::TUnknown{}) : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*result_type));
}
std::shared_ptr<registry::Type> result_err_type(std::shared_ptr<registry::Type> result_type) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, a] = tGeneric; return (((name == mlc::String("Result", 6)) && (a.length() >= 2)) ? a[1] : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return ((name == mlc::String("Result", 6)) ? std::make_shared<registry::Type>(registry::TUnknown{}) : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*result_type));
}
std::shared_ptr<registry::Type> option_inner_type(std::shared_ptr<registry::Type> opt_type) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, a] = tGeneric; return (((name == mlc::String("Option", 6)) && (a.length() >= 1)) ? a[0] : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return ((name == mlc::String("Option", 6)) ? std::make_shared<registry::Type>(registry::TUnknown{}) : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*opt_type));
}
std::shared_ptr<registry::Type> result_option_hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> arg_inferred_types) noexcept{
if (is_result_generic(receiver_type)) {
auto t_type = result_ok_type(receiver_type);
auto e_type = result_err_type(receiver_type);
if ((method_name == mlc::String("map", 3))) {
if ((arg_inferred_types.length() > 0)) {
return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Result", 6), mlc::Array{semantic_type_structure::function_return_type(arg_inferred_types[0]), e_type}});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if ((method_name == mlc::String("map_err", 7))) {
if ((arg_inferred_types.length() > 0)) {
return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Result", 6), mlc::Array{t_type, semantic_type_structure::function_return_type(arg_inferred_types[0])}});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if (((method_name == mlc::String("and_then", 8)) || (method_name == mlc::String("or_else", 7)))) {
if ((arg_inferred_types.length() > 0)) {
return semantic_type_structure::function_return_type(arg_inferred_types[0]);
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if (((method_name == mlc::String("unwrap_or", 9)) || (method_name == mlc::String("unwrap_or_else", 14)))) {
return t_type;
} else if ((method_name == mlc::String("ok", 2))) {
return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array{t_type}});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}




} else if (is_option_generic(receiver_type)) {
auto opt_t = option_inner_type(receiver_type);
if ((method_name == mlc::String("map", 3))) {
if ((arg_inferred_types.length() > 0)) {
return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array{semantic_type_structure::function_return_type(arg_inferred_types[0])}});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if ((method_name == mlc::String("and_then", 8))) {
if ((arg_inferred_types.length() > 0)) {
return semantic_type_structure::function_return_type(arg_inferred_types[0]);
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if ((method_name == mlc::String("or_else", 7))) {
if ((arg_inferred_types.length() > 0)) {
return semantic_type_structure::function_return_type(arg_inferred_types[0]);
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if ((method_name == mlc::String("unwrap_or", 9))) {
return opt_t;
} else if ((method_name == mlc::String("filter", 6))) {
return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array{opt_t}});
} else if ((method_name == mlc::String("ok_or", 5))) {
if ((arg_inferred_types.length() > 0)) {
return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Result", 6), mlc::Array{opt_t, arg_inferred_types[0]}});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}





} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}

}
void __skip__() noexcept{
return /* unit */;
}

} // namespace result_option_method_types
