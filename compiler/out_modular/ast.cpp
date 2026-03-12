#include "ast.hpp"

#include "ast_tokens.hpp"

namespace ast {

using namespace ast_tokens;

std::shared_ptr<ast::Decl> decl_inner(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;

std::shared_ptr<ast::Decl> decl_inner(std::shared_ptr<ast::Decl> decl) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclExported>((*decl))) { auto _v_declexported = std::get<ast::DeclExported>((*decl)); auto [d] = _v_declexported; return d; } return decl; }();}

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept{return std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [name, _w0, _w1, _w2] = declfn; return name; },
  [&](const DeclType& decltype_) { auto [name, _w0] = decltype_; return name; },
  [&](const DeclExtend& declextend) { auto [type_name, _w0] = declextend; return type_name; },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return ast::decl_name(d); }
}, (*decl));}

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
