#include "substitution.hpp"

#include "registry.hpp"

namespace substitution {

using namespace registry;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution_from_generic(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution_from_generic(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
[&]() -> void { if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [type_name, type_arguments] = _v_tgeneric; mlc::Array<mlc::String> parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name); int index = 0; [&]() { 
  while (index < parameter_names.size() && index < type_arguments.size()){
{
substitution.set(parameter_names[index], type_arguments[index]);
index = index + 1;
}
}
 }(); return; }  }();
return substitution;
}

} // namespace substitution
