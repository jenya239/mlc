#include "semantic_type_structure.hpp"

#include "registry.hpp"

namespace semantic_type_structure {

using namespace registry;

bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept;

bool zipped_shared_types_structurally_equal(mlc::Array<std::shared_ptr<registry::Type>> left_list, mlc::Array<std::shared_ptr<registry::Type>> right_list) noexcept;

bool zipped_suffix_types_structurally_equal(mlc::Array<std::shared_ptr<registry::Type>> left_list, mlc::Array<std::shared_ptr<registry::Type>> right_list, int index) noexcept;

bool type_is_channel(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> channel_element_type_from_channel_type(std::shared_ptr<registry::Type> channel_type) noexcept;

bool type_is_mutex(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> mutex_inner_type_from_mutex_type(std::shared_ptr<registry::Type> mutex_type) noexcept;

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

bool is_numeric_type(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> arithmetic_binary_result_type(std::shared_ptr<registry::Type> left_type) noexcept;

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept;

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept;

int builtin_method_expected_argument_count(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> substitute_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

mlc::String operator_method_for(mlc::String operation) noexcept;

bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*type_value))) {  return true; } return false; }();}

bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept{return type_is_unknown(left) || type_is_unknown(right) ? true : std::visit(overloaded{
  [&](const TI32& ti32) -> bool { return [&]() { if (std::holds_alternative<registry::TI32>((*right))) {  return true; } return false; }(); },
  [&](const TString& tstring) -> bool { return [&]() { if (std::holds_alternative<registry::TString>((*right))) {  return true; } return false; }(); },
  [&](const TBool& tbool) -> bool { return [&]() { if (std::holds_alternative<registry::TBool>((*right))) {  return true; } return false; }(); },
  [&](const TUnit& tunit) -> bool { return [&]() { if (std::holds_alternative<registry::TUnit>((*right))) {  return true; } return false; }(); },
  [&](const TI64& ti64) -> bool { return [&]() { if (std::holds_alternative<registry::TI64>((*right))) {  return true; } return false; }(); },
  [&](const TF64& tf64) -> bool { return [&]() { if (std::holds_alternative<registry::TF64>((*right))) {  return true; } return false; }(); },
  [&](const TU8& tu8) -> bool { return [&]() { if (std::holds_alternative<registry::TU8>((*right))) {  return true; } return false; }(); },
  [&](const TUsize& tusize) -> bool { return [&]() { if (std::holds_alternative<registry::TUsize>((*right))) {  return true; } return false; }(); },
  [&](const TChar& tchar) -> bool { return [&]() { if (std::holds_alternative<registry::TChar>((*right))) {  return true; } return false; }(); },
  [&](const TUnknown& tunknown) -> bool { return [&]() { if (std::holds_alternative<registry::TUnknown>((*right))) {  return true; } return false; }(); },
  [&](const TArray& tarray) -> bool { auto [inner_left] = tarray; return [&]() { if (std::holds_alternative<registry::TArray>((*right))) { auto _v_tarray = std::get<registry::TArray>((*right)); auto [inner_right] = _v_tarray; return types_structurally_equal(inner_left, inner_right); } return false; }(); },
  [&](const TShared& tshared) -> bool { auto [inner_left] = tshared; return [&]() { if (std::holds_alternative<registry::TShared>((*right))) { auto _v_tshared = std::get<registry::TShared>((*right)); auto [inner_right] = _v_tshared; return types_structurally_equal(inner_left, inner_right); } return false; }(); },
  [&](const TNamed& tnamed) -> bool { auto [left_name] = tnamed; return [&]() { if (std::holds_alternative<registry::TNamed>((*right))) { auto _v_tnamed = std::get<registry::TNamed>((*right)); auto [right_name] = _v_tnamed; return left_name == right_name; } return false; }(); },
  [&](const TGeneric& tgeneric) -> bool { auto [left_name, left_args] = tgeneric; return [&]() { if (std::holds_alternative<registry::TGeneric>((*right))) { auto _v_tgeneric = std::get<registry::TGeneric>((*right)); auto [right_name, right_args] = _v_tgeneric; return left_name == right_name && left_args.size() == right_args.size() && zipped_shared_types_structurally_equal(left_args, right_args); } return false; }(); },
  [&](const TTuple& ttuple) -> bool { auto [left_tuple_types] = ttuple; return [&]() { if (std::holds_alternative<registry::TTuple>((*right))) { auto _v_ttuple = std::get<registry::TTuple>((*right)); auto [right_tuple_types] = _v_ttuple; return left_tuple_types.size() == right_tuple_types.size() && zipped_shared_types_structurally_equal(left_tuple_types, right_tuple_types); } if (std::holds_alternative<registry::TPair>((*right))) { auto _v_tpair = std::get<registry::TPair>((*right)); auto [right_first, right_second] = _v_tpair; return left_tuple_types.size() == 2 && types_structurally_equal(left_tuple_types[0], right_first) && types_structurally_equal(left_tuple_types[1], right_second); } return false; }(); },
  [&](const TPair& tpair) -> bool { auto [left_first, left_second] = tpair; return [&]() { if (std::holds_alternative<registry::TPair>((*right))) { auto _v_tpair = std::get<registry::TPair>((*right)); auto [right_first, right_second] = _v_tpair; return types_structurally_equal(left_first, right_first) && types_structurally_equal(left_second, right_second); } if (std::holds_alternative<registry::TTuple>((*right))) { auto _v_ttuple = std::get<registry::TTuple>((*right)); auto [right_tuple_types] = _v_ttuple; return right_tuple_types.size() == 2 && types_structurally_equal(left_first, right_tuple_types[0]) && types_structurally_equal(left_second, right_tuple_types[1]); } return false; }(); },
  [&](const TFn& tfn) -> bool { auto [left_parameters, left_return_type] = tfn; return [&]() { if (std::holds_alternative<registry::TFn>((*right))) { auto _v_tfn = std::get<registry::TFn>((*right)); auto [right_parameters, right_return_type] = _v_tfn; return left_parameters.size() == right_parameters.size() && zipped_shared_types_structurally_equal(left_parameters, right_parameters) && types_structurally_equal(left_return_type, right_return_type); } return false; }(); },
  [&](const TAssoc& tassoc) -> bool { auto [left_parameter, left_associated] = tassoc; return [&]() { if (std::holds_alternative<registry::TAssoc>((*right))) { auto _v_tassoc = std::get<registry::TAssoc>((*right)); auto [right_parameter, right_associated] = _v_tassoc; return left_parameter == right_parameter && left_associated == right_associated; } return true; }(); }
}, (*left));}

