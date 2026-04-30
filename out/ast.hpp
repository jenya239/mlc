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
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct PatRecord;
struct PatTuple;
struct PatArray;
struct PatOr;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, PatRecord, PatTuple, PatArray, PatOr>;
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
struct ExprNamedArg;
struct ExprWith;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprFloat, ExprI64, ExprU8, ExprUsize, ExprChar, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprTuple, ExprQuestion, ExprExtern, ExprLambda, ExprNamedArg, ExprWith>;
struct StmtLet;
struct StmtLetPat;
struct StmtLetConst;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
using Stmt = std::variant<StmtLet, StmtLetPat, StmtLetConst, StmtExpr, StmtBreak, StmtContinue, StmtReturn>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord>;
struct DeclFn;
struct DeclType;
struct DeclTrait;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct DeclAssocType;
struct DeclAssocBind;
using Decl = std::variant<DeclFn, DeclType, DeclTrait, DeclExtend, DeclImport, DeclExported, DeclAssocType, DeclAssocBind>;
template<typename T>
struct Ok;
template<typename E>
struct Err;
template<typename T, typename E>
using Result = std::variant<Ok<T>, Err<E>>;
struct Span {mlc::String file;int line;int column;};
struct Diagnostic {mlc::String message;Span span;mlc::String severity;};
struct TyI32 {};
struct TyString {};
struct TyBool {};
struct TyUnit {};
struct TyNamed {mlc::String field0;};
struct TyArray {std::shared_ptr<TypeExpr> field0;};
struct TyShared {std::shared_ptr<TypeExpr> field0;};
struct TyGeneric {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct TyFn {mlc::Array<std::shared_ptr<TypeExpr>> field0;std::shared_ptr<TypeExpr> field1;};
struct TyAssoc {mlc::String field0;mlc::String field1;};
struct PatWild {Span field0;};
struct PatIdent {mlc::String field0;Span field1;};
struct PatInt {int field0;Span field1;};
struct PatStr {mlc::String field0;Span field1;};
struct PatBool {bool field0;Span field1;};
struct PatUnit {Span field0;};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;Span field2;};
struct PatRecord {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;Span field2;};
struct PatTuple {mlc::Array<std::shared_ptr<Pat>> field0;Span field1;};
struct PatArray {mlc::Array<std::shared_ptr<Pat>> field0;mlc::String field1;Span field2;};
struct PatOr {mlc::Array<std::shared_ptr<Pat>> field0;Span field1;};
struct FieldPat {mlc::String name;std::shared_ptr<Pat> pat;};
struct MatchArm {std::shared_ptr<Pat> pat;std::shared_ptr<Expr> body;};
struct FieldVal {mlc::String name;std::shared_ptr<Expr> val;};
struct ExprInt {int field0;Span field1;};
struct ExprStr {mlc::String field0;Span field1;};
struct ExprBool {bool field0;Span field1;};
struct ExprUnit {Span field0;};
struct ExprFloat {mlc::String field0;Span field1;};
struct ExprI64 {mlc::String field0;Span field1;};
struct ExprU8 {mlc::String field0;Span field1;};
struct ExprUsize {mlc::String field0;Span field1;};
struct ExprChar {mlc::String field0;Span field1;};
struct ExprIdent {mlc::String field0;Span field1;};
struct ExprBin {mlc::String field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;Span field3;};
struct ExprUn {mlc::String field0;std::shared_ptr<Expr> field1;Span field2;};
struct ExprCall {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Expr>> field1;Span field2;};
struct ExprMethod {std::shared_ptr<Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<Expr>> field2;Span field3;};
struct ExprField {std::shared_ptr<Expr> field0;mlc::String field1;Span field2;};
struct ExprIndex {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;Span field2;};
struct ExprIf {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;Span field3;};
struct ExprBlock {mlc::Array<std::shared_ptr<Stmt>> field0;std::shared_ptr<Expr> field1;Span field2;};
struct ExprWhile {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Stmt>> field1;Span field2;};
struct ExprFor {mlc::String field0;std::shared_ptr<Expr> field1;mlc::Array<std::shared_ptr<Stmt>> field2;Span field3;};
struct ExprMatch {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<MatchArm>> field1;Span field2;};
struct ExprRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldVal>> field1;Span field2;};
struct ExprRecordUpdate {mlc::String field0;std::shared_ptr<Expr> field1;mlc::Array<std::shared_ptr<FieldVal>> field2;Span field3;};
struct ExprArray {mlc::Array<std::shared_ptr<Expr>> field0;Span field1;};
struct ExprTuple {mlc::Array<std::shared_ptr<Expr>> field0;Span field1;};
struct ExprQuestion {std::shared_ptr<Expr> field0;Span field1;};
struct ExprExtern {Span field0;};
struct ExprLambda {mlc::Array<mlc::String> field0;std::shared_ptr<Expr> field1;Span field2;};
struct ExprNamedArg {mlc::String field0;std::shared_ptr<Expr> field1;Span field2;};
struct ExprWith {std::shared_ptr<Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<Stmt>> field2;Span field3;};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;Span field4;};
struct StmtLetPat {std::shared_ptr<Pat> field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;bool field4;std::shared_ptr<Expr> field5;Span field6;};
struct StmtLetConst {mlc::String field0;std::shared_ptr<TypeExpr> field1;std::shared_ptr<Expr> field2;Span field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;Span field1;};
struct StmtBreak {Span field0;};
struct StmtContinue {Span field0;};
struct StmtReturn {std::shared_ptr<Expr> field0;Span field1;};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;bool has_default;std::shared_ptr<Expr> default_;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;bool field1;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;bool field2;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;bool field2;};
struct DeclFn {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<mlc::Array<mlc::String>> field2;mlc::Array<std::shared_ptr<Param>> field3;std::shared_ptr<TypeExpr> field4;std::shared_ptr<Expr> field5;};
struct DeclType {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<TypeVariant>> field2;mlc::Array<mlc::String> field3;};
struct DeclTrait {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<Decl>> field2;};
struct DeclExtend {mlc::String field0;mlc::String field1;mlc::Array<std::shared_ptr<Decl>> field2;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct DeclAssocType {mlc::String field0;Span field1;};
struct DeclAssocBind {mlc::String field0;std::shared_ptr<TypeExpr> field1;Span field2;};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
template<typename T>
struct Ok {T field0;};
template<typename E>
struct Err {E field0;};
Span span_unknown() noexcept;
std::shared_ptr<Expr> expr_placeholder() noexcept;
Diagnostic diagnostic_new(mlc::String severity, mlc::String message, Span span) noexcept;
Diagnostic diagnostic_error(mlc::String message, Span span) noexcept;
mlc::String diagnostic_format(Diagnostic diagnostic) noexcept;
mlc::Array<Diagnostic> diagnostics_append(mlc::Array<Diagnostic> destination, mlc::Array<Diagnostic> source) noexcept;
mlc::Array<Diagnostic> infer_messages_as_diagnostics(mlc::Array<mlc::String> messages) noexcept;
mlc::Array<mlc::String> diagnostics_to_strings(mlc::Array<Diagnostic> diagnostics) noexcept;
Span expr_span(std::shared_ptr<Expr> expression) noexcept;
Span stmt_span(std::shared_ptr<Stmt> statement) noexcept;
Span pat_span(std::shared_ptr<Pat> pattern) noexcept;
mlc::String param_name(std::shared_ptr<Param> p) noexcept;
std::shared_ptr<TypeExpr> param_typ(std::shared_ptr<Param> p) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;

} // namespace ast

#endif // AST_HPP
