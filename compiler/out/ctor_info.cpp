#define main mlc_user_main
#include "ctor_info.hpp"

#include "ast.hpp"
#include "param_analysis.hpp"

namespace ctor_info {

using namespace ast;
using namespace param_analysis;
using namespace ast_tokens;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> field_definition) noexcept{
  return field_definition->type_value;
}
std::shared_ptr<CtorTypeInfo> ctor_type_info_for(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_type_expressions) noexcept{
  auto scan_parsed = argument_type_expressions.fold(ConstructorArgumentScanState{0, {}, {}}, [=](ConstructorArgumentScanState accumulated_state, std::shared_ptr<ast::TypeExpr> type_expression_under_scan) mutable { return ConstructorArgumentScanState{(accumulated_state.next_position_index + 1), (param_analysis::is_shared_type(type_expression_under_scan) ? (accumulated_state.shared_positions.concat(mlc::Array<int>{accumulated_state.next_position_index})) : (accumulated_state.shared_positions)), (param_analysis::is_shared_array_type(type_expression_under_scan) ? (accumulated_state.shared_array_positions.concat(mlc::Array<int>{accumulated_state.next_position_index})) : (accumulated_state.shared_array_positions))}; });
  return std::make_shared<CtorTypeInfo>(CtorTypeInfo{constructor_name, scan_parsed.shared_positions, scan_parsed.shared_array_positions});
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> variant_to_ctor_info(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [variant_name, types, __2] = varTuple; return mlc::Array<std::shared_ptr<CtorTypeInfo>>{ctor_type_info_for(variant_name, types)}; },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_definitions, __2] = varRecord; return mlc::Array<std::shared_ptr<CtorTypeInfo>>{ctor_type_info_for(variant_name, field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_def_type(field_definition); }))}; },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto empty = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
return empty;
}(); }
}, (*variant));
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return variants.flat_map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_to_ctor_info(variant); });
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> declaration_to_ctor_infos(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<CtorTypeInfo>> {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner] = declExported; return [&]() -> mlc::Array<std::shared_ptr<CtorTypeInfo>> {
auto __match_subject = ast::decl_inner(inner);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [__0, __1, variants, __3, __4] = declType; return add_ctor_infos_from_variants(variants);
}
return [&]() {
auto empty = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
return empty;
}();
std::abort();
}();
}
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [__0, __1, variants, __3, __4] = declType; return add_ctor_infos_from_variants(variants);
}
return [&]() {
auto empty = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
return empty;
}();
std::abort();
}();
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
  return declarations.flat_map([=](std::shared_ptr<ast::Decl> declaration) mutable { return declaration_to_ctor_infos(declaration); });
}
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String constructor_lookup_name) noexcept{
  return infos.fold(std::make_shared<CtorTypeInfo>(CtorTypeInfo{mlc::String("", 0), {}, {}}), [=](std::shared_ptr<CtorTypeInfo> chosen, std::shared_ptr<CtorTypeInfo> candidate_info) mutable { return ((candidate_info->name == constructor_lookup_name) ? (candidate_info) : (chosen)); });
}
mlc::HashMap<mlc::String, std::shared_ptr<CtorTypeInfo>> build_ctor_type_info_index(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos) noexcept{
  auto index = mlc::HashMap<mlc::String, std::shared_ptr<CtorTypeInfo>>();
  auto info_index = 0;
  while ((info_index < infos.length()))   {
    index.set(infos[info_index]->name, infos[info_index]);
    (info_index = (info_index + 1));
  }
  return index;
}
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info_indexed(mlc::HashMap<mlc::String, std::shared_ptr<CtorTypeInfo>> index, mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String constructor_lookup_name) noexcept{
  if (index.has(constructor_lookup_name))   {
    return index.get(constructor_lookup_name);
  } else   {
    return lookup_ctor_type_info(infos, constructor_lookup_name);
  }
}

} // namespace ctor_info
