#include "identifiers.hpp"

#include "context.hpp"
#include "cpp_naming.hpp"

namespace identifiers {

using namespace context;
using namespace cpp_naming;

mlc::String resolve_identifier_in_self_context(mlc::String name, context::CodegenContext context) noexcept;

mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept;

mlc::String resolve_identifier_in_self_context(mlc::String name, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> self_fields = context::lookup_fields(context.field_orders, context.self_type);
bool is_known_field = name == mlc::String("errors") || name == mlc::String("kind") || name == mlc::String("tokens") || name == mlc::String("line") || name == mlc::String("col") || name == mlc::String("inferred_type") || name == mlc::String("type_env");
return context::list_contains(self_fields, name) || is_known_field ? mlc::String("self.") + cpp_naming::cpp_safe(name) : [&]() -> mlc::String { 
  mlc::String mapped = cpp_naming::map_builtin(name);
  mlc::String resolved = context::context_resolve(context, mapped);
  bool needs_constructor_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_constructor_brace ? resolved + mlc::String("{}") : resolved;
 }();
}

mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept{return context.self_type.length() > 0 ? resolve_identifier_in_self_context(name, context) : [&]() -> mlc::String { 
  mlc::String mapped = cpp_naming::map_builtin(name);
  mlc::String resolved = context::context_resolve(context, mapped);
  bool needs_constructor_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_constructor_brace ? resolved + mlc::String("{}") : resolved;
 }();}

} // namespace identifiers
