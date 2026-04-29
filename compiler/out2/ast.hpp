#ifndef AST_HPP
#define AST_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace ast {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct Span {mlc::String file;int line;int column;};

struct Diagnostic {mlc::String message;ast::Span span;mlc::String severity;};

struct TyI32;struct TyString;struct TyBool;struct TyUnit;struct TyNamed;struct TyArray;struct TyShared;struct TyGeneric;struct TyFn;using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn>;struct TyI32 {};struct TyString {};struct TyBool {};struct TyUnit {};struct TyNamed {mlc::String field0;};struct TyArray {std::shared_ptr<ast::TypeExpr> field0;};struct TyShared {std::shared_ptr<ast::TypeExpr> field0;};struct TyGeneric {mlc::String field0;mlc::Array<std::shared_ptr<ast::TypeExpr>> field1;};struct TyFn {mlc::Array<std::shared_ptr<ast::TypeExpr>> field0;std::shared_ptr<ast::TypeExpr> field1;};

struct PatWild;struct PatIdent;struct PatInt;struct PatStr;struct PatBool;struct PatUnit;struct PatCtor;struct PatRecord;struct PatTuple;struct PatArray;struct PatOr;using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, PatRecord, PatTuple, PatArray, PatOr>;struct PatWild {ast::Span field0;};struct PatIdent {mlc::String field0;ast::Span field1;};struct PatInt {int field0;ast::Span field1;};struct PatStr {mlc::String field0;ast::Span field1;};struct PatBool {bool field0;ast::Span field1;};struct PatUnit {ast::Span field0;};struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<ast::Pat>> field1;ast::Span field2;};struct PatRecord {mlc::String field0;mlc::Array<std::shared_ptr<ast::Pat>> field1;ast::Span field2;};struct PatTuple {mlc::Array<std::shared_ptr<ast::Pat>> field0;ast::Span field1;};struct PatArray {mlc::Array<std::shared_ptr<ast::Pat>> field0;mlc::String field1;ast::Span field2;};struct PatOr {mlc::Array<std::shared_ptr<ast::Pat>> field0;ast::Span field1;};

struct FieldPat {mlc::String name;std::shared_ptr<ast::Pat> pat;};

struct MatchArm {std::shared_ptr<ast::Pat> pat;std::shared_ptr<ast::Expr> body;};

struct FieldVal {mlc::String name;std::shared_ptr<ast::Expr> val;};

struct ExprInt {int field0;ast::Span field1;};
struct ExprStr {mlc::String field0;ast::Span field1;};
struct ExprBool {bool field0;ast::Span field1;};
struct ExprUnit {ast::Span field0;};
struct ExprIdent {mlc::String field0;ast::Span field1;};
struct ExprBin {mlc::String field0;std::shared_ptr<ast::Expr> field1;std::shared_ptr<ast::Expr> field2;ast::Span field3;};
struct ExprUn {mlc::String field0;std::shared_ptr<ast::Expr> field1;ast::Span field2;};
struct ExprCall {std::shared_ptr<ast::Expr> field0;mlc::Array<std::shared_ptr<ast::Expr>> field1;ast::Span field2;};
struct ExprMethod {std::shared_ptr<ast::Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<ast::Expr>> field2;ast::Span field3;};
struct ExprField {std::shared_ptr<ast::Expr> field0;mlc::String field1;ast::Span field2;};
struct ExprIndex {std::shared_ptr<ast::Expr> field0;std::shared_ptr<ast::Expr> field1;ast::Span field2;};
struct ExprIf {std::shared_ptr<ast::Expr> field0;std::shared_ptr<ast::Expr> field1;std::shared_ptr<ast::Expr> field2;ast::Span field3;};
struct ExprBlock {mlc::Array<std::shared_ptr<ast::Stmt>> field0;std::shared_ptr<ast::Expr> field1;ast::Span field2;};
struct ExprWhile {std::shared_ptr<ast::Expr> field0;mlc::Array<std::shared_ptr<ast::Stmt>> field1;ast::Span field2;};
struct ExprFor {mlc::String field0;std::shared_ptr<ast::Expr> field1;mlc::Array<std::shared_ptr<ast::Stmt>> field2;ast::Span field3;};
struct ExprMatch {std::shared_ptr<ast::Expr> field0;mlc::Array<std::shared_ptr<ast::MatchArm>> field1;ast::Span field2;};
struct ExprRecord {mlc::String field0;mlc::Array<std::shared_ptr<ast::FieldVal>> field1;ast::Span field2;};
struct ExprRecordUpdate {mlc::String field0;std::shared_ptr<ast::Expr> field1;mlc::Array<std::shared_ptr<ast::FieldVal>> field2;ast::Span field3;};
struct ExprArray {mlc::Array<std::shared_ptr<ast::Expr>> field0;ast::Span field1;};
struct ExprTuple {mlc::Array<std::shared_ptr<ast::Expr>> field0;ast::Span field1;};
struct ExprQuestion {std::shared_ptr<ast::Expr> field0;ast::Span field1;};
struct ExprExtern {ast::Span field0;};
struct ExprLambda {mlc::Array<mlc::String> field0;std::shared_ptr<ast::Expr> field1;ast::Span field2;};
struct Expr {std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprTuple, ExprQuestion, ExprExtern, ExprLambda> _;};


struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<ast::TypeExpr> field2;std::shared_ptr<ast::Expr> field3;ast::Span field4;};
struct StmtLetPat {std::shared_ptr<ast::Pat> field0;bool field1;std::shared_ptr<ast::TypeExpr> field2;std::shared_ptr<ast::Expr> field3;bool field4;std::shared_ptr<ast::Expr> field5;ast::Span field6;};
struct StmtLetConst {mlc::String field0;std::shared_ptr<ast::TypeExpr> field1;std::shared_ptr<ast::Expr> field2;ast::Span field3;};
struct StmtExpr {std::shared_ptr<ast::Expr> field0;ast::Span field1;};
struct StmtBreak {ast::Span field0;};
struct StmtContinue {ast::Span field0;};
struct StmtReturn {std::shared_ptr<ast::Expr> field0;ast::Span field1;};
struct Stmt {std::variant<StmtLet, StmtLetPat, StmtLetConst, StmtExpr, StmtBreak, StmtContinue, StmtReturn> _;};


struct Param {mlc::String name;bool is_mut;std::shared_ptr<ast::TypeExpr> typ;bool has_default;std::shared_ptr<ast::Expr> default_;};

struct FieldDef {mlc::String name;std::shared_ptr<ast::TypeExpr> typ;};

struct VarUnit {mlc::String field0;bool field1;};struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<ast::TypeExpr>> field1;bool field2;};struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<ast::FieldDef>> field1;bool field2;};using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord>;

struct DeclFn;struct DeclType;struct DeclTrait;struct DeclExtend;struct DeclImport;struct DeclExported;using Decl = std::variant<DeclFn, DeclType, DeclTrait, DeclExtend, DeclImport, DeclExported>;struct DeclFn {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<mlc::Array<mlc::String>> field2;mlc::Array<std::shared_ptr<ast::Param>> field3;std::shared_ptr<ast::TypeExpr> field4;std::shared_ptr<ast::Expr> field5;};struct DeclType {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<ast::TypeVariant>> field2;mlc::Array<mlc::String> field3;};struct DeclTrait {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<ast::Decl>> field2;};struct DeclExtend {mlc::String field0;mlc::String field1;mlc::Array<std::shared_ptr<ast::Decl>> field2;};struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};struct DeclExported {std::shared_ptr<ast::Decl> field0;};

struct Program {mlc::Array<std::shared_ptr<ast::Decl>> decls;};

template<typename T>
struct Ok {T field0;};template<typename E>
struct Err {E field0;};template<typename T, typename E>
using Result = std::variant<Ok<T>, Err<E>>;

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

} // namespace ast

#endif // AST_HPP
