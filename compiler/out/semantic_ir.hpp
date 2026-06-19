#ifndef SEMANTIC_IR_HPP
#define SEMANTIC_IR_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "ast.hpp"

namespace semantic_ir {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct SemanticMatchArm {std::shared_ptr<ast::Pattern> pattern;bool has_guard;std::shared_ptr<semantic_ir::SemanticExpression> when_condition;std::shared_ptr<semantic_ir::SemanticExpression> body;};

struct SemanticFieldVal {mlc::String name;std::shared_ptr<semantic_ir::SemanticExpression> value;};

struct SemanticExpressionInt {int field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionStr {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionFloat {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionI64 {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionU8 {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionUsize {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionChar {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionBool {bool field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionUnit {std::shared_ptr<registry::Type> field0;ast::Span field1;};
struct SemanticExpressionExtern {std::shared_ptr<registry::Type> field0;ast::Span field1;};
struct SemanticExpressionIdent {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionBin {mlc::String field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;std::shared_ptr<semantic_ir::SemanticExpression> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SemanticExpressionUn {mlc::String field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionCall {std::shared_ptr<semantic_ir::SemanticExpression> field0;mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> field1;mlc::Array<int> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SemanticExpressionMethod {std::shared_ptr<semantic_ir::SemanticExpression> field0;mlc::String field1;mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> field2;mlc::Array<int> field3;std::shared_ptr<registry::Type> field4;ast::Span field5;};
struct SemanticExpressionField {std::shared_ptr<semantic_ir::SemanticExpression> field0;mlc::String field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionIndex {std::shared_ptr<semantic_ir::SemanticExpression> field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionIf {std::shared_ptr<semantic_ir::SemanticExpression> field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;std::shared_ptr<semantic_ir::SemanticExpression> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SemanticExpressionBlock {mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionWhile {std::shared_ptr<semantic_ir::SemanticExpression> field0;mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionFor {mlc::String field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SemanticExpressionMatch {std::shared_ptr<semantic_ir::SemanticExpression> field0;mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionRecord {mlc::String field0;mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionRecordUpdate {mlc::String field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SemanticExpressionArray {mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionTuple {mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionQuestion {std::shared_ptr<semantic_ir::SemanticExpression> field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};
struct SemanticExpressionLambda {mlc::Array<mlc::String> field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticExpressionWith {std::shared_ptr<semantic_ir::SemanticExpression> field0;mlc::String field1;mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SemanticExpression {std::variant<SemanticExpressionInt, SemanticExpressionStr, SemanticExpressionFloat, SemanticExpressionI64, SemanticExpressionU8, SemanticExpressionUsize, SemanticExpressionChar, SemanticExpressionBool, SemanticExpressionUnit, SemanticExpressionExtern, SemanticExpressionIdent, SemanticExpressionBin, SemanticExpressionUn, SemanticExpressionCall, SemanticExpressionMethod, SemanticExpressionField, SemanticExpressionIndex, SemanticExpressionIf, SemanticExpressionBlock, SemanticExpressionWhile, SemanticExpressionFor, SemanticExpressionMatch, SemanticExpressionRecord, SemanticExpressionRecordUpdate, SemanticExpressionArray, SemanticExpressionTuple, SemanticExpressionQuestion, SemanticExpressionLambda, SemanticExpressionWith> _;};


struct SemanticStatementLet {mlc::String field0;bool field1;std::shared_ptr<semantic_ir::SemanticExpression> field2;std::shared_ptr<registry::Type> field3;ast::Span field4;};
struct SemanticStatementLetPattern {std::shared_ptr<ast::Pattern> field0;bool field1;std::shared_ptr<semantic_ir::SemanticExpression> field2;std::shared_ptr<registry::Type> field3;bool field4;std::shared_ptr<semantic_ir::SemanticExpression> field5;ast::Span field6;};
struct SemanticStatementLetConst {mlc::String field0;std::shared_ptr<semantic_ir::SemanticExpression> field1;std::shared_ptr<registry::Type> field2;ast::Span field3;};
struct SemanticStatementExpr {std::shared_ptr<semantic_ir::SemanticExpression> field0;ast::Span field1;};
struct SemanticStatementReturn {std::shared_ptr<semantic_ir::SemanticExpression> field0;ast::Span field1;};
struct SemanticStatementBreak {ast::Span field0;};
struct SemanticStatementContinue {ast::Span field0;};
struct SemanticStatement {std::variant<SemanticStatementLet, SemanticStatementLetPattern, SemanticStatementLetConst, SemanticStatementExpr, SemanticStatementReturn, SemanticStatementBreak, SemanticStatementContinue> _;};


struct SemanticDeclarationFn;struct SemanticDeclarationType;struct SemanticDeclarationTypeAlias;struct SemanticDeclarationTrait;struct SemanticDeclarationExtend;struct SemanticDeclarationImport;struct SemanticDeclarationExported;struct SemanticDeclarationAssocBind;using SemanticDeclaration = std::variant<SemanticDeclarationFn, SemanticDeclarationType, SemanticDeclarationTypeAlias, SemanticDeclarationTrait, SemanticDeclarationExtend, SemanticDeclarationImport, SemanticDeclarationExported, SemanticDeclarationAssocBind>;struct SemanticDeclarationFn {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<mlc::Array<mlc::String>> field2;mlc::Array<std::shared_ptr<ast::Param>> field3;std::shared_ptr<registry::Type> field4;std::shared_ptr<semantic_ir::SemanticExpression> field5;mlc::Array<ast::WhereClauseBound> field6;ast::Span field7;};struct SemanticDeclarationType {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<ast::TypeVariant>> field2;mlc::Array<mlc::String> field3;ast::Span field4;};struct SemanticDeclarationTypeAlias {mlc::String field0;mlc::Array<mlc::String> field1;std::shared_ptr<ast::TypeExpr> field2;ast::Span field3;};struct SemanticDeclarationTrait {mlc::String field0;mlc::Array<mlc::String> field1;mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> field2;ast::Span field3;};struct SemanticDeclarationExtend {mlc::String field0;mlc::String field1;mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> field2;ast::Span field3;};struct SemanticDeclarationImport {mlc::String field0;mlc::Array<mlc::String> field1;};struct SemanticDeclarationExported {std::shared_ptr<semantic_ir::SemanticDeclaration> field0;};struct SemanticDeclarationAssocBind {mlc::String field0;std::shared_ptr<registry::Type> field1;ast::Span field2;};

struct SemanticProgram {mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> decls;};

struct SemanticNamespaceImportAlias {mlc::String alias;mlc::String module_path;};

struct SemanticLoadItem {mlc::String path;mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> decls;mlc::Array<mlc::String> imports;mlc::Array<semantic_ir::SemanticNamespaceImportAlias> namespace_import_aliases;};

std::shared_ptr<registry::Type> sexpr_type(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

ast::Span sexpr_span(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

std::shared_ptr<semantic_ir::SemanticDeclaration> sdecl_inner(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

ast::Span sdecl_span(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

} // namespace semantic_ir

#endif // SEMANTIC_IR_HPP