bool zipped_shared_types_structurally_equal(mlc::Array<std::shared_ptr<registry::Type>> left_list, mlc::Array<std::shared_ptr<registry::Type>> right_list) noexcept{return zipped_suffix_types_structurally_equal(left_list, right_list, 0);}

bool zipped_suffix_types_structurally_equal(mlc::Array<std::shared_ptr<registry::Type>> left_list, mlc::Array<std::shared_ptr<registry::Type>> right_list, int index) noexcept{return index >= left_list.size() ? true : types_structurally_equal(left_list[index], right_list[index]) && zipped_suffix_types_structurally_equal(left_list, right_list, index + 1);}

bool type_is_channel(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [type_name, _w0] = _v_tgeneric; return type_name == mlc::String("Channel"); } return false; }();}

std::shared_ptr<registry::Type> channel_element_type_from_channel_type(std::shared_ptr<registry::Type> channel_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*channel_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*channel_type)); auto [type_name, type_arguments] = _v_tgeneric; return type_name == mlc::String("Channel") && type_arguments.size() == 1 ? type_arguments[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

bool type_is_mutex(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [type_name, _w0] = _v_tgeneric; return type_name == mlc::String("Mutex"); } return false; }();}

std::shared_ptr<registry::Type> mutex_inner_type_from_mutex_type(std::shared_ptr<registry::Type> mutex_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*mutex_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*mutex_type)); auto [type_name, type_arguments] = _v_tgeneric; return type_name == mlc::String("Mutex") && type_arguments.size() == 1 ? type_arguments[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

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
  [&](const TI64& ti64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TF64& tf64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TU8& tu8) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUsize& tusize) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TChar& tchar) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [_w0] = tarray; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [_w0] = tshared; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [_w0] = tnamed; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tgeneric; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tpair; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [_w0] = ttuple; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TAssoc& tassoc) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tassoc; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); }
}, (*function_type));}

