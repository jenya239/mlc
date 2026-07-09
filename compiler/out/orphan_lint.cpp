#define main mlc_user_main
#include "orphan_lint.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace orphan_lint {

using namespace ast;
using namespace registry;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
  auto empty = mlc::Array<ast::Diagnostic>{};
  return empty;
}
bool extend_module_owns_type_or_trait(registry::TypeRegistry registry, mlc::String type_name, mlc::String trait_name, mlc::String extend_path) noexcept{
  auto type_path = registry::TypeRegistry_defining_path_for_type(registry, type_name);
  auto trait_path = registry::TypeRegistry_defining_path_for_trait(registry, registry::trait_base_name(trait_name));
  auto owns_type = ((type_path.length() > 0) && (type_path == extend_path));
  auto owns_trait = ((trait_path.length() > 0) && (trait_path == extend_path));
  return (owns_type || owns_trait);
}
bool extend_is_orphan_impl(registry::TypeRegistry registry, mlc::String type_name, mlc::String trait_name, mlc::String extend_path) noexcept{
  if ((trait_name.length() == 0))   {
    return false;
  } else if ((extend_path.length() == 0))   {
    return false;
  } else   {
    auto type_path = registry::TypeRegistry_defining_path_for_type(registry, type_name);
    auto trait_path = registry::TypeRegistry_defining_path_for_trait(registry, registry::trait_base_name(trait_name));
    if (((type_path.length() == 0) && (trait_path.length() == 0)))     {
      return false;
    } else     {
      return (!extend_module_owns_type_or_trait(registry, type_name, trait_name, extend_path));
    }
  }
}
ast::Diagnostic orphan_diagnostic_for_extend(mlc::String type_name, mlc::String trait_name, ast::Span extend_span) noexcept{
  return ast::diagnostic_error_with_code((((((((mlc::String("orphan impl: extend ", 20) + type_name) + mlc::String(" : ", 3)) + registry::trait_base_name(trait_name)) + mlc::String(" must be declared in the module defining ", 41)) + type_name) + mlc::String(" or ", 4)) + registry::trait_base_name(trait_name)), extend_span, diagnostic_codes::diagnostic_code_e086());
}
mlc::Array<ast::Diagnostic> orphan_diagnostics_for_declaration(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [type_name, trait_name, __2, __3] = declExtend; return [&]() {
auto extend_span = ast::decl_span(declaration);
return (extend_is_orphan_impl(registry, type_name, trait_name, extend_span.file) ? (mlc::Array<ast::Diagnostic>{orphan_diagnostic_for_extend(type_name, trait_name, extend_span)}) : (empty_diagnostic_list()));
}();
}
return empty_diagnostic_list();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> orphan_impl_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept{
  auto diagnostics = mlc::Array<ast::Diagnostic>{};
  auto index = 0;
  while ((index < program.decls.length()))   {
    (diagnostics = ast::diagnostics_append(diagnostics, orphan_diagnostics_for_declaration(program.decls[index], registry)));
    (index = (index + 1));
  }
  return diagnostics;
}

} // namespace orphan_lint
