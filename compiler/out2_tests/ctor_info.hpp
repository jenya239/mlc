#ifndef CTOR_INFO_HPP
#define CTOR_INFO_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "param_analysis.hpp"

namespace ctor_info {

struct CtorTypeInfo {mlc::String name;mlc::Array<int> shared_pos;mlc::Array<int> shared_arr_pos;};
std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept;
std::shared_ptr<CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String cti_name) noexcept;

} // namespace ctor_info

#endif // CTOR_INFO_HPP
