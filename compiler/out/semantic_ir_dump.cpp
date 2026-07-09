#include "semantic_ir_dump.hpp"

#include "semantic_ir.hpp"

namespace semantic_ir_dump {

using namespace semantic_ir;

mlc::String semantic_declaration_label(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::String print_semantic_load_item(semantic_ir::SemanticLoadItem item) noexcept;

mlc::String print_semantic_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

mlc::String semantic_declaration_label(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const SemanticDeclarationFn& semanticdeclarationfn) -> mlc::String { auto [name, _w0, _w1, _w2, _w3, _w4, _w5, _w6] = semanticdeclarationfn; return mlc::String("fn ") + name; },
  [&](const SemanticDeclarationType& semanticdeclarationtype) -> mlc::String { auto [name, _w0, _w1, _w2, _w3] = semanticdeclarationtype; return mlc::String("type ") + name; },
  [&](const SemanticDeclarationTypeAlias& semanticdeclarationtypealias) -> mlc::String { auto [name, _w0, _w1, _w2] = semanticdeclarationtypealias; return mlc::String("type ") + name; },
  [&](const SemanticDeclarationTrait& semanticdeclarationtrait) -> mlc::String { auto [name, _w0, _w1, _w2] = semanticdeclarationtrait; return mlc::String("trait ") + name; },
  [&](const SemanticDeclarationExtend& semanticdeclarationextend) -> mlc::String { auto [type_name, _w0, _w1, _w2] = semanticdeclarationextend; return mlc::String("extend ") + type_name; },
  [&](const SemanticDeclarationImport& semanticdeclarationimport) -> mlc::String { auto [path, _w0] = semanticdeclarationimport; return mlc::String("import ") + path; },
  [&](const SemanticDeclarationExported& semanticdeclarationexported) -> mlc::String { auto [inner] = semanticdeclarationexported; return semantic_declaration_label(inner); },
  [&](const SemanticDeclarationAssocBind& semanticdeclarationassocbind) -> mlc::String { auto [name, _w0, _w1] = semanticdeclarationassocbind; return mlc::String("assoc ") + name; }
}, (*declaration));}

mlc::String print_semantic_load_item(semantic_ir::SemanticLoadItem item) noexcept{
mlc::String result = mlc::String("sem module ") + item.path + mlc::String(" decls=") + mlc::to_string(item.decls.size());
int index = 0;
while (index < item.decls.size() && index < 256){
{
result = result + mlc::String("\n  ") + semantic_declaration_label(item.decls[index]);
index = index + 1;
}
}
if (item.decls.size() > 256){
{
result = result + mlc::String("\n  ... truncated (") + mlc::to_string(item.decls.size()) + mlc::String(" total)");
}
}
return result;
}

mlc::String print_semantic_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept{
mlc::String result = mlc::String("");
int index = 0;
while (index < items.size()){
{
if (index > 0){
{
result = result + mlc::String("\n\n");
}
}
result = result + print_semantic_load_item(items[index]);
index = index + 1;
}
}
return result;
}

} // namespace semantic_ir_dump
