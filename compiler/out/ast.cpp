#include "ast.hpp"

#include "ast_tokens.hpp"

namespace ast {

using namespace ast_tokens;

mlc::String param_name(std::shared_ptr<ast::Param> p) noexcept;

std::shared_ptr<ast::TypeExpr> param_typ(std::shared_ptr<ast::Param> p) noexcept;

std::shared_ptr<ast::Decl> decl_inner(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;

mlc::String param_name(std::shared_ptr<ast::Param> p) noexcept{return p->name;}

std::shared_ptr<ast::TypeExpr> param_typ(std::shared_ptr<ast::Param> p) noexcept{return p->typ;}

std::shared_ptr<ast::Decl> decl_inner(std::shared_ptr<ast::Decl> decl) noexcept{
std::shared_ptr<ast::Decl> unwrapped = decl;
return std::visit(overloaded{
  [&](const DeclExported& declexported) -> std::shared_ptr<ast::Decl> { auto [inner] = declexported; return inner; },
  [&](const DeclFn& declfn) -> std::shared_ptr<ast::Decl> { auto [_w0, _w1, _w2, _w3, _w4, _w5] = declfn; return decl; },
  [&](const DeclType& decltype_) -> std::shared_ptr<ast::Decl> { auto [_w0, _w1, _w2] = decltype_; return decl; },
  [&](const DeclTrait& decltrait) -> std::shared_ptr<ast::Decl> { auto [_w0, _w1, _w2] = decltrait; return decl; },
  [&](const DeclExtend& declextend) -> std::shared_ptr<ast::Decl> { auto [_w0, _w1, _w2] = declextend; return decl; },
  [&](const DeclImport& declimport) -> std::shared_ptr<ast::Decl> { auto [_w0, _w1] = declimport; return decl; }
}, (*unwrapped));
}

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept{
std::shared_ptr<ast::Decl> unwrapped = decl;
return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::String { auto [name, _w0, _w1, _w2, _w3, _w4] = declfn; return name; },
  [&](const DeclType& decltype_) -> mlc::String { auto [name, _w0, _w1] = decltype_; return name; },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [name, _w0, _w1] = decltrait; return name; },
  [&](const DeclExtend& declextend) -> mlc::String { auto [type_name, _w0, _w1] = declextend; return type_name; },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [inner] = declexported; return decl_name(inner); }
}, (*unwrapped));
}

mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
int i = 0;
while (i < src.size()){
{
dst.push_back(src[i]);
i = i + 1;
}
}
return dst;
}

} // namespace ast
