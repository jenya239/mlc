#define main mlc_user_main
#include "semantic_type_structure.hpp"

#include "registry.hpp"

namespace semantic_type_structure {

using namespace registry;

mlc::String generic_type_name_from_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return name; },
[&](const registry::TI32& tI32) { return mlc::String("", 0); },
[&](const registry::TString& tString) { return mlc::String("", 0); },
[&](const registry::TBool& tBool) { return mlc::String("", 0); },
[&](const registry::TUnit& tUnit) { return mlc::String("", 0); },
[&](const registry::TI64& tI64) { return mlc::String("", 0); },
[&](const registry::TF64& tF64) { return mlc::String("", 0); },
[&](const registry::TU8& tU8) { return mlc::String("", 0); },
[&](const registry::TUsize& tUsize) { return mlc::String("", 0); },
[&](const registry::TChar& tChar) { return mlc::String("", 0); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("", 0); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return mlc::String("", 0); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return mlc::String("", 0); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return mlc::String("", 0); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return mlc::String("", 0); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return mlc::String("", 0); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return mlc::String("", 0); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("", 0); }
}, (*type_value));
}
mlc::Array<std::shared_ptr<registry::Type>> generic_type_arguments_from_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [__0, type_arguments] = tGeneric; return type_arguments; },
[&](const registry::TI32& tI32) { return empty_type_parameter_list(); },
[&](const registry::TString& tString) { return empty_type_parameter_list(); },
[&](const registry::TBool& tBool) { return empty_type_parameter_list(); },
[&](const registry::TUnit& tUnit) { return empty_type_parameter_list(); },
[&](const registry::TI64& tI64) { return empty_type_parameter_list(); },
[&](const registry::TF64& tF64) { return empty_type_parameter_list(); },
[&](const registry::TU8& tU8) { return empty_type_parameter_list(); },
[&](const registry::TUsize& tUsize) { return empty_type_parameter_list(); },
[&](const registry::TChar& tChar) { return empty_type_parameter_list(); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return empty_type_parameter_list(); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return empty_type_parameter_list(); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return empty_type_parameter_list(); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return empty_type_parameter_list(); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return empty_type_parameter_list(); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return empty_type_parameter_list(); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return empty_type_parameter_list(); },
[&](const registry::TUnknown& tUnknown) { return empty_type_parameter_list(); }
}, (*type_value));
}
mlc::Array<std::shared_ptr<registry::Type>> empty_type_parameter_list() noexcept{
  return mlc::Array<std::shared_ptr<registry::Type>>{};
}
bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return false; },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return false; },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return false; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return false; },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; }
}, (*type_value));
}
bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept{
  if ((type_is_unknown(left) || type_is_unknown(right)))   {
    return true;
  } else   {
    return std::visit(overloaded{[&](const registry::TI32& tI32) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TString& tString) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TBool& tBool) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TUnit& tUnit) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TI64& tI64) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TI64>((*__match_subject))) {
const registry::TI64& tI64 = std::get<registry::TI64>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TF64& tF64) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TF64>((*__match_subject))) {
const registry::TF64& tF64 = std::get<registry::TF64>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TU8& tU8) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TU8>((*__match_subject))) {
const registry::TU8& tU8 = std::get<registry::TU8>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TUsize& tUsize) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TUsize>((*__match_subject))) {
const registry::TUsize& tUsize = std::get<registry::TUsize>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TChar& tChar) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TChar>((*__match_subject))) {
const registry::TChar& tChar = std::get<registry::TChar>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TUnknown& tUnknown) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return false;
std::abort();
}(); },
[&](const registry::TArray& tArray) { auto [inner_left] = tArray; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner_right] = tArray; return types_structurally_equal(inner_left, inner_right);
}
return false;
std::abort();
}(); },
[&](const registry::TShared& tShared) { auto [inner_left] = tShared; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [inner_right] = tShared; return types_structurally_equal(inner_left, inner_right);
}
return false;
std::abort();
}(); },
[&](const registry::TNamed& tNamed) { auto [left_name] = tNamed; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [right_name] = tNamed; return (left_name == right_name);
}
return false;
std::abort();
}(); },
[&](const registry::TGeneric& tGeneric) { auto [left_name, left_args] = tGeneric; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [right_name, right_args] = tGeneric; return (((left_name == right_name) && (left_args.length() == right_args.length())) && zipped_shared_types_structurally_equal(left_args, right_args));
}
return false;
std::abort();
}(); },
[&](const registry::TTuple& tTuple) { auto [left_tuple_types] = tTuple; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [right_tuple_types] = tTuple; return ((left_tuple_types.length() == right_tuple_types.length()) && zipped_shared_types_structurally_equal(left_tuple_types, right_tuple_types));
}
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [right_first, right_second] = tPair; return (((left_tuple_types.length() == 2) && types_structurally_equal(left_tuple_types[0], right_first)) && types_structurally_equal(left_tuple_types[1], right_second));
}
return false;
std::abort();
}(); },
[&](const registry::TPair& tPair) { auto [left_first, left_second] = tPair; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [right_first, right_second] = tPair; return (types_structurally_equal(left_first, right_first) && types_structurally_equal(left_second, right_second));
}
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [right_tuple_types] = tTuple; return (((right_tuple_types.length() == 2) && types_structurally_equal(left_first, right_tuple_types[0])) && types_structurally_equal(left_second, right_tuple_types[1]));
}
return false;
std::abort();
}(); },
[&](const registry::TFn& tFn) { auto [left_parameters, left_return_type] = tFn; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TFn>((*__match_subject))) {
const registry::TFn& tFn = std::get<registry::TFn>((*__match_subject));
auto [right_parameters, right_return_type] = tFn; return (((left_parameters.length() == right_parameters.length()) && zipped_shared_types_structurally_equal(left_parameters, right_parameters)) && types_structurally_equal(left_return_type, right_return_type));
}
return false;
std::abort();
}(); },
[&](const registry::TAssoc& tAssoc) { auto [left_parameter, left_associated] = tAssoc; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<registry::TAssoc>((*__match_subject))) {
const registry::TAssoc& tAssoc = std::get<registry::TAssoc>((*__match_subject));
auto [right_parameter, right_associated] = tAssoc; return ((left_parameter == right_parameter) && (left_associated == right_associated));
}
return true;
std::abort();
}(); }
}, (*left));
  }
}
bool zipped_shared_types_structurally_equal(mlc::Array<std::shared_ptr<registry::Type>> left_list, mlc::Array<std::shared_ptr<registry::Type>> right_list) noexcept{
  return zipped_suffix_types_structurally_equal(left_list, right_list, 0);
}
bool zipped_suffix_types_structurally_equal(mlc::Array<std::shared_ptr<registry::Type>> left_list, mlc::Array<std::shared_ptr<registry::Type>> right_list, int index) noexcept{
  if ((index >= left_list.length()))   {
    return true;
  } else   {
    return (types_structurally_equal(left_list[index], right_list[index]) && zipped_suffix_types_structurally_equal(left_list, right_list, (index + 1)));
  }
}
bool type_is_channel(std::shared_ptr<registry::Type> type_value) noexcept{
  return (generic_type_name_from_type(type_value) == mlc::String("Channel", 7));
}
std::shared_ptr<registry::Type> channel_element_type_from_channel_type(std::shared_ptr<registry::Type> channel_type) noexcept{
  if ((generic_type_name_from_type(channel_type) == mlc::String("Channel", 7)))   {
    auto type_arguments = generic_type_arguments_from_type(channel_type);
    if ((type_arguments.length() == 1))     {
      return type_arguments[0];
    } else     {
      return std::make_shared<registry::Type>(registry::TUnknown{});
    }
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}
bool type_is_mutex(std::shared_ptr<registry::Type> type_value) noexcept{
  return (generic_type_name_from_type(type_value) == mlc::String("Mutex", 5));
}
std::shared_ptr<registry::Type> mutex_inner_type_from_mutex_type(std::shared_ptr<registry::Type> mutex_type) noexcept{
  if ((generic_type_name_from_type(mutex_type) == mlc::String("Mutex", 5)))   {
    auto type_arguments = generic_type_arguments_from_type(mutex_type);
    if ((type_arguments.length() == 1))     {
      return type_arguments[0];
    } else     {
      return std::make_shared<registry::Type>(registry::TUnknown{});
    }
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}
bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [__0] = tArray; return true; },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return false; },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return false; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return false; },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; },
[&](const registry::TUnknown& tUnknown) { return false; }
}, (*type_value));
}
std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept{
  return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*array_type));
}
bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return true; },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return false; },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return false; },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return false; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return false; },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; },
[&](const registry::TUnknown& tUnknown) { return false; }
}, (*type_value));
}
std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept{
  return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [inner] = tShared; return inner; },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*shared_pointer_type));
}
bool type_is_extern_fn_generic(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, __1] = tGeneric; return (name == mlc::String("__ExternFn", 10));
}
return false;
std::abort();
}();
}
bool type_is_function(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return true; },
[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return (name == mlc::String("__ExternFn", 10)); },
[&](const registry::TI32& tI32) { return false; },
[&](const registry::TString& tString) { return false; },
[&](const registry::TBool& tBool) { return false; },
[&](const registry::TUnit& tUnit) { return false; },
[&](const registry::TI64& tI64) { return false; },
[&](const registry::TF64& tF64) { return false; },
[&](const registry::TU8& tU8) { return false; },
[&](const registry::TUsize& tUsize) { return false; },
[&](const registry::TChar& tChar) { return false; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return false; },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return false; },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return false; },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; },
[&](const registry::TUnknown& tUnknown) { return false; }
}, (*type_value));
}
std::shared_ptr<registry::Type> function_return_type(std::shared_ptr<registry::Type> function_type) noexcept{
  return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, return_type] = tFn; return return_type; },
