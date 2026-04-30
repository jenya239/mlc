#define main mlc_user_main
#include "ast.hpp"

namespace ast {

Span span_unknown() noexcept{
return Span{mlc::String("", 0), 0, 0};
}
std::shared_ptr<Expr> expr_placeholder() noexcept{
return std::make_shared<Expr>(ExprUnit{span_unknown()});
}
Diagnostic diagnostic_new(mlc::String severity, mlc::String message, Span span) noexcept{
return Diagnostic{message, span, severity};
}
Diagnostic diagnostic_error(mlc::String message, Span span) noexcept{
return diagnostic_new(mlc::String("error", 5), message, span);
}
mlc::String diagnostic_format(Diagnostic diagnostic) noexcept{
auto message_part = ((diagnostic.severity + mlc::String(": ", 2)) + diagnostic.message);
if (((diagnostic.span.line > 0) && (diagnostic.span.file.length() > 0))) {
return ((((((diagnostic.span.file + mlc::String(":", 1)) + mlc::to_string(diagnostic.span.line)) + mlc::String(":", 1)) + mlc::to_string(diagnostic.span.column)) + mlc::String(": ", 2)) + message_part);
} else if ((diagnostic.span.line > 0)) {
return ((((mlc::to_string(diagnostic.span.line) + mlc::String(":", 1)) + mlc::to_string(diagnostic.span.column)) + mlc::String(": ", 2)) + message_part);
} else {
return message_part;
}

}
mlc::Array<Diagnostic> diagnostics_append(mlc::Array<Diagnostic> destination, mlc::Array<Diagnostic> source) noexcept{
auto index = 0;
while ((index < source.length())) {
destination.push_back(source[index]);
index = (index + 1);
}
return destination;
}
mlc::Array<Diagnostic> infer_messages_as_diagnostics(mlc::Array<mlc::String> messages) noexcept{
auto collected = mlc::Array<Diagnostic>{};
auto index = 0;
while ((index < messages.length())) {
collected.push_back(diagnostic_error(messages[index], span_unknown()));
index = (index + 1);
}
return collected;
}
mlc::Array<mlc::String> diagnostics_to_strings(mlc::Array<Diagnostic> diagnostics) noexcept{
auto lines = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < diagnostics.length())) {
lines.push_back(diagnostic_format(diagnostics[index]));
index = (index + 1);
}
return lines;
}
Span expr_span(std::shared_ptr<Expr> expression) noexcept{
return std::visit(overloaded{[&](const ExprIdent& exprIdent) { auto [__0, source_span] = exprIdent; return source_span; },
[&](const ExprInt& exprInt) { auto [__0, source_span] = exprInt; return source_span; },
[&](const ExprStr& exprStr) { auto [__0, source_span] = exprStr; return source_span; },
[&](const ExprBool& exprBool) { auto [__0, source_span] = exprBool; return source_span; },
[&](const ExprUnit& exprUnit) { auto [source_span] = exprUnit; return source_span; },
[&](const ExprFloat& exprFloat) { auto [__0, source_span] = exprFloat; return source_span; },
[&](const ExprI64& exprI64) { auto [__0, source_span] = exprI64; return source_span; },
[&](const ExprU8& exprU8) { auto [__0, source_span] = exprU8; return source_span; },
[&](const ExprUsize& exprUsize) { auto [__0, source_span] = exprUsize; return source_span; },
[&](const ExprChar& exprChar) { auto [__0, source_span] = exprChar; return source_span; },
[&](const ExprBin& exprBin) { auto [__0, __1, __2, source_span] = exprBin; return source_span; },
[&](const ExprUn& exprUn) { auto [__0, __1, source_span] = exprUn; return source_span; },
[&](const ExprCall& exprCall) { auto [__0, __1, source_span] = exprCall; return source_span; },
[&](const ExprMethod& exprMethod) { auto [__0, __1, __2, source_span] = exprMethod; return source_span; },
[&](const ExprField& exprField) { auto [__0, __1, source_span] = exprField; return source_span; },
[&](const ExprIndex& exprIndex) { auto [__0, __1, source_span] = exprIndex; return source_span; },
[&](const ExprIf& exprIf) { auto [__0, __1, __2, source_span] = exprIf; return source_span; },
[&](const ExprBlock& exprBlock) { auto [__0, __1, source_span] = exprBlock; return source_span; },
[&](const ExprWhile& exprWhile) { auto [__0, __1, source_span] = exprWhile; return source_span; },
[&](const ExprFor& exprFor) { auto [__0, __1, __2, source_span] = exprFor; return source_span; },
[&](const ExprMatch& exprMatch) { auto [__0, __1, source_span] = exprMatch; return source_span; },
[&](const ExprRecord& exprRecord) { auto [__0, __1, source_span] = exprRecord; return source_span; },
[&](const ExprRecordUpdate& exprRecordUpdate) { auto [__0, __1, __2, source_span] = exprRecordUpdate; return source_span; },
[&](const ExprArray& exprArray) { auto [__0, source_span] = exprArray; return source_span; },
[&](const ExprTuple& exprTuple) { auto [__0, source_span] = exprTuple; return source_span; },
[&](const ExprQuestion& exprQuestion) { auto [__0, source_span] = exprQuestion; return source_span; },
[&](const ExprExtern& exprExtern) { auto [source_span] = exprExtern; return source_span; },
[&](const ExprLambda& exprLambda) { auto [__0, __1, source_span] = exprLambda; return source_span; },
[&](const ExprNamedArg& exprNamedArg) { auto [__0, __1, source_span] = exprNamedArg; return source_span; },
[&](const ExprWith& exprWith) { auto [__0, __1, __2, source_span] = exprWith; return source_span; },
[&](const auto& __v) { return span_unknown(); }
}, (*expression));
}
Span stmt_span(std::shared_ptr<Stmt> statement) noexcept{
return std::visit(overloaded{[&](const StmtLet& stmtLet) { auto [__0, __1, __2, __3, source_span] = stmtLet; return source_span; },
[&](const StmtLetPat& stmtLetPat) { auto [__0, __1, __2, __3, __4, __5, source_span] = stmtLetPat; return source_span; },
[&](const StmtLetConst& stmtLetConst) { auto [__0, __1, __2, source_span] = stmtLetConst; return source_span; },
[&](const StmtExpr& stmtExpr) { auto [__0, source_span] = stmtExpr; return source_span; },
[&](const StmtBreak& stmtBreak) { auto [source_span] = stmtBreak; return source_span; },
[&](const StmtContinue& stmtContinue) { auto [source_span] = stmtContinue; return source_span; },
[&](const StmtReturn& stmtReturn) { auto [__0, source_span] = stmtReturn; return source_span; }
}, (*statement));
}
Span pat_span(std::shared_ptr<Pat> pattern) noexcept{
return std::visit(overloaded{[&](const PatWild& patWild) { auto [source_span] = patWild; return source_span; },
[&](const PatIdent& patIdent) { auto [__0, source_span] = patIdent; return source_span; },
[&](const PatInt& patInt) { auto [__0, source_span] = patInt; return source_span; },
[&](const PatStr& patStr) { auto [__0, source_span] = patStr; return source_span; },
[&](const PatBool& patBool) { auto [__0, source_span] = patBool; return source_span; },
[&](const PatUnit& patUnit) { auto [source_span] = patUnit; return source_span; },
[&](const PatCtor& patCtor) { auto [__0, __1, source_span] = patCtor; return source_span; },
[&](const PatRecord& patRecord) { auto [__0, __1, source_span] = patRecord; return source_span; },
[&](const PatTuple& patTuple) { auto [__0, source_span] = patTuple; return source_span; },
[&](const PatArray& patArray) { auto [__0, __1, source_span] = patArray; return source_span; },
[&](const PatOr& patOr) { auto [__0, source_span] = patOr; return source_span; }
}, (*pattern));
}
mlc::String param_name(std::shared_ptr<Param> p) noexcept{
return p->name;
}
std::shared_ptr<TypeExpr> param_typ(std::shared_ptr<Param> p) noexcept{
return p->typ;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
auto unwrapped = decl;
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [inner] = declExported; return inner; },
[&](const DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5] = declFn; return decl; },
[&](const DeclType& declType) { auto [__0, __1, __2, __3] = declType; return decl; },
[&](const DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return decl; },
[&](const DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return decl; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; return decl; },
[&](const DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return decl; },
[&](const DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return decl; }
}, (*unwrapped));
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
auto unwrapped = decl;
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5] = declFn; return name; },
[&](const DeclType& declType) { auto [name, __1, __2, __3] = declType; return name; },
[&](const DeclTrait& declTrait) { auto [name, __1, __2] = declTrait; return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1, __2] = declExtend; return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const DeclExported& declExported) { auto [inner] = declExported; return decl_name(inner); },
[&](const DeclAssocType& declAssocType) { auto [name, __1] = declAssocType; return name; },
[&](const DeclAssocBind& declAssocBind) { auto [name, __1, __2] = declAssocBind; return name; }
}, (*unwrapped));
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
