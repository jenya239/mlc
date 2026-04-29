#include "semantic_type_structure.hpp"

#include "registry.hpp"

namespace semantic_type_structure {

using namespace registry;

bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept;

bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept;

bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept;

bool type_is_function(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> empty_type_parameter_list() noexcept;

std::shared_ptr<registry::Type> function_return_type(std::shared_ptr<registry::Type> function_type) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> function_parameter_list(std::shared_ptr<registry::Type> function_type) noexcept;

bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept;

bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept;

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept;

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept;

int builtin_method_expected_argument_count(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> substitute_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*type_value))) {  return true; } return false; }();}

bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept{return type_is_unknown(left) || type_is_unknown(right) ? true : std::visit(overloaded{
  [&](const TI32& ti32) -> bool { return [&]() { if (std::holds_alternative<registry::TI32>((*right))) {  return true; } return false; }(); },
  [&](const TString& tstring) -> bool { return [&]() { if (std::holds_alternative<registry::TString>((*right))) {  return true; } return false; }(); },
  [&](const TBool& tbool) -> bool { return [&]() { if (std::holds_alternative<registry::TBool>((*right))) {  return true; } return false; }(); },
  [&](const TUnit& tunit) -> bool { return [&]() { if (std::holds_alternative<registry::TUnit>((*right))) {  return true; } return false; }(); },
  [&](const TUnknown& tunknown) -> bool { return [&]() { if (std::holds_alternative<registry::TUnknown>((*right))) {  return true; } return false; }(); },
  [&](const TArray& tarray) -> bool { auto [inner_left] = tarray; return [&]() { if (std::holds_alternative<registry::TArray>((*right))) { auto _v_tarray = std::get<registry::TArray>((*right)); auto [inner_right] = _v_tarray; return types_structurally_equal(inner_left, inner_right); } return false; }(); },
  [&](const TShared& tshared) -> bool { auto [inner_left] = tshared; return [&]() { if (std::holds_alternative<registry::TShared>((*right))) { auto _v_tshared = std::get<registry::TShared>((*right)); auto [inner_right] = _v_tshared; return types_structurally_equal(inner_left, inner_right); } return false; }(); },
  [&](const TNamed& tnamed) -> bool { auto [left_name] = tnamed; return [&]() { if (std::holds_alternative<registry::TNamed>((*right))) { auto _v_tnamed = std::get<registry::TNamed>((*right)); auto [right_name] = _v_tnamed; return left_name == right_name; } return false; }(); },
  [&](const TGeneric& tgeneric) -> bool { auto [left_name, left_args] = tgeneric; return [&]() { if (std::holds_alternative<registry::TGeneric>((*right))) { auto _v_tgeneric = std::get<registry::TGeneric>((*right)); auto [right_name, right_args] = _v_tgeneric; return left_name != right_name || left_args.size() != right_args.size() ? false : [&]() -> bool { 
  int gi = 0;
  bool gok = true;
  while (gi < left_args.size() && gok){
{
if (!types_structurally_equal(left_args[gi], right_args[gi])){
{
gok = false;
}
}
gi = gi + 1;
}
}
  return gok;
 }(); } return false; }(); },
  [&](const TTuple& ttuple) -> bool { auto [t1] = ttuple; return [&]() { if (std::holds_alternative<registry::TTuple>((*right))) { auto _v_ttuple = std::get<registry::TTuple>((*right)); auto [t2] = _v_ttuple; return t1.size() != t2.size() ? false : [&]() -> bool { 
  int ti = 0;
  bool tok = true;
  while (ti < t1.size() && tok){
{
if (!types_structurally_equal(t1[ti], t2[ti])){
{
tok = false;
}
}
ti = ti + 1;
}
}
  return tok;
 }(); } if (std::holds_alternative<registry::TPair>((*right))) { auto _v_tpair = std::get<registry::TPair>((*right)); auto [a2, b2] = _v_tpair; return t1.size() == 2 && types_structurally_equal(t1[0], a2) && types_structurally_equal(t1[1], b2); } return false; }(); },
  [&](const TPair& tpair) -> bool { auto [a1, b1] = tpair; return [&]() { if (std::holds_alternative<registry::TPair>((*right))) { auto _v_tpair = std::get<registry::TPair>((*right)); auto [a2, b2] = _v_tpair; return types_structurally_equal(a1, a2) && types_structurally_equal(b1, b2); } if (std::holds_alternative<registry::TTuple>((*right))) { auto _v_ttuple = std::get<registry::TTuple>((*right)); auto [t2] = _v_ttuple; return t2.size() == 2 && types_structurally_equal(a1, t2[0]) && types_structurally_equal(b1, t2[1]); } return false; }(); },
  [&](const TFn& tfn) -> bool { auto [left_params, left_ret] = tfn; return [&]() { if (std::holds_alternative<registry::TFn>((*right))) { auto _v_tfn = std::get<registry::TFn>((*right)); auto [right_params, right_ret] = _v_tfn; return left_params.size() != right_params.size() ? false : [&]() -> bool { 
  int fi = 0;
  bool fok = true;
  while (fi < left_params.size() && fok){
{
if (!types_structurally_equal(left_params[fi], right_params[fi])){
{
fok = false;
}
}
fi = fi + 1;
}
}
  return fok && types_structurally_equal(left_ret, right_ret);
 }(); } return false; }(); }
}, (*left));}

bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [_w0] = _v_tarray; return true; } return false; }();}

std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*array_type))) { auto _v_tarray = std::get<registry::TArray>((*array_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [_w0] = _v_tshared; return true; } return false; }();}

std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TShared>((*shared_pointer_type))) { auto _v_tshared = std::get<registry::TShared>((*shared_pointer_type)); auto [inner] = _v_tshared; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

bool type_is_function(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TFn>((*type_value))) { auto _v_tfn = std::get<registry::TFn>((*type_value)); auto [_w0, _w1] = _v_tfn; return true; } return false; }();}

mlc::Array<std::shared_ptr<registry::Type>> empty_type_parameter_list() noexcept{return {};}

std::shared_ptr<registry::Type> function_return_type(std::shared_ptr<registry::Type> function_type) noexcept{return std::visit(overloaded{
  [&](const TFn& tfn) -> std::shared_ptr<registry::Type> { auto [_w0, return_type] = tfn; return return_type; },
  [&](const TI32& ti32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TString& tstring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TBool& tbool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnit& tunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [_w0] = tarray; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [_w0] = tshared; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [_w0] = tnamed; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tgeneric; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tpair; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [_w0] = ttuple; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); }
}, (*function_type));}

mlc::Array<std::shared_ptr<registry::Type>> function_parameter_list(std::shared_ptr<registry::Type> function_type) noexcept{return std::visit(overloaded{
  [&](const TFn& tfn) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [parameters, _w0] = tfn; return parameters; },
  [&](const TI32& ti32) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TString& tstring) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TBool& tbool) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TUnit& tunit) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TArray& tarray) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tarray; return empty_type_parameter_list(); },
  [&](const TShared& tshared) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tshared; return empty_type_parameter_list(); },
  [&](const TNamed& tnamed) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tnamed; return empty_type_parameter_list(); },
  [&](const TGeneric& tgeneric) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tgeneric; return empty_type_parameter_list(); },
  [&](const TPair& tpair) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tpair; return empty_type_parameter_list(); },
  [&](const TTuple& ttuple) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = ttuple; return empty_type_parameter_list(); },
  [&](const TUnknown& tunknown) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); }
}, (*function_type));}

bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TI32>((*type_value))) {  return true; } return false; }();}

bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TBool>((*type_value))) {  return true; } return false; }();}

bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TString>((*type_value))) {  return true; } return false; }();}

bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept{return [&]() { if (std::holds_alternative<registry::TNamed>((*receiver_type))) { auto _v_tnamed = std::get<registry::TNamed>((*receiver_type)); auto [name] = _v_tnamed; return name == mlc::String("Map"); } if (std::holds_alternative<registry::TGeneric>((*receiver_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*receiver_type)); auto [name, _w0] = _v_tgeneric; return name == mlc::String("Map"); } return false; }();}

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept{
return operation == mlc::String("+") || operation == mlc::String("-") || operation == mlc::String("*") || operation == mlc::String("/") || operation == mlc::String("%") ? [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return std::make_shared<registry::Type>((registry::TString{})); } return std::make_shared<registry::Type>((registry::TI32{})); }() : operation == mlc::String("=") ? std::make_shared<registry::Type>((registry::TUnit{})) : std::make_shared<registry::Type>((registry::TBool{}));
}

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TI32& ti32) -> mlc::String { return mlc::String("i32"); },
  [&](const TString& tstring) -> mlc::String { return mlc::String("string"); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String("bool"); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String("unit"); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String("unknown"); },
  [&](const TArray& tarray) -> mlc::String { auto [_w0] = tarray; return mlc::String("array"); },
  [&](const TShared& tshared) -> mlc::String { auto [_w0] = tshared; return mlc::String("shared"); },
  [&](const TNamed& tnamed) -> mlc::String { auto [type_name] = tnamed; return type_name; },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [type_name, _w0] = tgeneric; return type_name; },
  [&](const TPair& tpair) -> mlc::String { auto [_w0, _w1] = tpair; return mlc::String("pair"); },
  [&](const TTuple& ttuple) -> mlc::String { auto [_w0] = ttuple; return mlc::String("tuple"); },
  [&](const TFn& tfn) -> mlc::String { auto [_w0, _w1] = tfn; return mlc::String("function"); }
}, (*type_value));}

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || [&]() { if (std::holds_alternative<registry::TI32>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TI32>((*right_type))) {  return true; } return false; }(); } return false; }() || [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TString>((*right_type))) {  return true; } return false; }(); } return false; }();}

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || [&]() { if (std::holds_alternative<registry::TI32>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TI32>((*right_type))) {  return true; } return false; }(); } return false; }();}

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || [&]() { if (std::holds_alternative<registry::TI32>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TI32>((*right_type))) {  return true; } return false; }(); } return false; }() || [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TString>((*right_type))) {  return true; } return false; }(); } return false; }();}

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || [&]() { if (std::holds_alternative<registry::TBool>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TBool>((*right_type))) {  return true; } return false; }(); } return false; }();}

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept{
return method_name == mlc::String("length") || method_name == mlc::String("size") || method_name == mlc::String("to_i") ? std::make_shared<registry::Type>((registry::TI32{})) : method_name == mlc::String("push") || method_name == mlc::String("set") || method_name == mlc::String("remove") ? std::make_shared<registry::Type>((registry::TUnit{})) : method_name == mlc::String("keys") || method_name == mlc::String("values") ? std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TUnknown{})))) : method_name == mlc::String("char_at") || method_name == mlc::String("join") || method_name == mlc::String("to_string") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? std::make_shared<registry::Type>((registry::TString{})) : method_name == mlc::String("has") ? std::make_shared<registry::Type>((registry::TBool{})) : std::make_shared<registry::Type>((registry::TUnknown{}));
}

int builtin_method_expected_argument_count(mlc::String method_name) noexcept{
return method_name == mlc::String("push") ? 1 : method_name == mlc::String("set") ? 2 : method_name == mlc::String("length") || method_name == mlc::String("size") ? 0 : method_name == mlc::String("to_i") || method_name == mlc::String("to_lower") ? 0 : method_name == mlc::String("char_at") ? 1 : method_name == mlc::String("substring") ? 2 : method_name == mlc::String("join") ? 1 : method_name == mlc::String("to_string") ? 0 : method_name == mlc::String("has") || method_name == mlc::String("get") || method_name == mlc::String("remove") ? 1 : method_name == mlc::String("keys") || method_name == mlc::String("values") ? 0 : -1;
}

std::shared_ptr<registry::Type> substitute_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return substitution.has(name) ? substitute_type(substitution.get(name), substitution) : type_value; } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [inner] = _v_tarray; return std::make_shared<registry::Type>(registry::TArray(substitute_type(inner, substitution))); } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return std::make_shared<registry::Type>(registry::TShared(substitute_type(inner, substitution))); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [name, args] = _v_tgeneric; return [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<std::shared_ptr<registry::Type>> new_args = {};
  int i = 0;
  while (i < args.size()){
{
new_args.push_back(substitute_type(args[i], substitution));
i = i + 1;
}
}
  return std::make_shared<registry::Type>(registry::TGeneric(name, new_args));
 }(); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [a, b] = _v_tpair; return std::make_shared<registry::Type>(registry::TPair(substitute_type(a, substitution), substitute_type(b, substitution))); } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [ts] = _v_ttuple; return [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<std::shared_ptr<registry::Type>> new_ts = {};
  int si = 0;
  while (si < ts.size()){
{
new_ts.push_back(substitute_type(ts[si], substitution));
si = si + 1;
}
}
  return std::make_shared<registry::Type>(registry::TTuple(new_ts));
 }(); } if (std::holds_alternative<registry::TFn>((*type_value))) { auto _v_tfn = std::get<registry::TFn>((*type_value)); auto [params, ret] = _v_tfn; return [&]() -> std::shared_ptr<registry::Type> { 
  mlc::Array<std::shared_ptr<registry::Type>> new_params = {};
  int j = 0;
  while (j < params.size()){
{
new_params.push_back(substitute_type(params[j], substitution));
j = j + 1;
}
}
  return std::make_shared<registry::Type>(registry::TFn(new_params, substitute_type(ret, substitution)));
 }(); } return type_value; }();}

} // namespace semantic_type_structure
