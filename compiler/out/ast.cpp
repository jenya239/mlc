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
std::shared_ptr<ast::Decl> d = decl;
return std::visit(overloaded{
  [&](const DeclExported& declexported) { auto [inner] = declexported; return inner; },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return decl; },
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return decl; },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return decl; },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return decl; }
}, (*d));
}

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept{
std::shared_ptr<ast::Decl> d = decl;
return std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [name, _w0, _w1, _w2] = declfn; return name; },
  [&](const DeclType& decltype_) { auto [name, _w0] = decltype_; return name; },
  [&](const DeclExtend& declextend) { auto [type_name, _w0] = declextend; return type_name; },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [inner] = declexported; return decl_name(inner); }
}, (*d));
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
