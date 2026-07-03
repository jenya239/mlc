#define main mlc_user_main
#include "result_option_method_types.hpp"

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace result_option_method_types {

using namespace registry;
using namespace semantic_type_structure;

bool is_result_generic(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return ((name == mlc::String("Result", 6)) && (type_arguments.length() == 2));
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return (name == mlc::String("Result", 6));
}
return false;
std::abort();
}();
}
bool is_option_generic(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return ((name == mlc::String("Option", 6)) && (type_arguments.length() == 1));
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return (name == mlc::String("Option", 6));
}
return false;
std::abort();
}();
}
bool should_skip_array_hof_for_receiver(std::shared_ptr<registry::Type> type_value) noexcept{
  return (is_result_generic(type_value) || is_option_generic(type_value));
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
  if ((method_name == mlc::String("ok", 2)))   {
    return 0;
  } else   {
    return 1;
  }
}
std::shared_ptr<registry::Type> result_ok_type(std::shared_ptr<registry::Type> result_type) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = result_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return [&]() -> std::shared_ptr<registry::Type> {
  if (((name == mlc::String("Result", 6)) && (type_arguments.length() >= 1)))   {
    return type_arguments[0];
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}();
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return ((name == mlc::String("Result", 6)) ? (std::make_shared<registry::Type>(registry::TUnknown{})) : (std::make_shared<registry::Type>(registry::TUnknown{})));
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}
std::shared_ptr<registry::Type> result_err_type(std::shared_ptr<registry::Type> result_type) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = result_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return [&]() -> std::shared_ptr<registry::Type> {
  if (((name == mlc::String("Result", 6)) && (type_arguments.length() >= 2)))   {
    return type_arguments[1];
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}();
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return ((name == mlc::String("Result", 6)) ? (std::make_shared<registry::Type>(registry::TUnknown{})) : (std::make_shared<registry::Type>(registry::TUnknown{})));
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}
std::shared_ptr<registry::Type> option_inner_type(std::shared_ptr<registry::Type> option_type) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = option_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return [&]() -> std::shared_ptr<registry::Type> {
  if (((name == mlc::String("Option", 6)) && (type_arguments.length() >= 1)))   {
    return type_arguments[0];
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}();
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return ((name == mlc::String("Option", 6)) ? (std::make_shared<registry::Type>(registry::TUnknown{})) : (std::make_shared<registry::Type>(registry::TUnknown{})));
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}
std::shared_ptr<registry::Type> result_option_hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept{
  if (is_result_generic(receiver_type))   {
    auto success_type = result_ok_type(receiver_type);
    auto error_type = result_err_type(receiver_type);
    if ((method_name == mlc::String("map", 3)))     {
      if ((argument_inferred_types.length() > 0))       {
        return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Result", 6), mlc::Array<std::shared_ptr<registry::Type>>{semantic_type_structure::function_return_type(argument_inferred_types[0]), error_type}});
      } else       {
        return std::make_shared<registry::Type>(registry::TUnknown{});
      }
    } else if ((method_name == mlc::String("map_err", 7)))     {
      if ((argument_inferred_types.length() > 0))       {
        return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Result", 6), mlc::Array<std::shared_ptr<registry::Type>>{success_type, semantic_type_structure::function_return_type(argument_inferred_types[0])}});
      } else       {
        return std::make_shared<registry::Type>(registry::TUnknown{});
      }
    } else if (((method_name == mlc::String("and_then", 8)) || (method_name == mlc::String("or_else", 7))))     {
      if ((argument_inferred_types.length() > 0))       {
        return semantic_type_structure::function_return_type(argument_inferred_types[0]);
      } else       {
        return std::make_shared<registry::Type>(registry::TUnknown{});
      }
    } else if (((method_name == mlc::String("unwrap_or", 9)) || (method_name == mlc::String("unwrap_or_else", 14))))     {
      return success_type;
    } else if ((method_name == mlc::String("ok", 2)))     {
      return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array<std::shared_ptr<registry::Type>>{success_type}});
    } else     {
      return std::make_shared<registry::Type>(registry::TUnknown{});
    }
  } else if (is_option_generic(receiver_type))   {
    auto inner_option_type = option_inner_type(receiver_type);
    if ((method_name == mlc::String("map", 3)))     {
      if ((argument_inferred_types.length() > 0))       {
        return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array<std::shared_ptr<registry::Type>>{semantic_type_structure::function_return_type(argument_inferred_types[0])}});
      } else       {
        return std::make_shared<registry::Type>(registry::TUnknown{});
      }
    } else if ((method_name == mlc::String("and_then", 8)))     {
      if ((argument_inferred_types.length() > 0))       {
        return semantic_type_structure::function_return_type(argument_inferred_types[0]);
      } else       {
        return std::make_shared<registry::Type>(registry::TUnknown{});
      }
    } else if ((method_name == mlc::String("or_else", 7)))     {
      if ((argument_inferred_types.length() > 0))       {
        return semantic_type_structure::function_return_type(argument_inferred_types[0]);
      } else       {
        return std::make_shared<registry::Type>(registry::TUnknown{});
      }
    } else if ((method_name == mlc::String("unwrap_or", 9)))     {
      return inner_option_type;
    } else if ((method_name == mlc::String("filter", 6)))     {
      return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array<std::shared_ptr<registry::Type>>{inner_option_type}});
    } else if ((method_name == mlc::String("ok_or", 5)))     {
      if ((argument_inferred_types.length() > 0))       {
        return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Result", 6), mlc::Array<std::shared_ptr<registry::Type>>{inner_option_type, argument_inferred_types[0]}});
      } else       {
        return std::make_shared<registry::Type>(registry::TUnknown{});
      }
    } else     {
      return std::make_shared<registry::Type>(registry::TUnknown{});
    }
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}

} // namespace result_option_method_types
