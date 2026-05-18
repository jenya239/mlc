#define main mlc_user_main
#include "ctor_info.hpp"

namespace ctor_info {

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> field_definition) noexcept{
return field_definition->typ;
}
std::shared_ptr<CtorTypeInfo> ctor_type_info_for(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_type_expressions) noexcept{
auto scan_result = argument_type_expressions.fold(ConstructorArgumentScanState{0, {}, {}}, [=](ConstructorArgumentScanState accumulated_state, std::shared_ptr<ast::TypeExpr> type_expression_under_scan) mutable { return ConstructorArgumentScanState{(accumulated_state.next_position_index + 1), (param_analysis::is_shared_type(type_expression_under_scan) ? (accumulated_state.shared_positions.concat(mlc::Array<int>{accumulated_state.next_position_index})) : (accumulated_state.shared_positions)), (param_analysis::is_shared_array_type(type_expression_under_scan) ? (accumulated_state.shared_array_positions.concat(mlc::Array<int>{accumulated_state.next_position_index})) : (accumulated_state.shared_array_positions))}; });
return std::make_shared<CtorTypeInfo>(CtorTypeInfo{constructor_name, scan_result.shared_positions, scan_result.shared_array_positions});
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> variant_to_ctor_info(std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [variant_name, types, __2] = varTuple; return mlc::Array<std::shared_ptr<CtorTypeInfo>>{ctor_type_info_for(variant_name, types)}; },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs, __2] = varRecord; return mlc::Array<std::shared_ptr<CtorTypeInfo>>{ctor_type_info_for(variant_name, field_defs.map([=](std::shared_ptr<ast::FieldDef> fd) mutable { return field_def_type(fd); }))}; },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto empty = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
return empty;
}(); }
}, (*variant));
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
return variants.flat_map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_to_ctor_info(variant); });
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> decl_to_ctor_infos(std::shared_ptr<ast::Decl> decl) noexcept{
return std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1, variants, __3] = declType; return add_ctor_infos_from_variants(variants); },
[&](const auto& __v) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
return empty;
}(); }
}, (*ast::decl_inner(inner))); },
[&](const ast::DeclType& declType) { auto [__0, __1, variants, __3] = declType; return add_ctor_infos_from_variants(variants); },
[&](const auto& __v) { return [&]() {
auto empty = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
return empty;
}(); }
}, (*decl));
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
return decls.flat_map([=](std::shared_ptr<ast::Decl> decl) mutable { return decl_to_ctor_infos(decl); });
}
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String constructor_lookup_name) noexcept{
return infos.fold(std::make_shared<CtorTypeInfo>(CtorTypeInfo{mlc::String("", 0), {}, {}}), [=](auto chosen, std::shared_ptr<CtorTypeInfo> candidate_info) mutable { return ((candidate_info->name == constructor_lookup_name) ? (candidate_info) : (chosen)); });
}

} // namespace ctor_info
