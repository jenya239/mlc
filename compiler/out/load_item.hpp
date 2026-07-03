#ifndef LOAD_ITEM_HPP
#define LOAD_ITEM_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
namespace load_item {

struct NamespaceImportAlias {
  mlc::String alias;
  mlc::String module_path;
};
struct LoadItem {
  mlc::String path;
  mlc::Array<std::shared_ptr<ast::Decl>> decls;
  mlc::Array<mlc::String> imports;
  mlc::Array<NamespaceImportAlias> namespace_import_aliases;
};

} // namespace load_item

#endif // LOAD_ITEM_HPP
