#include "program_to_semantic.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "load_item.hpp"
#include "registry.hpp"
#include "transform_decl.hpp"
#include "trait_param_expand.hpp"

namespace program_to_semantic {

using namespace ast;
using namespace semantic_ir;
using namespace load_item;
using namespace registry;
using namespace transform_decl;
using namespace trait_param_expand;
using namespace ast_tokens;

bool module_path_character_is_safe(mlc::String character) noexcept;

bool module_path_contains_parent_segment(mlc::String module_path) noexcept;

bool module_path_is_safe(mlc::String module_path) noexcept;

ast::Result<semantic_ir::SemanticLoadItem, mlc::Array<mlc::String>> program_to_semantic_load_item(ast::Program program, mlc::String module_path) noexcept;

bool module_path_character_is_safe(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character >= mlc::String("0") && character <= mlc::String("9") || character == mlc::String("_") || character == mlc::String("/") || character == mlc::String(".");}

bool module_path_contains_parent_segment(mlc::String module_path) noexcept{
int index = 0;
while (index + 1 < module_path.length()){
{
if (module_path.char_at(index) == mlc::String(".") && module_path.char_at(index + 1) == mlc::String(".")){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

bool module_path_is_safe(mlc::String module_path) noexcept{
return module_path == mlc::String("") || module_path.length() > 128 ? false : module_path_contains_parent_segment(module_path) ? false : [&]() -> bool { 
  int index = 0;
  while (index < module_path.length()){
{
if (!module_path_character_is_safe(module_path.char_at(index))){
{
return false;
}
}
index = index + 1;
}
}
  return true;
 }();
}

ast::Result<semantic_ir::SemanticLoadItem, mlc::Array<mlc::String>> program_to_semantic_load_item(ast::Program program, mlc::String module_path) noexcept{
if (!module_path_is_safe(module_path)){
{
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("program_to_semantic: unsafe module path")});
}
}
registry::TypeRegistry registry = registry::build_registry(program);
load_item::LoadItem load_item = load_item::LoadItem{module_path, program.decls, {}, {}};
trait_param_expand::TraitNominalMaps trait_maps = trait_param_expand::build_trait_nominal_maps(program);
mlc::Array<semantic_ir::SemanticLoadItem> semantic_items = transform_decl::transform_load_items(mlc::Array<load_item::LoadItem>{load_item}, registry, trait_maps);
return ast::Ok<semantic_ir::SemanticLoadItem>(semantic_items[0]);
}

} // namespace program_to_semantic
