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
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct PatRecord;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, PatRecord>;
struct ExprInt;
struct ExprStr;
struct ExprBool;
struct ExprUnit;
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
struct ExprQuestion;
struct ExprExtern;
struct ExprLambda;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprExtern, ExprLambda>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn>;
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
using Decl = std::variant<DeclFn, DeclType, DeclTrait, DeclExtend, DeclImport, DeclExported>;
template<typename T>
struct Ok;
template<typename E>
struct Err;
template<typename T, typename E>
using Result = std::variant<Ok<T>, Err<E>>;
struct TyI32 {};
struct TyString {};
struct TyBool {};
struct TyUnit {};
struct TyNamed {mlc::String field0;};
struct TyArray {std::shared_ptr<TypeExpr> field0;};
struct TyShared {std::shared_ptr<TypeExpr> field0;};
struct TyGeneric {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct TyFn {mlc::Array<std::shared_ptr<TypeExpr>> field0;std::shared_ptr<TypeExpr> field1;};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct PatRecord {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct FieldPat {mlc::String name;std::shared_ptr<Pat> pat;};
struct MatchArm {std::shared_ptr<Pat> pat;std::shared_ptr<Expr> body;};
struct FieldVal {mlc::String name;std::shared_ptr<Expr> val;};
struct ExprInt {int field0;};
struct ExprStr {mlc::String field0;};
struct ExprBool {bool field0;};
struct ExprUnit {};
struct ExprIdent {mlc::String field0;};
struct ExprBin {mlc::String field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;};
struct ExprUn {mlc::String field0;std::shared_ptr<Expr> field1;};
struct ExprCall {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Expr>> field1;};
struct ExprMethod {std::shared_ptr<Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<Expr>> field2;};
struct ExprField {std::shared_ptr<Expr> field0;mlc::String field1;};
struct ExprIndex {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;};
struct ExprIf {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;};
struct ExprBlock {mlc::Array<std::shared_ptr<Stmt>> field0;std::shared_ptr<Expr> field1;};
struct ExprWhile {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Stmt>> field1;};
struct ExprFor {mlc::String field0;std::shared_ptr<Expr> field1;mlc::Array<std::shared_ptr<Stmt>> field2;};
struct ExprMatch {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<MatchArm>> field1;};
struct ExprRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldVal>> field1;};
struct ExprRecordUpdate {mlc::String field0;std::shared_ptr<Expr> field1;mlc::Array<std::shared_ptr<FieldVal>> field2;};
struct ExprArray {mlc::Array<std::shared_ptr<Expr>> field0;};
struct ExprQuestion {std::shared_ptr<Expr> field0;};
struct ExprExtern {};
struct ExprLambda {mlc::Array<mlc::String> field0;std::shared_ptr<Expr> field1;};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct DeclFn {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<mlc::Array<mlc::String>> field2;mlc::Array<std::shared_ptr<Param>> field3;std::shared_ptr<TypeExpr> field4;std::shared_ptr<Expr> field5;};
struct DeclType {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<TypeVariant>> field2;};
struct DeclTrait {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<Decl>> field2;};
struct DeclExtend {mlc::String field0;mlc::String field1;mlc::Array<std::shared_ptr<Decl>> field2;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
template<typename T>
struct Ok {T field0;};
template<typename E>
struct Err {E field0;};
mlc::String param_name(std::shared_ptr<Param> p) noexcept;
std::shared_ptr<TypeExpr> param_typ(std::shared_ptr<Param> p) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;

} // namespace ast

#endif // AST_HPP
