#ifndef AST_HPP
#define AST_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace ast {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct Span {mlc::String file;int line;int column;};

struct Diagnostic {mlc::String message;ast::Span span;mlc::String severity;mlc::String code;};

struct TyI32;struct TyString;struct TyBool;struct TyUnit;struct TyNamed;struct TyArray;struct TyShared;struct TyGeneric;struct TyFn;struct TyAssoc;using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, TyAssoc>;struct TyI32 {};struct TyString {};struct TyBool {};struct TyUnit {};struct TyNamed {mlc::String field0;};struct TyArray {std::shared_ptr<ast::TypeExpr> field0;};struct TyShared {std::shared_ptr<ast::TypeExpr> field0;};struct TyGeneric {mlc::String field0;mlc::Array<std::shared_ptr<ast::TypeExpr>> field1;};struct TyFn {mlc::Array<std::shared_ptr<ast::TypeExpr>> field0;std::shared_ptr<ast::TypeExpr> field1;};struct TyAssoc {mlc::String field0;mlc::String field1;};

struct PatternWild;struct PatternIdent;struct PatternInt;struct PatternStr;struct PatternStringLit;struct PatternBool;struct PatternUnit;struct PatternCtor;struct PatternRecord;struct PatternTuple;struct PatternArray;struct PatternOr;using Pattern = std::variant<PatternWild, PatternIdent, PatternInt, PatternStr, PatternStringLit, PatternBool, PatternUnit, PatternCtor, PatternRecord, PatternTuple, PatternArray, PatternOr>;struct PatternWild {ast::Span field0;};struct PatternIdent {mlc::String field0;ast::Span field1;};struct PatternInt {int field0;ast::Span field1;};struct PatternStr {mlc::String field0;ast::Span field1;};struct PatternStringLit {mlc::String field0;ast::Span field1;};struct PatternBool {bool field0;ast::Span field1;};struct PatternUnit {ast::Span field0;};struct PatternCtor {mlc::String field0;mlc::Array<std::shared_ptr<ast::Pattern>> field1;ast::Span field2;};struct PatternRecord {mlc::String field0;mlc::Array<std::shared_ptr<ast::Pattern>> field1;ast::Span field2;};struct PatternTuple {mlc::Array<std::shared_ptr<ast::Pattern>> field0;ast::Span field1;};struct PatternArray {mlc::Array<std::shared_ptr<ast::Pattern>> field0;mlc::String field1;ast::Span field2;};struct PatternOr {mlc::Array<std::shared_ptr<ast::Pattern>> field0;ast::Span field1;};

struct FieldPattern {mlc::String name;std::shared_ptr<ast::Pattern> pattern;};

struct MatchArm {std::shared_ptr<ast::Pattern> pattern;bool has_guard;std::shared_ptr<ast::Expr> when_condition;std::shared_ptr<ast::Expr> body;};

struct FieldVal {mlc::String name;std::shared_ptr<ast::Expr> value;};

struct RecordLitFields {mlc::Array<std::shared_ptr<ast::FieldVal>> field0;};struct RecordLitSpread {std::shared_ptr<ast::Expr> field0;};using RecordLitPart = std::variant<RecordLitFields, RecordLitSpread>;

struct ExprInt {int field0;ast::Span field1;};
struct ExprStr {mlc::String field0;ast::Span field1;};
struct ExprBool {bool field0;ast::Span field1;};
struct ExprUnit {ast::Span field0;};
struct ExprFloat {mlc::String field0;ast::Span field1;};
struct ExprI64 {mlc::String field0;ast::Span field1;};
struct ExprU8 {mlc::String field0;ast::Span field1;};
struct ExprUsize {mlc::String field0;ast::Span field1;};
struct ExprChar {mlc::String field0;ast::Span field1;};
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
struct ExprRecord {mlc::String field0;mlc::Array<ast::RecordLitPart> field1;ast::Span field2;};
struct ExprRecordUpdate {mlc::String field0;std::shared_ptr<ast::Expr> field1;mlc::Array<std::shared_ptr<ast::FieldVal>> field2;ast::Span field3;};
struct ExprArray {mlc::Array<std::shared_ptr<ast::Expr>> field0;ast::Span field1;};
struct ExprTuple {mlc::Array<std::shared_ptr<ast::Expr>> field0;ast::Span field1;};
struct ExprQuestion {std::shared_ptr<ast::Expr> field0;ast::Span field1;};
struct ExprExtern {ast::Span field0;};
struct ExprLambda {mlc::Array<mlc::String> field0;std::shared_ptr<ast::Expr> field1;ast::Span field2;};
struct ExprNamedArg {mlc::String field0;std::shared_ptr<ast::Expr> field1;ast::Span field2;};
struct ExprWith {std::shared_ptr<ast::Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<ast::Stmt>> field2;ast::Span field3;};
struct Expr {std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprFloat, ExprI64, ExprU8, ExprUsize, ExprChar, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprTuple, ExprQuestion, ExprExtern, ExprLambda, ExprNamedArg, ExprWith> _;};


struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<ast::TypeExpr> field2;std::shared_ptr<ast::Expr> field3;ast::Span field4;};
struct StmtLetPattern {std::shared_ptr<ast::Pattern> field0;bool field1;std::shared_ptr<ast::TypeExpr> field2;std::shared_ptr<ast::Expr> field3;bool field4;std::shared_ptr<ast::Expr> field5;ast::Span field6;};
struct StmtLetConst {mlc::String field0;std::shared_ptr<ast::TypeExpr> field1;std::shared_ptr<ast::Expr> field2;ast::Span field3;};
struct StmtExpr {std::shared_ptr<ast::Expr> field0;ast::Span field1;};
struct StmtBreak {ast::Span field0;};
struct StmtContinue {ast::Span field0;};
struct StmtReturn {std::shared_ptr<ast::Expr> field0;ast::Span field1;};
struct Stmt {std::variant<StmtLet, StmtLetPattern, StmtLetConst, StmtExpr, StmtBreak, StmtContinue, StmtReturn> _;};


struct Param {mlc::String name;bool is_mut;std::shared_ptr<ast::TypeExpr> type_value;bool has_default;std::shared_ptr<ast::Expr> default_;std::shared_ptr<ast::Pattern> param_pattern;};

struct FieldDef {mlc::String name;std::shared_ptr<ast::TypeExpr> type_value;bool has_default_expression;std::shared_ptr<ast::Expr> default_expression;};

struct WhereClauseBound {mlc::String parameter_name;mlc::Array<mlc::String> traits;};

struct VarUnit {mlc::String field0;bool field1;};struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<ast::TypeExpr>> field1;bool field2;};struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<ast::FieldDef>> field1;bool field2;};using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord>;

struct DeclFn;struct DeclType;struct DeclTypeAlias;struct DeclTrait;struct DeclExtend;struct DeclImport;struct DeclExported;struct DeclAssocType;struct DeclAssocBind;using Decl = std::variant<DeclFn, DeclType, DeclTypeAlias, DeclTrait, DeclExtend, DeclImport, DeclExported, DeclAssocType, DeclAssocBind>;struct DeclFn {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<mlc::Array<mlc::String>> field2;mlc::Array<std::shared_ptr<ast::Param>> field3;std::shared_ptr<ast::TypeExpr> field4;std::shared_ptr<ast::Expr> field5;mlc::Array<ast::WhereClauseBound> field6;};struct DeclType {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<ast::TypeVariant>> field2;mlc::Array<mlc::String> field3;ast::Span field4;};struct DeclTypeAlias {mlc::String field0;mlc::Array<mlc::String> field1;std::shared_ptr<ast::TypeExpr> field2;ast::Span field3;};struct DeclTrait {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<ast::Decl>> field2;ast::Span field3;};struct DeclExtend {mlc::String field0;mlc::String field1;mlc::Array<std::shared_ptr<ast::Decl>> field2;ast::Span field3;};struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};struct DeclExported {std::shared_ptr<ast::Decl> field0;};struct DeclAssocType {mlc::String field0;ast::Span field1;};struct DeclAssocBind {mlc::String field0;std::shared_ptr<ast::TypeExpr> field1;ast::Span field2;};

struct Program {mlc::Array<std::shared_ptr<ast::Decl>> decls;};

template<typename T>
struct Ok {T field0;};template<typename E>
struct Err {E field0;};template<typename T, typename E>
using Result = std::variant<Ok<T>, Err<E>>;

ast::Span span_unknown() noexcept;

std::shared_ptr<ast::Expr> expr_placeholder() noexcept;

ast::Diagnostic diagnostic_new(mlc::String severity, mlc::String message, ast::Span span) noexcept;

ast::Diagnostic diagnostic_new_with_code(mlc::String severity, mlc::String message, ast::Span span, mlc::String code) noexcept;

ast::Diagnostic diagnostic_error(mlc::String message, ast::Span span) noexcept;

ast::Diagnostic diagnostic_error_with_code(mlc::String message, ast::Span span, mlc::String code) noexcept;

mlc::String diagnostic_format(ast::Diagnostic diagnostic) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_append(mlc::Array<ast::Diagnostic> destination, mlc::Array<ast::Diagnostic> source) noexcept;

mlc::Array<mlc::String> diagnostics_to_strings(mlc::Array<ast::Diagnostic> diagnostics) noexcept;

ast::Span expr_span(std::shared_ptr<ast::Expr> expression) noexcept;

ast::Span stmt_span(std::shared_ptr<ast::Stmt> statement) noexcept;

ast::Span pattern_span(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::String param_name(std::shared_ptr<ast::Param> p) noexcept;

std::shared_ptr<ast::TypeExpr> param_type_value(std::shared_ptr<ast::Param> parameter) noexcept;

bool param_is_mut(std::shared_ptr<ast::Param> p) noexcept;

std::shared_ptr<ast::Decl> decl_inner(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::String decl_name(std::shared_ptr<ast::Decl> decl) noexcept;

ast::Span decl_name_span(std::shared_ptr<ast::Decl> decl) noexcept;

ast::Span decl_span(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String>& dst, mlc::Array<mlc::String> src) noexcept;

} // namespace ast

#endif // AST_HPP
