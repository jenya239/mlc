#define main mlc_user_main
#include "semantic_type_structure.hpp"

namespace semantic_type_structure {

bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return std::visit(overloaded{[&](const registry::TI32& tI32) { return true; },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TString& tString) { return std::visit(overloaded{[&](const registry::TString& tString) { return true; },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TBool& tBool) { return std::visit(overloaded{[&](const registry::TBool& tBool) { return true; },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TUnit& tUnit) { return std::visit(overloaded{[&](const registry::TUnit& tUnit) { return true; },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TUnknown& tUnknown) { return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TArray& tArray) { auto [inner_left] = tArray; return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_right] = tArray; return types_structurally_equal(inner_left, inner_right); },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TShared& tShared) { auto [inner_left] = tShared; return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [inner_right] = tShared; return types_structurally_equal(inner_left, inner_right); },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TNamed& tNamed) { auto [left_name] = tNamed; return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [right_name] = tNamed; return (left_name == right_name); },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TGeneric& tGeneric) { auto [left_name, left_args] = tGeneric; return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [right_name, right_args] = tGeneric; return (((left_name != right_name) || (left_args.length() != right_args.length())) ? false : [&]() {
auto gi = 0;
auto gok = true;
while (((gi < left_args.length()) && gok)) {
if ((!types_structurally_equal(left_args[gi], right_args[gi]))) {
gok = false;
}
gi = (gi + 1);
}
return gok;
}()); },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TTuple& tTuple) { auto [t1] = tTuple; return std::visit(overloaded{[&](const registry::TTuple& tTuple) { auto [t2] = tTuple; return ((t1.length() != t2.length()) ? false : [&]() {
auto ti = 0;
auto tok = true;
while (((ti < t1.length()) && tok)) {
if ((!types_structurally_equal(t1[ti], t2[ti]))) {
tok = false;
}
ti = (ti + 1);
}
return tok;
}()); },
[&](const registry::TPair& tPair) { auto [a2, b2] = tPair; return (((t1.length() == 2) && types_structurally_equal(t1[0], a2)) && types_structurally_equal(t1[1], b2)); },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TPair& tPair) { auto [a1, b1] = tPair; return std::visit(overloaded{[&](const registry::TPair& tPair) { auto [a2, b2] = tPair; return (types_structurally_equal(a1, a2) && types_structurally_equal(b1, b2)); },
[&](const registry::TTuple& tTuple) { auto [t2] = tTuple; return (((t2.length() == 2) && types_structurally_equal(a1, t2[0])) && types_structurally_equal(b1, t2[1])); },
[&](const auto& __v) { return false; }
}, (*right)); },
[&](const registry::TFn& tFn) { auto [left_params, left_ret] = tFn; return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [right_params, right_ret] = tFn; return ((left_params.length() != right_params.length()) ? false : [&]() {
auto fi = 0;
auto fok = true;
while (((fi < left_params.length()) && fok)) {
if ((!types_structurally_equal(left_params[fi], right_params[fi]))) {
fok = false;
}
fi = (fi + 1);
}
return (fok && types_structurally_equal(left_ret, right_ret));
}()); },
[&](const auto& __v) { return false; }
}, (*right)); }
}, (*left));
}
bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [__0] = tArray; return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept{
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*array_type));
}
bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept{
return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [inner] = tShared; return inner; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*shared_pointer_type));
}
bool type_is_function(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
mlc::Array<std::shared_ptr<registry::Type>> empty_type_parameter_list() noexcept{
return {};
}
std::shared_ptr<registry::Type> function_return_type(std::shared_ptr<registry::Type> function_type) noexcept{
return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, return_type] = tFn; return return_type; },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*function_type));
}
mlc::Array<std::shared_ptr<registry::Type>> function_parameter_list(std::shared_ptr<registry::Type> function_type) noexcept{
return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [parameters, __1] = tFn; return parameters; },
[&](const registry::TI32& tI32) { return empty_type_parameter_list(); },
[&](const registry::TString& tString) { return empty_type_parameter_list(); },
[&](const registry::TBool& tBool) { return empty_type_parameter_list(); },
[&](const registry::TUnit& tUnit) { return empty_type_parameter_list(); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return empty_type_parameter_list(); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return empty_type_parameter_list(); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return empty_type_parameter_list(); },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return empty_type_parameter_list(); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return empty_type_parameter_list(); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return empty_type_parameter_list(); },
[&](const registry::TUnknown& tUnknown) { return empty_type_parameter_list(); }
}, (*function_type));
}
bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TBool& tBool) { return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TString& tString) { return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept{
return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return (name == mlc::String("Map", 3)); },
[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return (name == mlc::String("Map", 3)); },
[&](const auto& __v) { return false; }
}, (*receiver_type));
}
std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept{
if ((((((operation == mlc::String("+", 1)) || (operation == mlc::String("-", 1))) || (operation == mlc::String("*", 1))) || (operation == mlc::String("/", 1))) || (operation == mlc::String("%", 1)))) {
return std::visit(overloaded{[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TString{}); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TI32{}); }
}, (*left_type));
} else if ((operation == mlc::String("=", 1))) {
return std::make_shared<registry::Type>(registry::TUnit{});
} else {
return std::make_shared<registry::Type>(registry::TBool{});
}

}
mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return mlc::String("i32", 3); },
[&](const registry::TString& tString) { return mlc::String("string", 6); },
[&](const registry::TBool& tBool) { return mlc::String("bool", 4); },
[&](const registry::TUnit& tUnit) { return mlc::String("unit", 4); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("unknown", 7); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return mlc::String("array", 5); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("shared", 6); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return type_name; },
[&](const registry::TGeneric& tGeneric) { auto [type_name, __1] = tGeneric; return type_name; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return mlc::String("pair", 4); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return mlc::String("tuple", 5); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return mlc::String("function", 8); }
}, (*type_value));
}
bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
return ((std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); }
}, (*left_type)) || std::visit(overloaded{[&](const registry::TI32& tI32) { return std::visit(overloaded{[&](const registry::TI32& tI32) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); },
[&](const auto& __v) { return false; }
}, (*left_type))) || std::visit(overloaded{[&](const registry::TString& tString) { return std::visit(overloaded{[&](const registry::TString& tString) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); },
[&](const auto& __v) { return false; }
}, (*left_type)));
}
bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
return (std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); }
}, (*left_type)) || std::visit(overloaded{[&](const registry::TI32& tI32) { return std::visit(overloaded{[&](const registry::TI32& tI32) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); },
[&](const auto& __v) { return false; }
}, (*left_type)));
}
bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
return ((std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); }
}, (*left_type)) || std::visit(overloaded{[&](const registry::TI32& tI32) { return std::visit(overloaded{[&](const registry::TI32& tI32) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); },
[&](const auto& __v) { return false; }
}, (*left_type))) || std::visit(overloaded{[&](const registry::TString& tString) { return std::visit(overloaded{[&](const registry::TString& tString) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); },
[&](const auto& __v) { return false; }
}, (*left_type)));
}
bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
return (std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); }
}, (*left_type)) || std::visit(overloaded{[&](const registry::TBool& tBool) { return std::visit(overloaded{[&](const registry::TBool& tBool) { return true; },
[&](const auto& __v) { return false; }
}, (*right_type)); },
[&](const auto& __v) { return false; }
}, (*left_type)));
}
std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept{
if ((((method_name == mlc::String("length", 6)) || (method_name == mlc::String("size", 4))) || (method_name == mlc::String("to_i", 4)))) {
return std::make_shared<registry::Type>(registry::TI32{});
} else if ((((method_name == mlc::String("push", 4)) || (method_name == mlc::String("set", 3))) || (method_name == mlc::String("remove", 6)))) {
return std::make_shared<registry::Type>(registry::TUnit{});
} else if (((method_name == mlc::String("keys", 4)) || (method_name == mlc::String("values", 6)))) {
return std::make_shared<registry::Type>(registry::TArray{std::make_shared<registry::Type>(registry::TUnknown{})});
} else if ((((((method_name == mlc::String("char_at", 7)) || (method_name == mlc::String("join", 4))) || (method_name == mlc::String("to_string", 9))) || (method_name == mlc::String("substring", 9))) || (method_name == mlc::String("to_lower", 8)))) {
return std::make_shared<registry::Type>(registry::TString{});
} else if ((method_name == mlc::String("has", 3))) {
return std::make_shared<registry::Type>(registry::TBool{});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}




}
int builtin_method_expected_argument_count(mlc::String method_name) noexcept{
if ((method_name == mlc::String("push", 4))) {
return 1;
} else if ((method_name == mlc::String("set", 3))) {
return 2;
} else if (((method_name == mlc::String("length", 6)) || (method_name == mlc::String("size", 4)))) {
return 0;
} else if (((method_name == mlc::String("to_i", 4)) || (method_name == mlc::String("to_lower", 8)))) {
return 0;
} else if ((method_name == mlc::String("char_at", 7))) {
return 1;
} else if ((method_name == mlc::String("substring", 9))) {
return 2;
} else if ((method_name == mlc::String("join", 4))) {
return 1;
} else if ((method_name == mlc::String("to_string", 9))) {
return 0;
} else if ((((method_name == mlc::String("has", 3)) || (method_name == mlc::String("get", 3))) || (method_name == mlc::String("remove", 6)))) {
return 1;
} else if (((method_name == mlc::String("keys", 4)) || (method_name == mlc::String("values", 6)))) {
return 0;
} else {
return (-1);
}









}
std::shared_ptr<registry::Type> substitute_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return (substitution.has(name) ? substitute_type(substitution.get(name), substitution) : type_value); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return std::make_shared<registry::Type>(registry::TArray{substitute_type(inner, substitution)}); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return std::make_shared<registry::Type>(registry::TShared{substitute_type(inner, substitution)}); },
[&](const registry::TGeneric& tGeneric) { auto [name, args] = tGeneric; return [&]() {
auto new_args = mlc::Array<std::shared_ptr<registry::Type>>{};
auto i = 0;
while ((i < mlc::io::args.length())) {
new_args.push_back(substitute_type(mlc::io::args[i], substitution));
i = (i + 1);
}
return std::make_shared<registry::Type>(registry::TGeneric{name, new_args});
}(); },
[&](const registry::TPair& tPair) { auto [a, b] = tPair; return std::make_shared<registry::Type>(registry::TPair{substitute_type(a, substitution), substitute_type(b, substitution)}); },
[&](const registry::TTuple& tTuple) { auto [ts] = tTuple; return [&]() {
auto new_ts = mlc::Array<std::shared_ptr<registry::Type>>{};
auto si = 0;
while ((si < ts.length())) {
new_ts.push_back(substitute_type(ts[si], substitution));
si = (si + 1);
}
return std::make_shared<registry::Type>(registry::TTuple{new_ts});
}(); },
[&](const registry::TFn& tFn) { auto [params, ret] = tFn; return [&]() {
auto new_params = mlc::Array<std::shared_ptr<registry::Type>>{};
auto j = 0;
while ((j < params.length())) {
new_params.push_back(substitute_type(params[j], substitution));
j = (j + 1);
}
return std::make_shared<registry::Type>(registry::TFn{new_params, substitute_type(ret, substitution)});
}(); },
[&](const auto& __v) { return type_value; }
}, (*type_value));
}

} // namespace semantic_type_structure