mlc::Array<std::shared_ptr<registry::Type>> function_parameter_list(std::shared_ptr<registry::Type> function_type) noexcept{return std::visit(overloaded{
  [&](const TFn& tfn) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [parameters, _w0] = tfn; return parameters; },
  [&](const TI32& ti32) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TString& tstring) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TBool& tbool) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TUnit& tunit) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TI64& ti64) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TF64& tf64) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TU8& tu8) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TUsize& tusize) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TChar& tchar) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); },
  [&](const TArray& tarray) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tarray; return empty_type_parameter_list(); },
  [&](const TShared& tshared) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tshared; return empty_type_parameter_list(); },
  [&](const TNamed& tnamed) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tnamed; return empty_type_parameter_list(); },
  [&](const TGeneric& tgeneric) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tgeneric; return empty_type_parameter_list(); },
  [&](const TPair& tpair) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tpair; return empty_type_parameter_list(); },
  [&](const TTuple& ttuple) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = ttuple; return empty_type_parameter_list(); },
  [&](const TAssoc& tassoc) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tassoc; return empty_type_parameter_list(); },
  [&](const TUnknown& tunknown) -> mlc::Array<std::shared_ptr<registry::Type>> { return empty_type_parameter_list(); }
}, (*function_type));}

bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TI32>((*type_value))) {  return true; } return false; }();}

bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TBool>((*type_value))) {  return true; } return false; }();}

bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TString>((*type_value))) {  return true; } return false; }();}

bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept{return [&]() { if (std::holds_alternative<registry::TNamed>((*receiver_type))) { auto _v_tnamed = std::get<registry::TNamed>((*receiver_type)); auto [name] = _v_tnamed; return name == mlc::String("Map"); } if (std::holds_alternative<registry::TGeneric>((*receiver_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*receiver_type)); auto [name, _w0] = _v_tgeneric; return name == mlc::String("Map"); } return false; }();}

bool is_numeric_type(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TI32>((*type_value))) {  return true; } if (std::holds_alternative<registry::TI64>((*type_value))) {  return true; } if (std::holds_alternative<registry::TF64>((*type_value))) {  return true; } if (std::holds_alternative<registry::TU8>((*type_value))) {  return true; } if (std::holds_alternative<registry::TUsize>((*type_value))) {  return true; } if (std::holds_alternative<registry::TChar>((*type_value))) {  return true; } return false; }();}

