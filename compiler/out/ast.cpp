#define main mlc_user_main
#include "ast.hpp"

namespace ast {

mlc::String param_name(std::shared_ptr<Param> p) noexcept{
return p->name;
}
std::shared_ptr<TypeExpr> param_typ(std::shared_ptr<Param> p) noexcept{
return p->typ;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
auto d = decl;
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [inner] = declExported; return inner; },
[&](const DeclFn& declFn) { auto [__0, __1, __2, __3] = declFn; return decl; },
[&](const DeclType& declType) { auto [__0, __1] = declType; return decl; },
[&](const DeclExtend& declExtend) { auto [__0, __1] = declExtend; return decl; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; return decl; }
}, (*d));
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
auto d = decl;
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const DeclExported& declExported) { auto [inner] = declExported; return decl_name(inner); }
}, (*d));
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}

} // namespace ast
