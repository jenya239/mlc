#include "ctor_info.hpp"

#include "ast.hpp"
#include "param_analysis.hpp"

namespace ctor_info {

using namespace ast;
using namespace param_analysis;
using namespace ast_tokens;

struct ConstructorArgumentScanState {int next_position_index;mlc::Array<int> shared_positions;mlc::Array<int> shared_array_positions;};

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> field_definition) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_info_for(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_type_expressions) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> variant_to_ctor_info(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> decl_to_ctor_infos(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> infos, mlc::String constructor_lookup_name) noexcept;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> field_definition) noexcept{return field_definition->typ;}

std::shared_ptr<ctor_info::CtorTypeInfo> ctor_type_info_for(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_type_expressions) noexcept{
ctor_info::ConstructorArgumentScanState scan_result = argument_type_expressions.fold(ctor_info::ConstructorArgumentScanState{0, {}, {}}, [](ctor_info::ConstructorArgumentScanState accumulated_state, std::shared_ptr<ast::TypeExpr> type_expression_under_scan) mutable { return ctor_info::ConstructorArgumentScanState{accumulated_state.next_position_index + 1, param_analysis::is_shared_type(type_expression_under_scan) ? accumulated_state.shared_positions.concat(mlc::Array<int>{accumulated_state.next_position_index}) : accumulated_state.shared_positions, param_analysis::is_shared_array_type(type_expression_under_scan) ? accumulated_state.shared_array_positions.concat(mlc::Array<int>{accumulated_state.next_position_index}) : accumulated_state.shared_array_positions}; });
return std::make_shared<ctor_info::CtorTypeInfo>(ctor_info::CtorTypeInfo{constructor_name, scan_result.shared_positions, scan_result.shared_array_positions});
}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> variant_to_ctor_info(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { auto [variant_name, types, _w0] = vartuple; return mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>>{ctor_type_info_for(variant_name, types)}; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { auto [variant_name, field_defs, _w0] = varrecord; return mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>>{ctor_type_info_for(variant_name, field_defs.map([](std::shared_ptr<ast::FieldDef> fd) mutable { return field_def_type(fd); }))}; },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { auto [_w0, _w1] = varunit; return {}; }
}, (*variant));}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return mlc::collections::flat_map(variants, [](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_to_ctor_info(variant); });}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> decl_to_ctor_infos(std::shared_ptr<ast::Decl> decl) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { if (std::holds_alternative<ast::DeclExported>((*decl))) { auto _v_declexported = std::get<ast::DeclExported>((*decl)); auto [inner] = _v_declexported; return [&]() -> mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(inner)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(inner))); auto [_w0, _w1, variants, _w2] = _v_decltype; return add_ctor_infos_from_variants(variants); } return {}; }(); } if (std::holds_alternative<ast::DeclType>((*decl))) { auto _v_decltype = std::get<ast::DeclType>((*decl)); auto [_w0, _w1, variants, _w2] = _v_decltype; return add_ctor_infos_from_variants(variants); } return {}; }();}

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{return mlc::collections::flat_map(decls, [](std::shared_ptr<ast::Decl> decl) mutable { return decl_to_ctor_infos(decl); });}

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> infos, mlc::String constructor_lookup_name) noexcept{return infos.fold(std::make_shared<ctor_info::CtorTypeInfo>(ctor_info::CtorTypeInfo{mlc::String(""), {}, {}}), [constructor_lookup_name](std::shared_ptr<ctor_info::CtorTypeInfo> chosen, std::shared_ptr<ctor_info::CtorTypeInfo> candidate_info) mutable { return candidate_info->name == constructor_lookup_name ? candidate_info : chosen; });}

} // namespace ctor_info
