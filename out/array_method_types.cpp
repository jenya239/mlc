#define main mlc_user_main
#include "array_method_types.hpp"

namespace array_method_types {

bool is_array_hof_method(mlc::String method_name) noexcept{
return (((((((((((((((((method_name == mlc::String("map", 3)) || (method_name == mlc::String("filter", 6))) || (method_name == mlc::String("fold", 4))) || (method_name == mlc::String("flat_map", 8))) || (method_name == mlc::String("any", 3))) || (method_name == mlc::String("all", 3))) || (method_name == mlc::String("find_index", 10))) || (method_name == mlc::String("find", 4))) || (method_name == mlc::String("sort_by", 7))) || (method_name == mlc::String("take", 4))) || (method_name == mlc::String("drop", 4))) || (method_name == mlc::String("zip", 3))) || (method_name == mlc::String("enumerate", 9))) || (method_name == mlc::String("join", 4))) || (method_name == mlc::String("sum", 3))) || (method_name == mlc::String("group_by", 8))) || (method_name == mlc::String("flat", 4)));
}
int array_hof_expected_argument_count(mlc::String method_name) noexcept{
if ((method_name == mlc::String("fold", 4))) {
return 2;
} else if ((((method_name == mlc::String("enumerate", 9)) || (method_name == mlc::String("sum", 3))) || (method_name == mlc::String("flat", 4)))) {
return 0;
} else {
return 1;
}

}
std::shared_ptr<registry::Type> array_hof_call_result_type(std::shared_ptr<registry::Type> element_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept{
if ((method_name == mlc::String("map", 3))) {
if ((argument_inferred_types.length() > 0)) {
return std::make_shared<registry::Type>(registry::TArray{semantic_type_structure::function_return_type(argument_inferred_types[0])});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if (((method_name == mlc::String("filter", 6)) || (method_name == mlc::String("sort_by", 7)))) {
return std::make_shared<registry::Type>(registry::TArray{element_type});
} else if ((method_name == mlc::String("fold", 4))) {
if ((argument_inferred_types.length() > 0)) {
return argument_inferred_types[0];
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if ((method_name == mlc::String("flat_map", 8))) {
if ((argument_inferred_types.length() > 0)) {
auto callback_return = semantic_type_structure::function_return_type(argument_inferred_types[0]);
if (semantic_type_structure::type_is_array(callback_return)) {
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return std::make_shared<registry::Type>(registry::TArray{inner}); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*callback_return));
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if (((method_name == mlc::String("any", 3)) || (method_name == mlc::String("all", 3)))) {
return std::make_shared<registry::Type>(registry::TBool{});
} else if ((method_name == mlc::String("find_index", 10))) {
return std::make_shared<registry::Type>(registry::TI32{});
} else if ((method_name == mlc::String("find", 4))) {
return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array{element_type}});
} else if (((method_name == mlc::String("take", 4)) || (method_name == mlc::String("drop", 4)))) {
return std::make_shared<registry::Type>(registry::TArray{element_type});
} else if ((method_name == mlc::String("zip", 3))) {
if ((argument_inferred_types.length() > 0)) {
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [other_el] = tArray; return std::make_shared<registry::Type>(registry::TArray{std::make_shared<registry::Type>(registry::TPair{element_type, other_el})}); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*argument_inferred_types[0]));
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if ((method_name == mlc::String("enumerate", 9))) {
return std::make_shared<registry::Type>(registry::TArray{std::make_shared<registry::Type>(registry::TPair{std::make_shared<registry::Type>(registry::TI32{}), element_type})});
} else if ((method_name == mlc::String("join", 4))) {
return std::make_shared<registry::Type>(registry::TString{});
} else if ((method_name == mlc::String("sum", 3))) {
return std::make_shared<registry::Type>(registry::TI32{});
} else if ((method_name == mlc::String("group_by", 8))) {
if ((argument_inferred_types.length() > 0)) {
auto key_t = semantic_type_structure::function_return_type(argument_inferred_types[0]);
return std::make_shared<registry::Type>(registry::TArray{std::make_shared<registry::Type>(registry::TPair{key_t, std::make_shared<registry::Type>(registry::TArray{element_type})})});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else if ((method_name == mlc::String("flat", 4))) {
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return std::make_shared<registry::Type>(registry::TArray{inner}); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*element_type));
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}













}

} // namespace array_method_types
