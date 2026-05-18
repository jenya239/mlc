#ifndef CTOR_INFO_HPP
#define CTOR_INFO_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "param_analysis.hpp"

namespace ctor_info {

struct CtorTypeInfo {mlc::String name;mlc::Array<int> shared_pos;mlc::Array<int> shared_arr_pos;};
struct ConstructorArgumentScanState {int next_position_index;mlc::Array<int> shared_positions;mlc::Array<int> shared_array_positions;};
std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> field_definition) noexcept;
std::shared_ptr<CtorTypeInfo> ctor_type_info_for(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> argument_type_expressions) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> variant_to_ctor_info(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> decl_to_ctor_infos(std::shared_ptr<ast::Decl> decl) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String constructor_lookup_name) noexcept;

} // namespace ctor_info

#endif // CTOR_INFO_HPP
