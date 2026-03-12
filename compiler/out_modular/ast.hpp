#ifndef AST_HPP
#define AST_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace ast {

struct Expr;
struct Stmt;

struct TyI32;struct TyString;struct TyBool;struct TyUnit;struct TyNamed;struct TyArray;struct TyShared;struct TyGeneric;struct TyFn;using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn>;struct TyI32 {};struct TyString {};struct TyBool {};struct TyUnit {};struct TyNamed {mlc::String field0;};struct TyArray {std::shared_ptr<ast::TypeExpr> field0;};struct TyShared {std::shared_ptr<ast::TypeExpr> field0;};struct TyGeneric {mlc::String field0;mlc::Array<std::shared_ptr<ast::TypeExpr>> field1;};struct TyFn {mlc::Array<std::shared_ptr<ast::TypeExpr>> field0;std::shared_ptr<ast::TypeExpr> field1;};

struct PatWild;struct PatIdent;struct PatInt;struct PatStr;struct PatBool;struct PatUnit;struct PatCtor;using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor>;struct PatWild {};struct PatIdent {mlc::String field0;};struct PatInt {int field0;};struct PatStr {mlc::String field0;};struct PatBool {bool field0;};struct PatUnit {};struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<ast::Pat>> field1;};

struct FieldPat {mlc::String name;std::shared_ptr<ast::Pat> pat;};

struct MatchArm {std::shared_ptr<ast::Pat> pat;std::shared_ptr<ast::Expr> body;};

struct FieldVal {mlc::String name;std::shared_ptr<ast::Expr> val;};

struct ExprInt {int field0;};
struct ExprStr {mlc::String field0;};
struct ExprBool {bool field0;};
struct ExprUnit {};
struct ExprIdent {mlc::String field0;};
struct ExprBin {mlc::String field0;std::shared_ptr<ast::Expr> field1;std::shared_ptr<ast::Expr> field2;};
struct ExprUn {mlc::String field0;std::shared_ptr<ast::Expr> field1;};
struct ExprCall {std::shared_ptr<ast::Expr> field0;mlc::Array<std::shared_ptr<ast::Expr>> field1;};
struct ExprMethod {std::shared_ptr<ast::Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<ast::Expr>> field2;};
struct ExprField {std::shared_ptr<ast::Expr> field0;mlc::String field1;};
struct ExprIndex {std::shared_ptr<ast::Expr> field0;std::shared_ptr<ast::Expr> field1;};
struct ExprIf {std::shared_ptr<ast::Expr> field0;std::shared_ptr<ast::Expr> field1;std::shared_ptr<ast::Expr> field2;};
struct ExprBlock {mlc::Array<std::shared_ptr<ast::Stmt>> field0;std::shared_ptr<ast::Expr> field1;};
struct ExprWhile {std::shared_ptr<ast::Expr> field0;mlc::Array<std::shared_ptr<ast::Stmt>> field1;};
struct ExprFor {mlc::String field0;std::shared_ptr<ast::Expr> field1;mlc::Array<std::shared_ptr<ast::Stmt>> field2;};
struct ExprMatch {std::shared_ptr<ast::Expr> field0;mlc::Array<std::shared_ptr<ast::MatchArm>> field1;};
struct ExprRecord {mlc::String field0;mlc::Array<std::shared_ptr<ast::FieldVal>> field1;};
struct ExprRecordUpdate {mlc::String field0;std::shared_ptr<ast::Expr> field1;mlc::Array<std::shared_ptr<ast::FieldVal>> field2;};
struct ExprArray {mlc::Array<std::shared_ptr<ast::Expr>> field0;};
struct ExprQuestion {std::shared_ptr<ast::Expr> field0;};
struct ExprLambda {mlc::Array<mlc::String> field0;std::shared_ptr<ast::Expr> field1;};
struct Expr {std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda> _;};


struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<ast::TypeExpr> field2;std::shared_ptr<ast::Expr> field3;};
struct StmtExpr {std::shared_ptr<ast::Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<ast::Expr> field0;};
struct Stmt {std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn> _;};


struct Param {mlc::String name;bool is_mut;std::shared_ptr<ast::TypeExpr> typ;};

struct FieldDef {mlc::String name;std::shared_ptr<ast::TypeExpr> typ;};

struct VarUnit {mlc::String field0;};struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<ast::TypeExpr>> field1;};struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<ast::FieldDef>> field1;};using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord>;

struct DeclFn;struct DeclType;struct DeclExtend;struct DeclImport;struct DeclExported;using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported>;struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<ast::Param>> field1;std::shared_ptr<ast::TypeExpr> field2;std::shared_ptr<ast::Expr> field3;};struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<ast::TypeVariant>> field1;};struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<ast::Decl>> field1;};struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};struct DeclExported {std::shared_ptr<ast::Decl> field0;};

struct Program {mlc::Array<std::shared_ptr<ast::Decl>> decls;};

std::shared_ptr<ast::Decl> decl_inner(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;

} // namespace ast

#endif // AST_HPP
