#ifndef AST_HPP
#define AST_HPP

#include "mlc.hpp"
#include <variant>
#include "ast_tokens.hpp"
namespace ast {

struct TyI32;
struct TyString;
struct TyBool;
struct TyUnit;
struct TyNamed;
struct TyArray;
struct TyShared;
struct TyGeneric;
struct TyFn;
struct TyAssoc;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, TyAssoc>;
struct PatternWild;
struct PatternIdent;
struct PatternInt;
struct PatternStr;
struct PatternStringLit;
struct PatternBool;
struct PatternUnit;
struct PatternCtor;
struct PatternRecord;
struct PatternTuple;
struct PatternArray;
struct PatternOr;
using Pattern = std::variant<PatternWild, PatternIdent, PatternInt, PatternStr, PatternStringLit, PatternBool, PatternUnit, PatternCtor, PatternRecord, PatternTuple, PatternArray, PatternOr>;
struct RecordLitFields;
struct RecordLitSpread;
using RecordLitPart = std::variant<RecordLitFields, RecordLitSpread>;
struct ExprInt;
struct ExprStr;
struct ExprBool;
struct ExprUnit;
struct ExprFloat;
struct ExprI64;
struct ExprU8;
struct ExprUsize;
struct ExprChar;
struct ExprIdent;
struct ExprBin;
struct ExprUn;
struct ExprCall;
struct ExprMethod;
struct ExprField;
struct ExprIndex;
struct ExprIf;
struct ExprBlock;
struct ExprWhile;
struct ExprFor;
struct ExprMatch;
struct ExprRecord;
struct ExprRecordUpdate;
struct ExprArray;
struct ExprTuple;
struct ExprQuestion;
struct ExprExtern;
struct ExprLambda;
struct ExprSpawn;
struct ExprNamedArg;
struct ExprWith;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprFloat, ExprI64, ExprU8, ExprUsize, ExprChar, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprTuple, ExprQuestion, ExprExtern, ExprLambda, ExprSpawn, ExprNamedArg, ExprWith>;
struct StmtLet;
struct StmtLetPattern;
struct StmtLetConst;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
using Stmt = std::variant<StmtLet, StmtLetPattern, StmtLetConst, StmtExpr, StmtBreak, StmtContinue, StmtReturn>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord>;
struct DeclFn;
struct DeclType;
struct DeclTypeAlias;
struct DeclTrait;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct DeclAssocType;
struct DeclAssocBind;
using Decl = std::variant<DeclFn, DeclType, DeclTypeAlias, DeclTrait, DeclExtend, DeclImport, DeclExported, DeclAssocType, DeclAssocBind>;
template<typename T>
struct Ok;
template<typename E>
struct Err;
template<typename T, typename E>
using Result = std::variant<Ok<T>, Err<E>>;
struct Span {
  mlc::String file;
  int line;
  int column;
  int start_offset;
  int end_offset;
};
struct Diagnostic {
  mlc::String message;
  Span span;
  mlc::String severity;
  mlc::String code;
};
struct TyI32 {};
struct TyString {};
struct TyBool {};
struct TyUnit {};
struct TyNamed {
  mlc::String field0;
};
struct TyArray {
  std::shared_ptr<TypeExpr> field0;
};
struct TyShared {
  std::shared_ptr<TypeExpr> field0;
};
struct TyGeneric {
  mlc::String field0;
  mlc::Array<std::shared_ptr<TypeExpr>> field1;
};
struct TyFn {
  mlc::Array<std::shared_ptr<TypeExpr>> field0;
  std::shared_ptr<TypeExpr> field1;
};
struct TyAssoc {
  mlc::String field0;
  mlc::String field1;
};
struct PatternWild {
  Span field0;
};
struct PatternIdent {
  mlc::String field0;
  Span field1;
};
struct PatternInt {
  int field0;
  Span field1;
};
struct PatternStr {
  mlc::String field0;
  Span field1;
};
struct PatternStringLit {
  mlc::String field0;
  Span field1;
};
struct PatternBool {
  bool field0;
  Span field1;
};
struct PatternUnit {
  Span field0;
};
struct PatternCtor {
  mlc::String field0;
  mlc::Array<std::shared_ptr<Pattern>> field1;
  Span field2;
};
struct PatternRecord {
  mlc::String field0;
  mlc::Array<std::shared_ptr<Pattern>> field1;
  Span field2;
};
struct PatternTuple {
  mlc::Array<std::shared_ptr<Pattern>> field0;
  Span field1;
};
struct PatternArray {
  mlc::Array<std::shared_ptr<Pattern>> field0;
  mlc::String field1;
  Span field2;
};
struct PatternOr {
  mlc::Array<std::shared_ptr<Pattern>> field0;
  Span field1;
};
struct FieldPattern {
  mlc::String name;
  std::shared_ptr<Pattern> pattern;
};
struct MatchArm {
  std::shared_ptr<Pattern> pattern;
  bool has_guard;
  std::shared_ptr<Expr> when_condition;
  std::shared_ptr<Expr> body;
};
struct FieldVal {
  mlc::String name;
  std::shared_ptr<Expr> value;
};
struct RecordLitFields {
  mlc::Array<std::shared_ptr<FieldVal>> field0;
};
struct RecordLitSpread {
  std::shared_ptr<Expr> field0;
};
struct ExprInt {
  int field0;
  Span field1;
};
struct ExprStr {
  mlc::String field0;
  Span field1;
};
struct ExprBool {
  bool field0;
  Span field1;
};
struct ExprUnit {
  Span field0;
};
struct ExprFloat {
  mlc::String field0;
  Span field1;
};
struct ExprI64 {
  mlc::String field0;
  Span field1;
};
struct ExprU8 {
  mlc::String field0;
  Span field1;
};
struct ExprUsize {
  mlc::String field0;
  Span field1;
};
struct ExprChar {
  mlc::String field0;
  Span field1;
};
struct ExprIdent {
  mlc::String field0;
  Span field1;
};
struct ExprBin {
  mlc::String field0;
  std::shared_ptr<Expr> field1;
  std::shared_ptr<Expr> field2;
  Span field3;
};
struct ExprUn {
  mlc::String field0;
  std::shared_ptr<Expr> field1;
  Span field2;
};
struct ExprCall {
  std::shared_ptr<Expr> field0;
  mlc::Array<std::shared_ptr<Expr>> field1;
  Span field2;
};
struct ExprMethod {
  std::shared_ptr<Expr> field0;
  mlc::String field1;
  mlc::Array<std::shared_ptr<Expr>> field2;
  Span field3;
};
struct ExprField {
  std::shared_ptr<Expr> field0;
  mlc::String field1;
  Span field2;
};
struct ExprIndex {
  std::shared_ptr<Expr> field0;
  std::shared_ptr<Expr> field1;
  Span field2;
};
struct ExprIf {
  std::shared_ptr<Expr> field0;
  std::shared_ptr<Expr> field1;
  std::shared_ptr<Expr> field2;
  Span field3;
};
struct ExprBlock {
  mlc::Array<std::shared_ptr<Stmt>> field0;
  std::shared_ptr<Expr> field1;
  Span field2;
};
struct ExprWhile {
  std::shared_ptr<Expr> field0;
  mlc::Array<std::shared_ptr<Stmt>> field1;
  Span field2;
};
struct ExprFor {
  mlc::String field0;
  std::shared_ptr<Expr> field1;
  mlc::Array<std::shared_ptr<Stmt>> field2;
  Span field3;
};
struct ExprMatch {
  std::shared_ptr<Expr> field0;
  mlc::Array<std::shared_ptr<MatchArm>> field1;
  Span field2;
};
struct ExprRecord {
  mlc::String field0;
  mlc::Array<RecordLitPart> field1;
  Span field2;
};
struct ExprRecordUpdate {
  mlc::String field0;
  std::shared_ptr<Expr> field1;
  mlc::Array<std::shared_ptr<FieldVal>> field2;
  Span field3;
};
struct ExprArray {
  mlc::Array<std::shared_ptr<Expr>> field0;
  Span field1;
};
struct ExprTuple {
  mlc::Array<std::shared_ptr<Expr>> field0;
  Span field1;
};
struct ExprQuestion {
  std::shared_ptr<Expr> field0;
  Span field1;
};
struct ExprExtern {
  Span field0;
};
struct ExprLambda {
  mlc::Array<mlc::String> field0;
  std::shared_ptr<Expr> field1;
  Span field2;
};
struct ExprSpawn {
  mlc::Array<std::shared_ptr<Stmt>> field0;
  Span field1;
};
struct ExprNamedArg {
  mlc::String field0;
  std::shared_ptr<Expr> field1;
  Span field2;
};
struct ExprWith {
  std::shared_ptr<Expr> field0;
  mlc::String field1;
  mlc::Array<std::shared_ptr<Stmt>> field2;
  Span field3;
};
struct StmtLet {
  mlc::String field0;
  bool field1;
  std::shared_ptr<TypeExpr> field2;
  std::shared_ptr<Expr> field3;
  Span field4;
};
struct StmtLetPattern {
  std::shared_ptr<Pattern> field0;
  bool field1;
  std::shared_ptr<TypeExpr> field2;
  std::shared_ptr<Expr> field3;
  bool field4;
  std::shared_ptr<Expr> field5;
  Span field6;
};
struct StmtLetConst {
  mlc::String field0;
  std::shared_ptr<TypeExpr> field1;
  std::shared_ptr<Expr> field2;
  Span field3;
};
struct StmtExpr {
  std::shared_ptr<Expr> field0;
  Span field1;
};
struct StmtBreak {
  Span field0;
};
struct StmtContinue {
  Span field0;
};
struct StmtReturn {
  std::shared_ptr<Expr> field0;
  Span field1;
};
struct Param {
  mlc::String name;
  bool is_mut;
  std::shared_ptr<TypeExpr> type_value;
  bool has_default;
  std::shared_ptr<Expr> default_;
  std::shared_ptr<Pattern> param_pattern;
};
struct FieldDef {
  mlc::String name;
  std::shared_ptr<TypeExpr> type_value;
  bool has_default_expression;
  std::shared_ptr<Expr> default_expression;
};
struct WhereClauseBound {
  mlc::String parameter_name;
  mlc::Array<mlc::String> traits;
};
struct VarUnit {
  mlc::String field0;
  bool field1;
};
struct VarTuple {
  mlc::String field0;
  mlc::Array<std::shared_ptr<TypeExpr>> field1;
  bool field2;
};
struct VarRecord {
  mlc::String field0;
  mlc::Array<std::shared_ptr<FieldDef>> field1;
  bool field2;
};
struct DeclFn {
  mlc::String field0;
  mlc::Array<mlc::String> field1;
  mlc::Array<mlc::Array<mlc::String>> field2;
  mlc::Array<std::shared_ptr<Param>> field3;
  std::shared_ptr<TypeExpr> field4;
  std::shared_ptr<Expr> field5;
  mlc::Array<WhereClauseBound> field6;
};
struct DeclType {
  mlc::String field0;
  mlc::Array<mlc::String> field1;
  mlc::Array<std::shared_ptr<TypeVariant>> field2;
  mlc::Array<mlc::String> field3;
  Span field4;
};
struct DeclTypeAlias {
  mlc::String field0;
  mlc::Array<mlc::String> field1;
  std::shared_ptr<TypeExpr> field2;
  Span field3;
};
struct DeclTrait {
  mlc::String field0;
  mlc::Array<mlc::String> field1;
  mlc::Array<std::shared_ptr<Decl>> field2;
  Span field3;
};
struct DeclExtend {
  mlc::String field0;
  mlc::String field1;
  mlc::Array<std::shared_ptr<Decl>> field2;
  Span field3;
};
struct DeclImport {
  mlc::String field0;
  mlc::Array<mlc::String> field1;
};
struct DeclExported {
  std::shared_ptr<Decl> field0;
};
struct DeclAssocType {
  mlc::String field0;
  Span field1;
};
struct DeclAssocBind {
  mlc::String field0;
  std::shared_ptr<TypeExpr> field1;
  Span field2;
};
struct Program {
  mlc::Array<std::shared_ptr<Decl>> decls;
};
template<typename T>
struct Ok {
  T field0;
};
template<typename E>
struct Err {
  E field0;
};
Span span_unknown() noexcept;
Span span_make(mlc::String file, int line, int column) noexcept;
std::shared_ptr<Expr> expr_placeholder() noexcept;
Diagnostic diagnostic_new(mlc::String severity, mlc::String message, Span span) noexcept;
Diagnostic diagnostic_new_with_code(mlc::String severity, mlc::String message, Span span, mlc::String code) noexcept;
Diagnostic diagnostic_error(mlc::String message, Span span) noexcept;
Diagnostic diagnostic_error_with_code(mlc::String message, Span span, mlc::String code) noexcept;
mlc::String diagnostic_format(Diagnostic diagnostic) noexcept;
mlc::Array<Diagnostic> diagnostics_append(mlc::Array<Diagnostic> destination, mlc::Array<Diagnostic> source) noexcept;
mlc::Array<mlc::String> diagnostics_to_strings(mlc::Array<Diagnostic> diagnostics) noexcept;
Span expr_span(std::shared_ptr<Expr> expression) noexcept;
std::shared_ptr<Expr> expr_spawn_body_result(mlc::Array<std::shared_ptr<Stmt>> statements) noexcept;
Span stmt_span(std::shared_ptr<Stmt> statement) noexcept;
Span pattern_span(std::shared_ptr<Pattern> pattern) noexcept;
mlc::String param_name(std::shared_ptr<Param> p) noexcept;
std::shared_ptr<TypeExpr> param_type_value(std::shared_ptr<Param> parameter) noexcept;
bool param_is_mut(std::shared_ptr<Param> p) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
Span decl_name_span(std::shared_ptr<Decl> decl) noexcept;
Span decl_span(std::shared_ptr<Decl> declaration) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String>& dst, mlc::Array<mlc::String> src) noexcept;

} // namespace ast

#endif // AST_HPP
