#define main mlc_user_main
#include "identifiers.hpp"

#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"

namespace identifiers {

using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;

mlc::String resolve_identifier_in_self_context(mlc::String name, context::CodegenContext context) noexcept{
  auto self_fields = context::lookup_fields_for_context(context, context.self_type);
  auto is_known_field = (((((((name == mlc::String("errors", 6)) || (name == mlc::String("kind", 4))) || (name == mlc::String("tokens", 6))) || (name == mlc::String("line", 4))) || (name == mlc::String("col", 3))) || (name == mlc::String("inferred_type", 13))) || (name == mlc::String("type_env", 8)));
  if ((decl_index::list_contains(self_fields, name) || is_known_field))   {
    return (mlc::String("self.", 5) + cpp_naming::cpp_safe(name));
  } else   {
    auto mapped = cpp_naming::map_builtin_identifier_reference(name);
    auto resolved = context::CodegenContext_resolve(context, mapped);
    auto needs_constructor_brace = (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1)));
    if (needs_constructor_brace)     {
      return (resolved + mlc::String("{}", 2));
    } else     {
      return resolved;
    }
  }
}
mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept{
  if ((context.self_type.length() > 0))   {
    return resolve_identifier_in_self_context(name, context);
  } else   {
    auto mapped = cpp_naming::map_builtin_identifier_reference(name);
    auto resolved = context::CodegenContext_resolve(context, mapped);
    auto needs_constructor_brace = (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1)));
    if (needs_constructor_brace)     {
      return (resolved + mlc::String("{}", 2));
    } else     {
      return resolved;
    }
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_identifier_cpp(mlc::String name, context::CodegenContext context) noexcept{
  return emit_helpers::make_identifier_cpp_expression(gen_identifier(name, context));
}

} // namespace identifiers
