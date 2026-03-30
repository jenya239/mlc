#include "ast.hpp"

#include "ast_tokens.hpp"

namespace ast {

using namespace ast_tokens;

ast::Span span_unknown() noexcept;

ast::Diagnostic diagnostic_new(mlc::String severity, mlc::String message, ast::Span span) noexcept;

ast::Diagnostic diagnostic_error(mlc::String message, ast::Span span) noexcept;

mlc::String diagnostic_format(ast::Diagnostic diagnostic) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_append(mlc::Array<ast::Diagnostic> destination, mlc::Array<ast::Diagnostic> source) noexcept;

mlc::Array<ast::Diagnostic> infer_messages_as_diagnostics(mlc::Array<mlc::String> messages) noexcept;

mlc::Array<mlc::String> diagnostics_to_strings(mlc::Array<ast::Diagnostic> diagnostics) noexcept;

ast::Span expr_span(std::shared_ptr<ast::Expr> expression) noexcept;

ast::Span stmt_span(std::shared_ptr<ast::Stmt> statement) noexcept;

ast::Span pat_span(std::shared_ptr<ast::Pat> pattern) noexcept;

mlc::String param_name(std::shared_ptr<ast::Param> p) noexcept;

std::shared_ptr<ast::TypeExpr> param_typ(std::shared_ptr<ast::Param> p) noexcept;

std::shared_ptr<ast::Decl> decl_inner(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;

ast::Span span_unknown() noexcept{return ast::Span{mlc::String(""), 0, 0};}

ast::Diagnostic diagnostic_new(mlc::String severity, mlc::String message, ast::Span span) noexcept{return ast::Diagnostic{message, span, severity};}

ast::Diagnostic diagnostic_error(mlc::String message, ast::Span span) noexcept{return diagnostic_new(mlc::String("error"), message, span);}

mlc::String diagnostic_format(ast::Diagnostic diagnostic) noexcept{
mlc::String where_part = diagnostic.span.file.length() > 0 ? diagnostic.span.file + mlc::String(":") + mlc::to_string(diagnostic.span.line) + mlc::String(":") + mlc::to_string(diagnostic.span.column) : mlc::to_string(diagnostic.span.line) + mlc::String(":") + mlc::to_string(diagnostic.span.column);
return diagnostic.severity + mlc::String(": ") + diagnostic.message + mlc::String(" at ") + where_part;
}

mlc::Array<ast::Diagnostic> diagnostics_append(mlc::Array<ast::Diagnostic> destination, mlc::Array<ast::Diagnostic> source) noexcept{
int index = 0;
while (index < source.size()){
{
destination.push_back(source[index]);
index = index + 1;
}
}
return destination;
}

mlc::Array<ast::Diagnostic> infer_messages_as_diagnostics(mlc::Array<mlc::String> messages) noexcept{
mlc::Array<ast::Diagnostic> collected = {};
int index = 0;
while (index < messages.size()){
{
collected.push_back(diagnostic_error(messages[index], span_unknown()));
index = index + 1;
}
}
return collected;
}

mlc::Array<mlc::String> diagnostics_to_strings(mlc::Array<ast::Diagnostic> diagnostics) noexcept{
mlc::Array<mlc::String> lines = {};
int index = 0;
while (index < diagnostics.size()){
{
lines.push_back(diagnostic_format(diagnostics[index]));
index = index + 1;
}
}
return lines;
}

ast::Span expr_span(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() -> ast::Span { if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [_w0, source_span] = _v_exprident; return source_span; } if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [_w0, source_span] = _v_exprint; return source_span; } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [_w0, source_span] = _v_exprstr; return source_span; } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [_w0, source_span] = _v_exprbool; return source_span; } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [source_span] = _v_exprunit; return source_span; } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [_w0, _w1, _w2, source_span] = _v_exprbin; return source_span; } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, _w1, source_span] = _v_exprun; return source_span; } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [_w0, _w1, source_span] = _v_exprcall; return source_span; } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [_w0, _w1, _w2, source_span] = _v_exprmethod; return source_span; } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [_w0, _w1, source_span] = _v_exprfield; return source_span; } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [_w0, _w1, source_span] = _v_exprindex; return source_span; } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [_w0, _w1, _w2, source_span] = _v_exprif; return source_span; } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [_w0, _w1, source_span] = _v_exprblock; return source_span; } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [_w0, _w1, source_span] = _v_exprwhile; return source_span; } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [_w0, _w1, _w2, source_span] = _v_exprfor; return source_span; } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [_w0, _w1, source_span] = _v_exprmatch; return source_span; } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, _w1, source_span] = _v_exprrecord; return source_span; } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, _w1, _w2, source_span] = _v_exprrecordupdate; return source_span; } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [_w0, source_span] = _v_exprarray; return source_span; } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [_w0, source_span] = _v_exprquestion; return source_span; } if (std::holds_alternative<ast::ExprExtern>((*expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*expression)._); auto [source_span] = _v_exprextern; return source_span; } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [_w0, _w1, source_span] = _v_exprlambda; return source_span; } return span_unknown(); }();}

ast::Span stmt_span(std::shared_ptr<ast::Stmt> statement) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> ast::Span { auto [_w0, _w1, _w2, _w3, source_span] = stmtlet; return source_span; },
  [&](const StmtLetConst& stmtletconst) -> ast::Span { auto [_w0, _w1, _w2, source_span] = stmtletconst; return source_span; },
  [&](const StmtExpr& stmtexpr) -> ast::Span { auto [_w0, source_span] = stmtexpr; return source_span; },
  [&](const StmtBreak& stmtbreak) -> ast::Span { auto [source_span] = stmtbreak; return source_span; },
  [&](const StmtContinue& stmtcontinue) -> ast::Span { auto [source_span] = stmtcontinue; return source_span; },
  [&](const StmtReturn& stmtreturn) -> ast::Span { auto [_w0, source_span] = stmtreturn; return source_span; }
}, (*statement)._);}

ast::Span pat_span(std::shared_ptr<ast::Pat> pattern) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) -> ast::Span { auto [source_span] = patwild; return source_span; },
  [&](const PatIdent& patident) -> ast::Span { auto [_w0, source_span] = patident; return source_span; },
  [&](const PatInt& patint) -> ast::Span { auto [_w0, source_span] = patint; return source_span; },
  [&](const PatStr& patstr) -> ast::Span { auto [_w0, source_span] = patstr; return source_span; },
  [&](const PatBool& patbool) -> ast::Span { auto [_w0, source_span] = patbool; return source_span; },
  [&](const PatUnit& patunit) -> ast::Span { auto [source_span] = patunit; return source_span; },
  [&](const PatCtor& patctor) -> ast::Span { auto [_w0, _w1, source_span] = patctor; return source_span; },
  [&](const PatRecord& patrecord) -> ast::Span { auto [_w0, _w1, source_span] = patrecord; return source_span; }
}, (*pattern));}

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
