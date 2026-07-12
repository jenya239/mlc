#define main mlc_user_main
#include "ast.hpp"

#include "ast_tokens.hpp"

namespace ast {

using namespace ast_tokens;

Span span_unknown() noexcept{
  return Span{mlc::String("", 0), 0, 0, (-1), (-1)};
}
Span span_make(mlc::String file, int line, int column) noexcept{
  return Span{file, line, column, (-1), (-1)};
}
std::shared_ptr<Expr> expr_placeholder() noexcept{
  return std::make_shared<Expr>(ExprUnit{span_unknown()});
}
Diagnostic diagnostic_new(mlc::String severity, mlc::String message, Span span) noexcept{
  return diagnostic_new_with_code(severity, message, span, mlc::String("", 0));
}
Diagnostic diagnostic_new_with_code(mlc::String severity, mlc::String message, Span span, mlc::String code) noexcept{
  return Diagnostic{message, span, severity, code};
}
Diagnostic diagnostic_error(mlc::String message, Span span) noexcept{
  return diagnostic_new(mlc::String("error", 5), message, span);
}
Diagnostic diagnostic_error_with_code(mlc::String message, Span span, mlc::String code) noexcept{
  return diagnostic_new_with_code(mlc::String("error", 5), message, span, code);
}
mlc::String diagnostic_format(Diagnostic diagnostic) noexcept{
  auto message_part = ((diagnostic.code.length() > 0) ? (((((diagnostic.severity + mlc::String("[", 1)) + diagnostic.code) + mlc::String("]: ", 3)) + diagnostic.message)) : (((diagnostic.severity + mlc::String(": ", 2)) + diagnostic.message)));
  if ((diagnostic.span.line > 0))   {
    auto location_part = ((diagnostic.span.file.length() > 0) ? (((((diagnostic.span.file + mlc::String(":", 1)) + mlc::to_string(diagnostic.span.line)) + mlc::String(":", 1)) + mlc::to_string(diagnostic.span.column))) : (((mlc::to_string(diagnostic.span.line) + mlc::String(":", 1)) + mlc::to_string(diagnostic.span.column))));
    return ((message_part + mlc::String("\n  --> ", 7)) + location_part);
  } else   {
    return message_part;
  }
}
mlc::Array<Diagnostic> diagnostics_append(mlc::Array<Diagnostic> destination, mlc::Array<Diagnostic> source) noexcept{
  auto accumulator = destination;
  auto index = 0;
  while ((index < source.length()))   {
    accumulator.push_back(source[index]);
    (index = mlc::arith::checked_add(index, 1));
  }
  return accumulator;
}
mlc::Array<mlc::String> diagnostics_to_strings(mlc::Array<Diagnostic> diagnostics) noexcept{
  auto lines = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < diagnostics.length()))   {
    lines.push_back(diagnostic_format(diagnostics[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return lines;
}
Span expr_span(std::shared_ptr<Expr> expression) noexcept{
  return [&]() -> Span {
auto __match_subject = expression;
if (std::holds_alternative<ExprIdent>((*__match_subject))) {
const ExprIdent& exprIdent = std::get<ExprIdent>((*__match_subject));
auto [__0, source_span] = exprIdent; return source_span;
}
if (std::holds_alternative<ExprInt>((*__match_subject))) {
const ExprInt& exprInt = std::get<ExprInt>((*__match_subject));
auto [__0, source_span] = exprInt; return source_span;
}
if (std::holds_alternative<ExprStr>((*__match_subject))) {
const ExprStr& exprStr = std::get<ExprStr>((*__match_subject));
auto [__0, source_span] = exprStr; return source_span;
}
if (std::holds_alternative<ExprBool>((*__match_subject))) {
const ExprBool& exprBool = std::get<ExprBool>((*__match_subject));
auto [__0, source_span] = exprBool; return source_span;
}
if (std::holds_alternative<ExprUnit>((*__match_subject))) {
const ExprUnit& exprUnit = std::get<ExprUnit>((*__match_subject));
auto [source_span] = exprUnit; return source_span;
}
if (std::holds_alternative<ExprFloat>((*__match_subject))) {
const ExprFloat& exprFloat = std::get<ExprFloat>((*__match_subject));
auto [__0, source_span] = exprFloat; return source_span;
}
if (std::holds_alternative<ExprI64>((*__match_subject))) {
const ExprI64& exprI64 = std::get<ExprI64>((*__match_subject));
auto [__0, source_span] = exprI64; return source_span;
}
if (std::holds_alternative<ExprU8>((*__match_subject))) {
const ExprU8& exprU8 = std::get<ExprU8>((*__match_subject));
auto [__0, source_span] = exprU8; return source_span;
}
if (std::holds_alternative<ExprUsize>((*__match_subject))) {
const ExprUsize& exprUsize = std::get<ExprUsize>((*__match_subject));
auto [__0, source_span] = exprUsize; return source_span;
}
if (std::holds_alternative<ExprChar>((*__match_subject))) {
const ExprChar& exprChar = std::get<ExprChar>((*__match_subject));
auto [__0, source_span] = exprChar; return source_span;
}
if (std::holds_alternative<ExprBin>((*__match_subject))) {
const ExprBin& exprBin = std::get<ExprBin>((*__match_subject));
auto [__0, __1, __2, source_span] = exprBin; return source_span;
}
if (std::holds_alternative<ExprUn>((*__match_subject))) {
const ExprUn& exprUn = std::get<ExprUn>((*__match_subject));
auto [__0, __1, source_span] = exprUn; return source_span;
}
if (std::holds_alternative<ExprCall>((*__match_subject))) {
const ExprCall& exprCall = std::get<ExprCall>((*__match_subject));
auto [__0, __1, source_span] = exprCall; return source_span;
}
if (std::holds_alternative<ExprMethod>((*__match_subject))) {
const ExprMethod& exprMethod = std::get<ExprMethod>((*__match_subject));
auto [__0, __1, __2, source_span] = exprMethod; return source_span;
}
if (std::holds_alternative<ExprField>((*__match_subject))) {
const ExprField& exprField = std::get<ExprField>((*__match_subject));
auto [__0, __1, source_span] = exprField; return source_span;
}
if (std::holds_alternative<ExprIndex>((*__match_subject))) {
const ExprIndex& exprIndex = std::get<ExprIndex>((*__match_subject));
auto [__0, __1, source_span] = exprIndex; return source_span;
}
if (std::holds_alternative<ExprIf>((*__match_subject))) {
const ExprIf& exprIf = std::get<ExprIf>((*__match_subject));
auto [__0, __1, __2, source_span] = exprIf; return source_span;
}
if (std::holds_alternative<ExprBlock>((*__match_subject))) {
const ExprBlock& exprBlock = std::get<ExprBlock>((*__match_subject));
auto [__0, __1, source_span] = exprBlock; return source_span;
}
if (std::holds_alternative<ExprWhile>((*__match_subject))) {
const ExprWhile& exprWhile = std::get<ExprWhile>((*__match_subject));
auto [__0, __1, source_span] = exprWhile; return source_span;
}
if (std::holds_alternative<ExprFor>((*__match_subject))) {
const ExprFor& exprFor = std::get<ExprFor>((*__match_subject));
auto [__0, __1, __2, source_span] = exprFor; return source_span;
}
if (std::holds_alternative<ExprMatch>((*__match_subject))) {
const ExprMatch& exprMatch = std::get<ExprMatch>((*__match_subject));
auto [__0, __1, source_span] = exprMatch; return source_span;
}
if (std::holds_alternative<ExprRecord>((*__match_subject))) {
const ExprRecord& exprRecord = std::get<ExprRecord>((*__match_subject));
auto [__0, __1, source_span] = exprRecord; return source_span;
}
if (std::holds_alternative<ExprRecordUpdate>((*__match_subject))) {
const ExprRecordUpdate& exprRecordUpdate = std::get<ExprRecordUpdate>((*__match_subject));
auto [__0, __1, __2, source_span] = exprRecordUpdate; return source_span;
}
if (std::holds_alternative<ExprArray>((*__match_subject))) {
const ExprArray& exprArray = std::get<ExprArray>((*__match_subject));
auto [__0, source_span] = exprArray; return source_span;
}
if (std::holds_alternative<ExprTuple>((*__match_subject))) {
const ExprTuple& exprTuple = std::get<ExprTuple>((*__match_subject));
auto [__0, source_span] = exprTuple; return source_span;
}
if (std::holds_alternative<ExprQuestion>((*__match_subject))) {
const ExprQuestion& exprQuestion = std::get<ExprQuestion>((*__match_subject));
auto [__0, source_span] = exprQuestion; return source_span;
}
if (std::holds_alternative<ExprExtern>((*__match_subject))) {
const ExprExtern& exprExtern = std::get<ExprExtern>((*__match_subject));
auto [__0, __1, __2, source_span] = exprExtern; return source_span;
}
if (std::holds_alternative<ExprLambda>((*__match_subject))) {
const ExprLambda& exprLambda = std::get<ExprLambda>((*__match_subject));
auto [__0, __1, source_span] = exprLambda; return source_span;
}
if (std::holds_alternative<ExprSpawn>((*__match_subject))) {
const ExprSpawn& exprSpawn = std::get<ExprSpawn>((*__match_subject));
auto [__0, source_span] = exprSpawn; return source_span;
}
if (std::holds_alternative<ExprNamedArg>((*__match_subject))) {
const ExprNamedArg& exprNamedArg = std::get<ExprNamedArg>((*__match_subject));
auto [__0, __1, source_span] = exprNamedArg; return source_span;
}
if (std::holds_alternative<ExprWith>((*__match_subject))) {
const ExprWith& exprWith = std::get<ExprWith>((*__match_subject));
auto [__0, __1, __2, source_span] = exprWith; return source_span;
}
if (std::holds_alternative<ExprScope>((*__match_subject))) {
const ExprScope& exprScope = std::get<ExprScope>((*__match_subject));
auto [__0, __1, source_span] = exprScope; return source_span;
}
if (std::holds_alternative<ExprRegion>((*__match_subject))) {
const ExprRegion& exprRegion = std::get<ExprRegion>((*__match_subject));
auto [__0, __1, source_span] = exprRegion; return source_span;
}
return span_unknown();
std::abort();
}();
}
std::shared_ptr<Expr> expr_spawn_body_result(mlc::Array<std::shared_ptr<Stmt>> statements) noexcept{
  if ((statements.length() == 0))   {
    return std::make_shared<Expr>(ExprUnit{span_unknown()});
  } else   {
    return [&]() -> std::shared_ptr<Expr> {
auto __match_subject = statements[mlc::arith::checked_sub(statements.length(), 1)];
if (std::holds_alternative<StmtReturn>((*__match_subject))) {
const StmtReturn& stmtReturn = std::get<StmtReturn>((*__match_subject));
auto [expression, __1] = stmtReturn; return expression;
}
if (std::holds_alternative<StmtExpr>((*__match_subject))) {
const StmtExpr& stmtExpr = std::get<StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; return expression;
}
return std::make_shared<Expr>(ExprUnit{span_unknown()});
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<Stmt>> expr_spawn_body_statements(mlc::Array<std::shared_ptr<Stmt>> statements) noexcept{
  if ((statements.length() == 0))   {
    return statements;
  }
  return [&]() -> mlc::Array<std::shared_ptr<Stmt>> {
auto __match_subject = statements[mlc::arith::checked_sub(statements.length(), 1)];
if (std::holds_alternative<StmtReturn>((*__match_subject))) {
const StmtReturn& stmtReturn = std::get<StmtReturn>((*__match_subject));
auto [__0, __1] = stmtReturn; return expr_spawn_body_without_last(statements);
}
if (std::holds_alternative<StmtExpr>((*__match_subject))) {
const StmtExpr& stmtExpr = std::get<StmtExpr>((*__match_subject));
auto [__0, __1] = stmtExpr; return expr_spawn_body_without_last(statements);
}
return statements;
std::abort();
}();
}
mlc::Array<std::shared_ptr<Stmt>> expr_spawn_body_without_last(mlc::Array<std::shared_ptr<Stmt>> statements) noexcept{
  auto result = mlc::Array<std::shared_ptr<Stmt>>{};
  auto index = 0;
  while ((index < mlc::arith::checked_sub(statements.length(), 1)))   {
    result.push_back(statements[index]);
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
Span stmt_span(std::shared_ptr<Stmt> statement) noexcept{
  return std::visit(overloaded{[&](const StmtLet& stmtLet) -> Span { auto [__0, __1, __2, __3, source_span] = stmtLet; return source_span; },
[&](const StmtLetPattern& stmtLetPattern) -> Span { auto [__0, __1, __2, __3, __4, __5, source_span] = stmtLetPattern; return source_span; },
[&](const StmtLetConst& stmtLetConst) -> Span { auto [__0, __1, __2, source_span] = stmtLetConst; return source_span; },
[&](const StmtExpr& stmtExpr) -> Span { auto [__0, source_span] = stmtExpr; return source_span; },
[&](const StmtBreak& stmtBreak) -> Span { auto [source_span] = stmtBreak; return source_span; },
[&](const StmtContinue& stmtContinue) -> Span { auto [source_span] = stmtContinue; return source_span; },
[&](const StmtReturn& stmtReturn) -> Span { auto [__0, source_span] = stmtReturn; return source_span; }
}, (*statement));
}
Span pattern_span(std::shared_ptr<Pattern> pattern) noexcept{
  return std::visit(overloaded{[&](const PatternWild& patternWild) -> Span { auto [source_span] = patternWild; return source_span; },
[&](const PatternIdent& patternIdent) -> Span { auto [__0, source_span] = patternIdent; return source_span; },
[&](const PatternInt& patternInt) -> Span { auto [__0, source_span] = patternInt; return source_span; },
[&](const PatternStr& patternStr) -> Span { auto [__0, source_span] = patternStr; return source_span; },
[&](const PatternStringLit& patternStringLit) -> Span { auto [__0, source_span] = patternStringLit; return source_span; },
[&](const PatternBool& patternBool) -> Span { auto [__0, source_span] = patternBool; return source_span; },
[&](const PatternUnit& patternUnit) -> Span { auto [source_span] = patternUnit; return source_span; },
[&](const PatternCtor& patternCtor) -> Span { auto [__0, __1, source_span] = patternCtor; return source_span; },
[&](const PatternRecord& patternRecord) -> Span { auto [__0, __1, source_span] = patternRecord; return source_span; },
[&](const PatternTuple& patternTuple) -> Span { auto [__0, source_span] = patternTuple; return source_span; },
[&](const PatternArray& patternArray) -> Span { auto [__0, __1, source_span] = patternArray; return source_span; },
[&](const PatternOr& patternOr) -> Span { auto [__0, source_span] = patternOr; return source_span; }
}, (*pattern));
}
mlc::String param_name(std::shared_ptr<Param> p) noexcept{
  return p->name;
}
std::shared_ptr<TypeExpr> param_type_value(std::shared_ptr<Param> parameter) noexcept{
  return parameter->type_value;
}
bool param_is_mut(std::shared_ptr<Param> p) noexcept{
  return p->is_mut;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
  auto unwrapped = decl;
  return std::visit(overloaded{[&](const DeclExported& declExported) { auto [inner] = declExported; return inner; },
[&](const DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5, __6] = declFn; return decl; },
[&](const DeclType& declType) { auto [__0, __1, __2, __3, __4] = declType; return decl; },
[&](const DeclTypeAlias& declTypeAlias) { auto [__0, __1, __2, __3] = declTypeAlias; return decl; },
[&](const DeclTrait& declTrait) { auto [__0, __1, __2, __3] = declTrait; return decl; },
[&](const DeclExtend& declExtend) { auto [__0, __1, __2, __3] = declExtend; return decl; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; return decl; },
[&](const DeclExternLib& declExternLib) { auto [__0, __1] = declExternLib; return decl; },
[&](const DeclExternType& declExternType) { auto [__0, __1, __2, __3, __4, __5] = declExternType; return decl; },
[&](const DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return decl; },
[&](const DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return decl; }
}, (*unwrapped));
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
  auto unwrapped = decl;
  return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5, __6] = declFn; return name; },
[&](const DeclType& declType) { auto [name, __1, __2, __3, __4] = declType; return name; },
[&](const DeclTypeAlias& declTypeAlias) { auto [name, __1, __2, __3] = declTypeAlias; return name; },
[&](const DeclTrait& declTrait) { auto [name, __1, __2, __3] = declTrait; return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1, __2, __3] = declExtend; return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const DeclExternLib& declExternLib) { auto [__0, __1] = declExternLib; return mlc::String("", 0); },
[&](const DeclExternType& declExternType) { auto [type_name, __1, __2, __3, __4, __5] = declExternType; return type_name; },
[&](const DeclExported& declExported) { auto [inner] = declExported; return decl_name(inner); },
[&](const DeclAssocType& declAssocType) { auto [name, __1] = declAssocType; return name; },
[&](const DeclAssocBind& declAssocBind) { auto [name, __1, __2] = declAssocBind; return name; }
}, (*unwrapped));
}
Span decl_name_span(std::shared_ptr<Decl> decl) noexcept{
  auto unwrapped = decl;
  return [&]() -> Span {
auto __match_subject = unwrapped;
if (std::holds_alternative<DeclType>((*__match_subject))) {
const DeclType& declType = std::get<DeclType>((*__match_subject));
auto [__0, __1, __2, __3, name_span] = declType; return name_span;
}
if (std::holds_alternative<DeclTypeAlias>((*__match_subject))) {
const DeclTypeAlias& declTypeAlias = std::get<DeclTypeAlias>((*__match_subject));
auto [__0, __1, __2, name_span] = declTypeAlias; return name_span;
}
if (std::holds_alternative<DeclTrait>((*__match_subject))) {
const DeclTrait& declTrait = std::get<DeclTrait>((*__match_subject));
auto [__0, __1, __2, name_span] = declTrait; return name_span;
}
if (std::holds_alternative<DeclExtend>((*__match_subject))) {
const DeclExtend& declExtend = std::get<DeclExtend>((*__match_subject));
auto [__0, __1, __2, name_span] = declExtend; return name_span;
}
if (std::holds_alternative<DeclExported>((*__match_subject))) {
const DeclExported& declExported = std::get<DeclExported>((*__match_subject));
auto [inner] = declExported; return decl_name_span(inner);
}
return span_unknown();
std::abort();
}();
}
Span decl_span(std::shared_ptr<Decl> declaration) noexcept{
  return [&]() -> Span {
auto __match_subject = decl_inner(declaration);
if (std::holds_alternative<DeclFn>((*__match_subject))) {
const DeclFn& declFn = std::get<DeclFn>((*__match_subject));
auto [__0, __1, __2, __3, __4, body, __6] = declFn; return expr_span(body);
}
if (std::holds_alternative<DeclType>((*__match_subject))) {
const DeclType& declType = std::get<DeclType>((*__match_subject));
auto [__0, __1, __2, __3, name_span] = declType; return name_span;
}
if (std::holds_alternative<DeclTypeAlias>((*__match_subject))) {
const DeclTypeAlias& declTypeAlias = std::get<DeclTypeAlias>((*__match_subject));
auto [__0, __1, __2, name_span] = declTypeAlias; return name_span;
}
if (std::holds_alternative<DeclTrait>((*__match_subject))) {
const DeclTrait& declTrait = std::get<DeclTrait>((*__match_subject));
auto [__0, __1, __2, name_span] = declTrait; return name_span;
}
if (std::holds_alternative<DeclExtend>((*__match_subject))) {
const DeclExtend& declExtend = std::get<DeclExtend>((*__match_subject));
auto [__0, __1, __2, name_span] = declExtend; return name_span;
}
if (std::holds_alternative<DeclAssocBind>((*__match_subject))) {
const DeclAssocBind& declAssocBind = std::get<DeclAssocBind>((*__match_subject));
auto [__0, __1, span] = declAssocBind; return span;
}
if (std::holds_alternative<DeclAssocType>((*__match_subject))) {
const DeclAssocType& declAssocType = std::get<DeclAssocType>((*__match_subject));
auto [__0, span] = declAssocType; return span;
}
if (std::holds_alternative<DeclExternLib>((*__match_subject))) {
const DeclExternLib& declExternLib = std::get<DeclExternLib>((*__match_subject));
auto [__0, span] = declExternLib; return span;
}
if (std::holds_alternative<DeclExternType>((*__match_subject))) {
const DeclExternType& declExternType = std::get<DeclExternType>((*__match_subject));
auto [__0, __1, __2, __3, __4, span] = declExternType; return span;
}
if (std::holds_alternative<DeclExported>((*__match_subject))) {
const DeclExported& declExported = std::get<DeclExported>((*__match_subject));
auto [inner] = declExported; return decl_span(inner);
}
return span_unknown();
std::abort();
}();
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String>& dst, mlc::Array<mlc::String> src) noexcept{
  auto i = 0;
  while ((i < src.length()))   {
    dst.push_back(src[i]);
    (i = mlc::arith::checked_add(i, 1));
  }
  return dst;
}

} // namespace ast