std::shared_ptr<registry::Type> arithmetic_binary_result_type(std::shared_ptr<registry::Type> left_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TString>((*left_type))) {  return std::make_shared<registry::Type>((registry::TString{})); } if (std::holds_alternative<registry::TI64>((*left_type))) {  return std::make_shared<registry::Type>((registry::TI64{})); } if (std::holds_alternative<registry::TF64>((*left_type))) {  return std::make_shared<registry::Type>((registry::TF64{})); } if (std::holds_alternative<registry::TU8>((*left_type))) {  return std::make_shared<registry::Type>((registry::TU8{})); } if (std::holds_alternative<registry::TUsize>((*left_type))) {  return std::make_shared<registry::Type>((registry::TUsize{})); } if (std::holds_alternative<registry::TChar>((*left_type))) {  return std::make_shared<registry::Type>((registry::TChar{})); } return std::make_shared<registry::Type>((registry::TI32{})); }();}

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (operation == mlc::String("+")) { return arithmetic_binary_result_type(left_type); } if (operation == mlc::String("-")) { return arithmetic_binary_result_type(left_type); } if (operation == mlc::String("*")) { return arithmetic_binary_result_type(left_type); } if (operation == mlc::String("/")) { return arithmetic_binary_result_type(left_type); } if (operation == mlc::String("%")) { return arithmetic_binary_result_type(left_type); } if (operation == mlc::String("=")) { return std::make_shared<registry::Type>((registry::TUnit{})); } return std::make_shared<registry::Type>((registry::TBool{})); }();}

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TI32& ti32) -> mlc::String { return mlc::String("i32"); },
  [&](const TString& tstring) -> mlc::String { return mlc::String("string"); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String("bool"); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String("unit"); },
  [&](const TI64& ti64) -> mlc::String { return mlc::String("i64"); },
  [&](const TF64& tf64) -> mlc::String { return mlc::String("f64"); },
  [&](const TU8& tu8) -> mlc::String { return mlc::String("u8"); },
  [&](const TUsize& tusize) -> mlc::String { return mlc::String("usize"); },
  [&](const TChar& tchar) -> mlc::String { return mlc::String("char"); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String("unknown"); },
  [&](const TArray& tarray) -> mlc::String { auto [_w0] = tarray; return mlc::String("array"); },
  [&](const TShared& tshared) -> mlc::String { auto [_w0] = tshared; return mlc::String("shared"); },
  [&](const TNamed& tnamed) -> mlc::String { auto [type_name] = tnamed; return type_name; },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [type_name, _w0] = tgeneric; return type_name; },
  [&](const TPair& tpair) -> mlc::String { auto [_w0, _w1] = tpair; return mlc::String("pair"); },
  [&](const TTuple& ttuple) -> mlc::String { auto [_w0] = ttuple; return mlc::String("tuple"); },
  [&](const TFn& tfn) -> mlc::String { auto [_w0, _w1] = tfn; return mlc::String("function"); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [parameter_name, associated_type_name] = tassoc; return parameter_name + mlc::String(".") + associated_type_name; }
}, (*type_value));}

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || is_numeric_type(left_type) && types_structurally_equal(left_type, right_type) || [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TString>((*right_type))) {  return true; } return false; }(); } return false; }();}

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || is_numeric_type(left_type) && types_structurally_equal(left_type, right_type);}

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || is_numeric_type(left_type) && types_structurally_equal(left_type, right_type) || [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TString>((*right_type))) {  return true; } return false; }(); } return false; }();}

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*left_type))) {  return true; } return [&]() { if (std::holds_alternative<registry::TUnknown>((*right_type))) {  return true; } return false; }(); }() || [&]() { if (std::holds_alternative<registry::TBool>((*left_type))) {  return [&]() { if (std::holds_alternative<registry::TBool>((*right_type))) {  return true; } return false; }(); } return false; }();}

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (method_name == mlc::String("length")) { return std::make_shared<registry::Type>((registry::TI32{})); } if (method_name == mlc::String("size")) { return std::make_shared<registry::Type>((registry::TI32{})); } if (method_name == mlc::String("to_i")) { return std::make_shared<registry::Type>((registry::TI32{})); } if (method_name == mlc::String("push")) { return std::make_shared<registry::Type>((registry::TUnit{})); } if (method_name == mlc::String("set")) { return std::make_shared<registry::Type>((registry::TUnit{})); } if (method_name == mlc::String("remove")) { return std::make_shared<registry::Type>((registry::TUnit{})); } if (method_name == mlc::String("keys")) { return std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TUnknown{})))); } if (method_name == mlc::String("values")) { return std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TUnknown{})))); } if (method_name == mlc::String("char_at")) { return std::make_shared<registry::Type>((registry::TString{})); } if (method_name == mlc::String("join")) { return std::make_shared<registry::Type>((registry::TString{})); } if (method_name == mlc::String("to_string")) { return std::make_shared<registry::Type>((registry::TString{})); } if (method_name == mlc::String("substring")) { return std::make_shared<registry::Type>((registry::TString{})); } if (method_name == mlc::String("to_lower")) { return std::make_shared<registry::Type>((registry::TString{})); } if (method_name == mlc::String("make_temp_directory")) { return std::make_shared<registry::Type>((registry::TString{})); } if (method_name == mlc::String("temp_directory_base")) { return std::make_shared<registry::Type>((registry::TString{})); } if (method_name == mlc::String("has")) { return std::make_shared<registry::Type>((registry::TBool{})); } if (method_name == mlc::String("send")) { return std::make_shared<registry::Type>((registry::TBool{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

int builtin_method_expected_argument_count(mlc::String method_name) noexcept{return [&]() { if (method_name == mlc::String("push")) { return 1; } if (method_name == mlc::String("set")) { return 2; } if (method_name == mlc::String("length")) { return 0; } if (method_name == mlc::String("size")) { return 0; } if (method_name == mlc::String("to_i")) { return 0; } if (method_name == mlc::String("to_lower")) { return 0; } if (method_name == mlc::String("char_at")) { return 1; } if (method_name == mlc::String("substring")) { return 2; } if (method_name == mlc::String("join")) { return 1; } if (method_name == mlc::String("make_temp_directory")) { return 1; } if (method_name == mlc::String("temp_directory_base")) { return 0; } if (method_name == mlc::String("to_string")) { return 0; } if (method_name == mlc::String("has")) { return 1; } if (method_name == mlc::String("get")) { return 1; } if (method_name == mlc::String("remove")) { return 1; } if (method_name == mlc::String("keys")) { return 0; } if (method_name == mlc::String("values")) { return 0; } if (method_name == mlc::String("send")) { return 1; } if (method_name == mlc::String("recv")) { return 0; } return -1; }();}

std::shared_ptr<registry::Type> substitute_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return substitution.has(name) ? substitute_type(substitution.get(name), substitution) : type_value; } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [inner] = _v_tarray; return std::make_shared<registry::Type>(registry::TArray(substitute_type(inner, substitution))); } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return std::make_shared<registry::Type>(registry::TShared(substitute_type(inner, substitution))); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [name, type_arguments] = _v_tgeneric; return std::make_shared<registry::Type>(registry::TGeneric(name, type_arguments.map([substitution](std::shared_ptr<registry::Type> argument) mutable { return substitute_type(argument, substitution); }))); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [a, b] = _v_tpair; return std::make_shared<registry::Type>(registry::TPair(substitute_type(a, substitution), substitute_type(b, substitution))); } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [parameter_types] = _v_ttuple; return std::make_shared<registry::Type>(registry::TTuple(parameter_types.map([substitution](std::shared_ptr<registry::Type> entry) mutable { return substitute_type(entry, substitution); }))); } if (std::holds_alternative<registry::TFn>((*type_value))) { auto _v_tfn = std::get<registry::TFn>((*type_value)); auto [parameter_list, return_type] = _v_tfn; return std::make_shared<registry::Type>(registry::TFn(parameter_list.map([substitution](std::shared_ptr<registry::Type> parameter) mutable { return substitute_type(parameter, substitution); }), substitute_type(return_type, substitution))); } return type_value; }();}

mlc::String operator_method_for(mlc::String operation) noexcept{return [&]() -> mlc::String { if (operation == mlc::String("+")) { return mlc::String("add"); } if (operation == mlc::String("-")) { return mlc::String("sub"); } if (operation == mlc::String("*")) { return mlc::String("mul"); } if (operation == mlc::String("/")) { return mlc::String("div"); } if (operation == mlc::String("%")) { return mlc::String("rem"); } return mlc::String(""); }();}

} // namespace semantic_type_structure
