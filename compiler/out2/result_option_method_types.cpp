#include "result_option_method_types.hpp"

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace result_option_method_types {

using namespace registry;
using namespace semantic_type_structure;

bool is_result_generic(std::shared_ptr<registry::Type> t) noexcept;

bool is_option_generic(std::shared_ptr<registry::Type> t) noexcept;

bool should_skip_array_hof_for_receiver(std::shared_ptr<registry::Type> t) noexcept;

bool is_result_combinator_method(mlc::String method_name) noexcept;

bool is_option_combinator_method(mlc::String method_name) noexcept;

bool should_infer_result_option_combinator(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

int result_option_hof_expected_argument_count(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> result_ok_type(std::shared_ptr<registry::Type> result_type) noexcept;

std::shared_ptr<registry::Type> result_err_type(std::shared_ptr<registry::Type> result_type) noexcept;

std::shared_ptr<registry::Type> option_inner_type(std::shared_ptr<registry::Type> opt_type) noexcept;

std::shared_ptr<registry::Type> result_option_hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> arg_inferred_types) noexcept;

bool is_result_generic(std::shared_ptr<registry::Type> t) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*t))) { auto _v_tgeneric = std::get<registry::TGeneric>((*t)); auto [name, type_args] = _v_tgeneric; return name == mlc::String("Result") && type_args.size() == 2; } if (std::holds_alternative<registry::TNamed>((*t))) { auto _v_tnamed = std::get<registry::TNamed>((*t)); auto [name] = _v_tnamed; return name == mlc::String("Result"); } return false; }();}

bool is_option_generic(std::shared_ptr<registry::Type> t) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*t))) { auto _v_tgeneric = std::get<registry::TGeneric>((*t)); auto [name, type_args] = _v_tgeneric; return name == mlc::String("Option") && type_args.size() == 1; } if (std::holds_alternative<registry::TNamed>((*t))) { auto _v_tnamed = std::get<registry::TNamed>((*t)); auto [name] = _v_tnamed; return name == mlc::String("Option"); } return false; }();}

bool should_skip_array_hof_for_receiver(std::shared_ptr<registry::Type> t) noexcept{return is_result_generic(t) || is_option_generic(t);}

bool is_result_combinator_method(mlc::String method_name) noexcept{return method_name == mlc::String("map") || method_name == mlc::String("map_err") || method_name == mlc::String("and_then") || method_name == mlc::String("or_else") || method_name == mlc::String("unwrap_or") || method_name == mlc::String("unwrap_or_else") || method_name == mlc::String("ok");}

bool is_option_combinator_method(mlc::String method_name) noexcept{return method_name == mlc::String("map") || method_name == mlc::String("and_then") || method_name == mlc::String("or_else") || method_name == mlc::String("unwrap_or") || method_name == mlc::String("filter") || method_name == mlc::String("ok_or");}

bool should_infer_result_option_combinator(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return is_result_generic(receiver_type) && is_result_combinator_method(method_name) || is_option_generic(receiver_type) && is_option_combinator_method(method_name);}

int result_option_hof_expected_argument_count(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return method_name == mlc::String("ok") ? 0 : 1;}

std::shared_ptr<registry::Type> result_ok_type(std::shared_ptr<registry::Type> result_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*result_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*result_type)); auto [name, a] = _v_tgeneric; return name == mlc::String("Result") && a.size() >= 1 ? a[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } if (std::holds_alternative<registry::TNamed>((*result_type))) { auto _v_tnamed = std::get<registry::TNamed>((*result_type)); auto [name] = _v_tnamed; return name == mlc::String("Result") ? std::make_shared<registry::Type>((registry::TUnknown{})) : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

std::shared_ptr<registry::Type> result_err_type(std::shared_ptr<registry::Type> result_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*result_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*result_type)); auto [name, a] = _v_tgeneric; return name == mlc::String("Result") && a.size() >= 2 ? a[1] : std::make_shared<registry::Type>((registry::TUnknown{})); } if (std::holds_alternative<registry::TNamed>((*result_type))) { auto _v_tnamed = std::get<registry::TNamed>((*result_type)); auto [name] = _v_tnamed; return name == mlc::String("Result") ? std::make_shared<registry::Type>((registry::TUnknown{})) : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

std::shared_ptr<registry::Type> option_inner_type(std::shared_ptr<registry::Type> opt_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*opt_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*opt_type)); auto [name, a] = _v_tgeneric; return name == mlc::String("Option") && a.size() >= 1 ? a[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } if (std::holds_alternative<registry::TNamed>((*opt_type))) { auto _v_tnamed = std::get<registry::TNamed>((*opt_type)); auto [name] = _v_tnamed; return name == mlc::String("Option") ? std::make_shared<registry::Type>((registry::TUnknown{})) : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

std::shared_ptr<registry::Type> result_option_hof_call_result_type(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> arg_inferred_types) noexcept{
return is_result_generic(receiver_type) ? [&]() -> std::shared_ptr<registry::Type> { 
  std::shared_ptr<registry::Type> t_type = result_ok_type(receiver_type);
  std::shared_ptr<registry::Type> e_type = result_err_type(receiver_type);
  return method_name == mlc::String("map") ? arg_inferred_types.size() > 0 ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Result"), mlc::Array<std::shared_ptr<registry::Type>>{semantic_type_structure::function_return_type(arg_inferred_types[0]), e_type})) : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("map_err") ? arg_inferred_types.size() > 0 ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Result"), mlc::Array<std::shared_ptr<registry::Type>>{t_type, semantic_type_structure::function_return_type(arg_inferred_types[0])})) : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("and_then") || method_name == mlc::String("or_else") ? arg_inferred_types.size() > 0 ? semantic_type_structure::function_return_type(arg_inferred_types[0]) : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("unwrap_or") || method_name == mlc::String("unwrap_or_else") ? t_type : method_name == mlc::String("ok") ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{t_type})) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : is_option_generic(receiver_type) ? [&]() -> std::shared_ptr<registry::Type> { 
  std::shared_ptr<registry::Type> opt_t = option_inner_type(receiver_type);
  return method_name == mlc::String("map") ? arg_inferred_types.size() > 0 ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{semantic_type_structure::function_return_type(arg_inferred_types[0])})) : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("and_then") ? arg_inferred_types.size() > 0 ? semantic_type_structure::function_return_type(arg_inferred_types[0]) : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("or_else") ? arg_inferred_types.size() > 0 ? semantic_type_structure::function_return_type(arg_inferred_types[0]) : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("unwrap_or") ? opt_t : method_name == mlc::String("filter") ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{opt_t})) : method_name == mlc::String("ok_or") ? arg_inferred_types.size() > 0 ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Result"), mlc::Array<std::shared_ptr<registry::Type>>{opt_t, arg_inferred_types[0]})) : std::make_shared<registry::Type>((registry::TUnknown{})) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
}

} // namespace result_option_method_types
