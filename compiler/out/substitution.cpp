#define main mlc_user_main
#include "substitution.hpp"

#include "registry.hpp"

namespace substitution {

using namespace registry;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution_from_generic(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
  auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  [&]() {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [type_name, type_arguments] = tGeneric; [&]() {
auto parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
auto index = 0;
return [&]() {
while (((index < parameter_names.length()) && (index < type_arguments.length()))) {
substitution.set(parameter_names[index], type_arguments[index]);
(index = mlc::arith::checked_add(index, 1));
}
}();
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
  return substitution;
}

} // namespace substitution
