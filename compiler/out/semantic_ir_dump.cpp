#define main mlc_user_main
#include "semantic_ir_dump.hpp"

#include "semantic_ir.hpp"

namespace semantic_ir_dump {

using namespace semantic_ir;

mlc::String semantic_declaration_label(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [name, __1, __2, __3, __4, __5, __6, __7] = semanticDeclarationFn; return ((mlc::String("fn ", 3) + mlc::to_string(name)) + mlc::String("", 0)); },
[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [name, __1, __2, __3, __4] = semanticDeclarationType; return ((mlc::String("type ", 5) + mlc::to_string(name)) + mlc::String("", 0)); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [name, __1, __2, __3] = semanticDeclarationTypeAlias; return ((mlc::String("type ", 5) + mlc::to_string(name)) + mlc::String("", 0)); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [name, __1, __2, __3] = semanticDeclarationTrait; return ((mlc::String("trait ", 6) + mlc::to_string(name)) + mlc::String("", 0)); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, __1, __2, __3] = semanticDeclarationExtend; return ((mlc::String("extend ", 7) + mlc::to_string(type_name)) + mlc::String("", 0)); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [path, __1] = semanticDeclarationImport; return ((mlc::String("import ", 7) + mlc::to_string(path)) + mlc::String("", 0)); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner] = semanticDeclarationExported; return semantic_declaration_label(inner); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [name, __1, __2] = semanticDeclarationAssocBind; return ((mlc::String("assoc ", 6) + mlc::to_string(name)) + mlc::String("", 0)); }
}, (*declaration));
}
mlc::String print_semantic_load_item(semantic_ir::SemanticLoadItem item) noexcept{
  auto result = ((((mlc::String("sem module ", 11) + mlc::to_string(item.path)) + mlc::String(" decls=", 7)) + mlc::to_string(item.decls.length())) + mlc::String("", 0));
  auto index = 0;
  while (((index < item.decls.length()) && (index < 256)))   {
    (result = (result + ((mlc::String("\n  ", 3) + mlc::to_string(semantic_declaration_label(item.decls[index]))) + mlc::String("", 0))));
    (index = (index + 1));
  }
  if ((item.decls.length() > 256))   {
    (result = (result + ((mlc::String("\n  ... truncated (", 18) + mlc::to_string(item.decls.length())) + mlc::String(" total)", 7))));
  }
  return result;
}
mlc::String print_semantic_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
  auto result = mlc::String("", 0);
  auto index = 0;
  while ((index < items.length()))   {
    if ((index > 0))     {
      (result = (result + mlc::String("\n\n", 2)));
    }
    (result = (result + print_semantic_load_item(items[index])));
    (index = (index + 1));
  }
  return result;
}

} // namespace semantic_ir_dump