[&](const registry::TGeneric& tGeneric) { auto [name, type_arguments] = tGeneric; return (((name == mlc::String("__ExternFn", 10)) && (type_arguments.length() >= 1)) ? (type_arguments[0]) : (std::make_shared<registry::Type>(registry::TUnknown{}))); },
[&](const registry::TI32& tI32) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TBool& tBool) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnit& tUnit) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TI64& tI64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TF64& tF64) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TU8& tU8) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUsize& tUsize) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TChar& tChar) { return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return std::make_shared<registry::Type>(registry::TUnknown{}); },
[&](const registry::TUnknown& tUnknown) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*function_type));
}
mlc::Array<std::shared_ptr<registry::Type>> function_parameter_list(std::shared_ptr<registry::Type> function_type) noexcept{
  return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [parameters, __1] = tFn; return parameters; },
[&](const registry::TGeneric& tGeneric) { auto [name, type_arguments] = tGeneric; return ((name == mlc::String("__ExternFn", 10)) ? (type_arguments.drop(1)) : (empty_type_parameter_list())); },
[&](const registry::TI32& tI32) { return empty_type_parameter_list(); },
[&](const registry::TString& tString) { return empty_type_parameter_list(); },
[&](const registry::TBool& tBool) { return empty_type_parameter_list(); },
[&](const registry::TUnit& tUnit) { return empty_type_parameter_list(); },
[&](const registry::TI64& tI64) { return empty_type_parameter_list(); },
[&](const registry::TF64& tF64) { return empty_type_parameter_list(); },
[&](const registry::TU8& tU8) { return empty_type_parameter_list(); },
[&](const registry::TUsize& tUsize) { return empty_type_parameter_list(); },
[&](const registry::TChar& tChar) { return empty_type_parameter_list(); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return empty_type_parameter_list(); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return empty_type_parameter_list(); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return empty_type_parameter_list(); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return empty_type_parameter_list(); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return empty_type_parameter_list(); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return empty_type_parameter_list(); },
[&](const registry::TUnknown& tUnknown) { return empty_type_parameter_list(); }
}, (*function_type));
}
bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept{
  return [&]() -> bool {
auto __match_subject = receiver_type;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return (name == mlc::String("Map", 3));
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, __1] = tGeneric; return (name == mlc::String("Map", 3));
}
return false;
std::abort();
}();
}
bool is_numeric_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TI64>((*__match_subject))) {
const registry::TI64& tI64 = std::get<registry::TI64>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TF64>((*__match_subject))) {
const registry::TF64& tF64 = std::get<registry::TF64>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TU8>((*__match_subject))) {
const registry::TU8& tU8 = std::get<registry::TU8>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TUsize>((*__match_subject))) {
const registry::TUsize& tUsize = std::get<registry::TUsize>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TChar>((*__match_subject))) {
const registry::TChar& tChar = std::get<registry::TChar>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
bool types_compatible_for_call_argument(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type) noexcept{
  if (types_structurally_equal(expected_type, actual_type))   {
    return true;
  } else   {
    return [&]() -> bool {
auto __match_subject = expected_type;
if (std::holds_alternative<registry::TI64>((*__match_subject))) {
const registry::TI64& tI64 = std::get<registry::TI64>((*__match_subject));
return [&]() -> bool {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TChar>((*__match_subject))) {
const registry::TChar& tChar = std::get<registry::TChar>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return [&]() -> bool {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TChar>((*__match_subject))) {
const registry::TChar& tChar = std::get<registry::TChar>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
return false;
std::abort();
}();
  }
}
bool zipped_type_arguments_assignment_compatible(mlc::Array<std::shared_ptr<registry::Type>> expected_arguments, mlc::Array<std::shared_ptr<registry::Type>> actual_arguments, int index) noexcept{
  if ((index >= expected_arguments.length()))   {
    return true;
  } else   {
    return (types_assignment_compatible(expected_arguments[index], actual_arguments[index]) && zipped_type_arguments_assignment_compatible(expected_arguments, actual_arguments, (index + 1)));
  }
}
bool extern_fn_compatible_with_function(mlc::Array<std::shared_ptr<registry::Type>> expected_extern_arguments, mlc::Array<std::shared_ptr<registry::Type>> actual_parameters, std::shared_ptr<registry::Type> actual_return_type) noexcept{
  if ((expected_extern_arguments.length() == 0))   {
    return false;
  } else if (((expected_extern_arguments.length() - 1) != actual_parameters.length()))   {
    return false;
  } else if ((!types_assignment_compatible(expected_extern_arguments[0], actual_return_type)))   {
    return false;
  } else   {
    return zipped_type_arguments_assignment_compatible(expected_extern_arguments.drop(1), actual_parameters, 0);
  }
}
bool types_assignment_compatible(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type) noexcept{
  if (types_structurally_equal(expected_type, actual_type))   {
    return true;
  } else if (types_compatible_for_call_argument(expected_type, actual_type))   {
    return true;
  } else   {
    return [&]() -> bool {
auto __match_subject = expected_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [expected_name, expected_arguments] = tGeneric; return [&]() -> bool {
  if ((expected_name == mlc::String("__ExternFn", 10)))   {
    return [&]() -> bool {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TFn>((*__match_subject))) {
const registry::TFn& tFn = std::get<registry::TFn>((*__match_subject));
auto [actual_parameters, actual_return_type] = tFn; return extern_fn_compatible_with_function(expected_arguments, actual_parameters, actual_return_type);
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [actual_name, actual_arguments] = tGeneric; return (((expected_name == actual_name) && (expected_arguments.length() == actual_arguments.length())) && zipped_type_arguments_assignment_compatible(expected_arguments, actual_arguments, 0));
}
return false;
std::abort();
}();
  } else   {
    return [&]() -> bool {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [actual_name, actual_arguments] = tGeneric; return (((expected_name == actual_name) && (expected_arguments.length() == actual_arguments.length())) && zipped_type_arguments_assignment_compatible(expected_arguments, actual_arguments, 0));
}
return false;
std::abort();
}();
  }
}();
}
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [expected_element] = tArray; return [&]() -> bool {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [actual_element] = tArray; return types_assignment_compatible(expected_element, actual_element);
}
return false;
std::abort();
}();
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [expected_element] = tShared; return [&]() -> bool {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [actual_element] = tShared; return types_assignment_compatible(expected_element, actual_element);
}
return false;
std::abort();
}();
}
return false;
std::abort();
}();
  }
}
std::shared_ptr<registry::Type> arithmetic_binary_result_type(std::shared_ptr<registry::Type> left_type) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return std::make_shared<registry::Type>(registry::TString{});
}
if (std::holds_alternative<registry::TI64>((*__match_subject))) {
const registry::TI64& tI64 = std::get<registry::TI64>((*__match_subject));
return std::make_shared<registry::Type>(registry::TI64{});
}
if (std::holds_alternative<registry::TF64>((*__match_subject))) {
const registry::TF64& tF64 = std::get<registry::TF64>((*__match_subject));
return std::make_shared<registry::Type>(registry::TF64{});
}
if (std::holds_alternative<registry::TU8>((*__match_subject))) {
const registry::TU8& tU8 = std::get<registry::TU8>((*__match_subject));
return std::make_shared<registry::Type>(registry::TU8{});
}
if (std::holds_alternative<registry::TUsize>((*__match_subject))) {
const registry::TUsize& tUsize = std::get<registry::TUsize>((*__match_subject));
return std::make_shared<registry::Type>(registry::TUsize{});
}
if (std::holds_alternative<registry::TChar>((*__match_subject))) {
const registry::TChar& tChar = std::get<registry::TChar>((*__match_subject));
return std::make_shared<registry::Type>(registry::TChar{});
}
return std::make_shared<registry::Type>(registry::TI32{});
std::abort();
}();
}
std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept{
  if ((operation == mlc::String("+", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("-", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("*", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("/", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("%", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("&", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("|", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("^", 1)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("<<", 2)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String(">>", 2)))   {
    return arithmetic_binary_result_type(left_type);
  } else if ((operation == mlc::String("=", 1)))   {
    return std::make_shared<registry::Type>(registry::TUnit{});
  } else   {
    return std::make_shared<registry::Type>(registry::TBool{});
  }
}
mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept{
  return std::visit(overloaded{[&](const registry::TI32& tI32) { return mlc::String("i32", 3); },
[&](const registry::TString& tString) { return mlc::String("string", 6); },
[&](const registry::TBool& tBool) { return mlc::String("bool", 4); },
[&](const registry::TUnit& tUnit) { return mlc::String("unit", 4); },
[&](const registry::TI64& tI64) { return mlc::String("i64", 3); },
[&](const registry::TF64& tF64) { return mlc::String("f64", 3); },
[&](const registry::TU8& tU8) { return mlc::String("u8", 2); },
[&](const registry::TUsize& tUsize) { return mlc::String("usize", 5); },
[&](const registry::TChar& tChar) { return mlc::String("char", 4); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("unknown", 7); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return mlc::String("array", 5); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("shared", 6); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return type_name; },
[&](const registry::TGeneric& tGeneric) { auto [type_name, __1] = tGeneric; return ((type_name == mlc::String("__ExternFn", 10)) ? (mlc::String("extern fn", 9)) : (type_name)); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return mlc::String("pair", 4); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return mlc::String("tuple", 5); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return mlc::String("function", 8); },
[&](const registry::TAssoc& tAssoc) { auto [parameter_name, associated_type_name] = tAssoc; return ((parameter_name + mlc::String(".", 1)) + associated_type_name); }
}, (*type_value));
}
bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
  return (([&]() -> bool {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return [&]() -> bool {
auto __match_subject = right_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return false;
std::abort();
}();
std::abort();
}() || (is_numeric_type(left_type) && types_structurally_equal(left_type, right_type))) || [&]() -> bool {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return [&]() -> bool {
auto __match_subject = right_type;
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
return false;
std::abort();
}());
}
bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
  return ([&]() -> bool {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return [&]() -> bool {
auto __match_subject = right_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return false;
std::abort();
}();
std::abort();
}() || (is_numeric_type(left_type) && types_structurally_equal(left_type, right_type)));
}
bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
  return (([&]() -> bool {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return [&]() -> bool {
auto __match_subject = right_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return false;
std::abort();
}();
std::abort();
}() || (is_numeric_type(left_type) && types_structurally_equal(left_type, right_type))) || [&]() -> bool {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return [&]() -> bool {
auto __match_subject = right_type;
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
return false;
std::abort();
}());
}
bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{
  return ([&]() -> bool {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return [&]() -> bool {
auto __match_subject = right_type;
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return true;
}
return false;
std::abort();
}();
std::abort();
}() || [&]() -> bool {
auto __match_subject = left_type;
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return [&]() -> bool {
auto __match_subject = right_type;
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
return false;
std::abort();
}());
}
std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept{
  if ((method_name == mlc::String("length", 6)))   {
    return std::make_shared<registry::Type>(registry::TI32{});
  } else if ((method_name == mlc::String("size", 4)))   {
    return std::make_shared<registry::Type>(registry::TI32{});
  } else if ((method_name == mlc::String("to_i", 4)))   {
    return std::make_shared<registry::Type>(registry::TI32{});
  } else if ((method_name == mlc::String("push", 4)))   {
    return std::make_shared<registry::Type>(registry::TUnit{});
  } else if ((method_name == mlc::String("set", 3)))   {
    return std::make_shared<registry::Type>(registry::TUnit{});
  } else if ((method_name == mlc::String("remove", 6)))   {
    return std::make_shared<registry::Type>(registry::TUnit{});
  } else if ((method_name == mlc::String("keys", 4)))   {
    return std::make_shared<registry::Type>(registry::TArray{std::make_shared<registry::Type>(registry::TUnknown{})});
  } else if ((method_name == mlc::String("values", 6)))   {
    return std::make_shared<registry::Type>(registry::TArray{std::make_shared<registry::Type>(registry::TUnknown{})});
  } else if ((method_name == mlc::String("char_at", 7)))   {
    return std::make_shared<registry::Type>(registry::TString{});
  } else if ((method_name == mlc::String("join", 4)))   {
    return std::make_shared<registry::Type>(registry::TString{});
  } else if ((method_name == mlc::String("to_string", 9)))   {
    return std::make_shared<registry::Type>(registry::TString{});
  } else if ((method_name == mlc::String("substring", 9)))   {
    return std::make_shared<registry::Type>(registry::TString{});
  } else if ((method_name == mlc::String("to_lower", 8)))   {
    return std::make_shared<registry::Type>(registry::TString{});
  } else if ((method_name == mlc::String("make_temp_directory", 19)))   {
    return std::make_shared<registry::Type>(registry::TString{});
  } else if ((method_name == mlc::String("temp_directory_base", 19)))   {
    return std::make_shared<registry::Type>(registry::TString{});
  } else if ((method_name == mlc::String("has", 3)))   {
    return std::make_shared<registry::Type>(registry::TBool{});
  } else if ((method_name == mlc::String("send", 4)))   {
    return std::make_shared<registry::Type>(registry::TBool{});
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}
int builtin_method_expected_argument_count(mlc::String method_name) noexcept{
  if ((method_name == mlc::String("push", 4)))   {
    return 1;
  } else if ((method_name == mlc::String("set", 3)))   {
    return 2;
  } else if ((method_name == mlc::String("length", 6)))   {
    return 0;
  } else if ((method_name == mlc::String("size", 4)))   {
    return 0;
  } else if ((method_name == mlc::String("to_i", 4)))   {
    return 0;
  } else if ((method_name == mlc::String("to_lower", 8)))   {
    return 0;
  } else if ((method_name == mlc::String("char_at", 7)))   {
    return 1;
  } else if ((method_name == mlc::String("substring", 9)))   {
    return 2;
  } else if ((method_name == mlc::String("join", 4)))   {
    return 1;
  } else if ((method_name == mlc::String("make_temp_directory", 19)))   {
    return 1;
  } else if ((method_name == mlc::String("temp_directory_base", 19)))   {
    return 0;
  } else if ((method_name == mlc::String("to_string", 9)))   {
    return 0;
  } else if ((method_name == mlc::String("has", 3)))   {
    return 1;
  } else if ((method_name == mlc::String("get", 3)))   {
    return 1;
  } else if ((method_name == mlc::String("remove", 6)))   {
    return 1;
  } else if ((method_name == mlc::String("keys", 4)))   {
    return 0;
  } else if ((method_name == mlc::String("values", 6)))   {
    return 0;
  } else if ((method_name == mlc::String("send", 4)))   {
    return 1;
  } else if ((method_name == mlc::String("recv", 4)))   {
    return 0;
  } else   {
    return (-1);
  }
}
std::shared_ptr<registry::Type> substitute_named_type_for_map(mlc::String name, std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  if (substitution.has(name))   {
    return substitute_type(substitution.get(name), substitution);
  } else   {
    return type_value;
  }
}
std::shared_ptr<registry::Type> substitute_array_type_for_map(std::shared_ptr<registry::Type> inner, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  return std::make_shared<registry::Type>(registry::TArray{substitute_type(inner, substitution)});
}
std::shared_ptr<registry::Type> substitute_shared_type_for_map(std::shared_ptr<registry::Type> inner, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  return std::make_shared<registry::Type>(registry::TShared{substitute_type(inner, substitution)});
}
std::shared_ptr<registry::Type> substitute_generic_type_for_map(mlc::String name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  return std::make_shared<registry::Type>(registry::TGeneric{name, type_arguments.map([=](std::shared_ptr<registry::Type> argument) mutable { return substitute_type(argument, substitution); })});
}
std::shared_ptr<registry::Type> substitute_pair_type_for_map(std::shared_ptr<registry::Type> first, std::shared_ptr<registry::Type> second, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  return std::make_shared<registry::Type>(registry::TPair{substitute_type(first, substitution), substitute_type(second, substitution)});
}
std::shared_ptr<registry::Type> substitute_tuple_type_for_map(mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  return std::make_shared<registry::Type>(registry::TTuple{parameter_types.map([=](std::shared_ptr<registry::Type> entry) mutable { return substitute_type(entry, substitution); })});
}
std::shared_ptr<registry::Type> substitute_function_type_for_map(mlc::Array<std::shared_ptr<registry::Type>> parameter_list, std::shared_ptr<registry::Type> return_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  return std::make_shared<registry::Type>(registry::TFn{parameter_list.map([=](std::shared_ptr<registry::Type> parameter) mutable { return substitute_type(parameter, substitution); }), substitute_type(return_type, substitution)});
}
std::shared_ptr<registry::Type> substitute_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
  return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return substitute_named_type_for_map(name, type_value, substitution); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return substitute_array_type_for_map(inner, substitution); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return substitute_shared_type_for_map(inner, substitution); },
[&](const registry::TGeneric& tGeneric) { auto [name, type_arguments] = tGeneric; return substitute_generic_type_for_map(name, type_arguments, substitution); },
[&](const registry::TPair& tPair) { auto [first, second] = tPair; return substitute_pair_type_for_map(first, second, substitution); },
[&](const registry::TTuple& tTuple) { auto [parameter_types] = tTuple; return substitute_tuple_type_for_map(parameter_types, substitution); },
[&](const registry::TFn& tFn) { auto [parameter_list, return_type] = tFn; return substitute_function_type_for_map(parameter_list, return_type, substitution); },
[&](const registry::TI32& tI32) { return type_value; },
[&](const registry::TString& tString) { return type_value; },
[&](const registry::TBool& tBool) { return type_value; },
[&](const registry::TUnit& tUnit) { return type_value; },
[&](const registry::TI64& tI64) { return type_value; },
[&](const registry::TF64& tF64) { return type_value; },
[&](const registry::TU8& tU8) { return type_value; },
[&](const registry::TUsize& tUsize) { return type_value; },
[&](const registry::TChar& tChar) { return type_value; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return type_value; },
[&](const registry::TUnknown& tUnknown) { return type_value; }
}, (*type_value));
}
mlc::String operator_method_for(mlc::String operation) noexcept{
  if ((operation == mlc::String("+", 1)))   {
    return mlc::String("add", 3);
  } else if ((operation == mlc::String("-", 1)))   {
    return mlc::String("sub", 3);
  } else if ((operation == mlc::String("*", 1)))   {
    return mlc::String("mul", 3);
  } else if ((operation == mlc::String("/", 1)))   {
    return mlc::String("div", 3);
  } else if ((operation == mlc::String("%", 1)))   {
    return mlc::String("rem", 3);
  } else   {
    return mlc::String("", 0);
  }
}

} // namespace semantic_type_structure
