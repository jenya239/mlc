#ifndef SEMANTIC_IR_HPP
#define SEMANTIC_IR_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "ast.hpp"

namespace semantic_ir {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct SMatchArm {std::shared_ptr<ast::Pat> pat;std::shared_ptr<semantic_ir::SExpr> body;};

struct SFieldVal {mlc::String name;std::shared_ptr<semantic_ir::SExpr> val;};

struct SExprInt {int field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SExprStr {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SExprBool {bool field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SExprUnit {std::shared_ptr<registry::Type> field0;ast::Span field1;};
struct SExprExtern {std::shared_ptr<registry::Type> field0;ast::Span field1;};
struct SExprIdent {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SExprBin {mlc::String field0;std::shared_ptr<semantic_ir::SExpr> field1;std::shared_ptr<semantic_ir::SExpr> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SExprUn {mlc::String field0;std::shared_ptr<semantic_ir::SExpr> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprCall {std::shared_ptr<semantic_ir::SExpr> field0;mlc::Array<std::shared_ptr<semantic_ir::SExpr>> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprMethod {std::shared_ptr<semantic_ir::SExpr> field0;mlc::String field1;mlc::Array<std::shared_ptr<semantic_ir::SExpr>> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SExprField {std::shared_ptr<semantic_ir::SExpr> field0;mlc::String field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprIndex {std::shared_ptr<semantic_ir::SExpr> field0;std::shared_ptr<semantic_ir::SExpr> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprIf {std::shared_ptr<semantic_ir::SExpr> field0;std::shared_ptr<semantic_ir::SExpr> field1;std::shared_ptr<semantic_ir::SExpr> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SExprBlock {mlc::Array<std::shared_ptr<semantic_ir::SStmt>> field0;std::shared_ptr<semantic_ir::SExpr> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprWhile {std::shared_ptr<semantic_ir::SExpr> field0;mlc::Array<std::shared_ptr<semantic_ir::SStmt>> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprFor {mlc::String field0;std::shared_ptr<semantic_ir::SExpr> field1;mlc::Array<std::shared_ptr<semantic_ir::SStmt>> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SExprMatch {std::shared_ptr<semantic_ir::SExpr> field0;mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprRecord {mlc::String field0;mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExprRecordUpdate {mlc::String field0;std::shared_ptr<semantic_ir::SExpr> field1;mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SExprArray {mlc::Array<std::shared_ptr<semantic_ir::SExpr>> field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SExprTuple {mlc::Array<std::shared_ptr<semantic_ir::SExpr>> field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SExprQuestion {std::shared_ptr<semantic_ir::SExpr> field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SExprLambda {mlc::Array<mlc::String> field0;std::shared_ptr<semantic_ir::SExpr> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SExpr {std::variant<SExprInt, SExprStr, SExprBool, SExprUnit, SExprExtern, SExprIdent, SExprBin, SExprUn, SExprCall, SExprMethod, SExprField, SExprIndex, SExprIf, SExprBlock, SExprWhile, SExprFor, SExprMatch, SExprRecord, SExprRecordUpdate, SExprArray, SExprTuple, SExprQuestion, SExprLambda> _;};


struct SStmtLet {mlc::String field0;bool field1;std::shared_ptr<semantic_ir::SExpr> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SStmtLetPat {std::shared_ptr<ast::Pat> field0;bool field1;std::shared_ptr<semantic_ir::SExpr> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SStmtLetConst {mlc::String field0;std::shared_ptr<semantic_ir::SExpr> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SStmtExpr {std::shared_ptr<semantic_ir::SExpr> field0;ast::Span field1;};
struct SStmtReturn {std::shared_ptr<semantic_ir::SExpr> field0;ast::Span field1;};
struct SStmtBreak {ast::Span field0;};
struct SStmtContinue {ast::Span field0;};
struct SStmt {std::variant<SStmtLet, SStmtLetPat, SStmtLetConst, SStmtExpr, SStmtReturn, SStmtBreak, SStmtContinue> _;};


struct SDeclFn;struct SDeclType;struct SDeclTrait;struct SDeclExtend;struct SDeclImport;struct SDeclExported;using SDecl = std::variant<SDeclFn, SDeclType, SDeclTrait, SDeclExtend, SDeclImport, SDeclExported>;struct SDeclFn {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<mlc::Array<mlc::String>> field2;mlc::Array<std::shared_ptr<ast::Param>> field3;std::shared_ptr<registry::Type> field4;std::shared_ptr<semantic_ir::SExpr> field5;};struct SDeclType {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<ast::TypeVariant>> field2;};struct SDeclTrait {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<semantic_ir::SDecl>> field2;};struct SDeclExtend {mlc::String field0;mlc::String field1;mlc::Array<std::shared_ptr<semantic_ir::SDecl>> field2;};struct SDeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};struct SDeclExported {std::shared_ptr<semantic_ir::SDecl> field0;};

struct SProgram {mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls;};

struct SNamespaceImportAlias {mlc::String alias;mlc::String module_path;};

struct SLoadItem {mlc::String path;mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls;mlc::Array<mlc::String> imports;mlc::Array<semantic_ir::SNamespaceImportAlias> namespace_import_aliases;};

std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<semantic_ir::SExpr> expression) noexcept;

ast::Span sexpr_span(std::shared_ptr<semantic_ir::SExpr> expression) noexcept;

std::shared_ptr<semantic_ir::SDecl> sdecl_inner(std::shared_ptr<semantic_ir::SDecl> declaration) noexcept;

} // namespace semantic_ir

#endif // SEMANTIC_IR_HPP
