#ifndef CTOR_INFO_HPP
#define CTOR_INFO_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "param_analysis.hpp"

namespace ctor_info {

struct Expr;
struct Stmt;

struct CtorTypeInfo {mlc::String name;mlc::Array<int> shared_pos;mlc::Array<int> shared_arr_pos;};

mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

std::shared_ptr<ctor_info::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> infos, mlc::String cti_name) noexcept;

} // namespace ctor_info

#endif // CTOR_INFO_HPP
