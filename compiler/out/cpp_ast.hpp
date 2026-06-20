#ifndef CPP_AST_HPP
#define CPP_AST_HPP

#include "mlc.hpp"
#include <variant>

namespace cpp_ast {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CppTypeName;struct CppTypeTemplate;struct CppTypeRef;struct CppTypeRRef;struct CppTypePtr;struct CppTypeConst;using CppType = std::variant<CppTypeName, CppTypeTemplate, CppTypeRef, CppTypeRRef, CppTypePtr, CppTypeConst>;struct CppTypeName {mlc::String field0;};struct CppTypeTemplate {mlc::String field0;mlc::Array<std::shared_ptr<cpp_ast::CppType>> field1;};struct CppTypeRef {std::shared_ptr<cpp_ast::CppType> field0;};struct CppTypeRRef {std::shared_ptr<cpp_ast::CppType> field0;};struct CppTypePtr {std::shared_ptr<cpp_ast::CppType> field0;};struct CppTypeConst {std::shared_ptr<cpp_ast::CppType> field0;};

struct CppCastStatic {};struct CppCastDynamic {};struct CppCastReinterpret {};struct CppCastConst {};using CppCastKind = std::variant<CppCastStatic, CppCastDynamic, CppCastReinterpret, CppCastConst>;

struct CppCapture {mlc::String name;bool by_reference;};

struct CppParam {mlc::String name;std::shared_ptr<cpp_ast::CppType> parameter_type;};

struct CppAutoDecl {mlc::String field0;std::shared_ptr<cpp_ast::CppExpression> field1;};
struct CppVarDecl {mlc::String field0;std::shared_ptr<cpp_ast::CppType> field1;std::shared_ptr<cpp_ast::CppExpression> field2;};
struct CppConstDecl {mlc::String field0;std::shared_ptr<cpp_ast::CppType> field1;std::shared_ptr<cpp_ast::CppExpression> field2;};
struct CppConstexprAutoDecl {mlc::String field0;std::shared_ptr<cpp_ast::CppExpression> field1;};
struct CppReturn {std::shared_ptr<cpp_ast::CppExpression> field0;};
struct CppExpressionStatement {std::shared_ptr<cpp_ast::CppExpression> field0;};
struct CppBlock {mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> field0;};
struct CppIf {std::shared_ptr<cpp_ast::CppExpression> field0;std::shared_ptr<cpp_ast::CppStatement> field1;std::shared_ptr<cpp_ast::CppStatement> field2;};
struct CppWhile {std::shared_ptr<cpp_ast::CppExpression> field0;std::shared_ptr<cpp_ast::CppStatement> field1;};
struct CppFor {mlc::String field0;std::shared_ptr<cpp_ast::CppExpression> field1;mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> field2;};
struct CppStructuredBinding {mlc::Array<mlc::String> field0;std::shared_ptr<cpp_ast::CppExpression> field1;};
struct CppStatementFragment {mlc::String field0;};
struct CppStatement {std::variant<CppAutoDecl, CppVarDecl, CppConstDecl, CppConstexprAutoDecl, CppReturn, CppExpressionStatement, CppBlock, CppIf, CppWhile, CppFor, CppStructuredBinding, CppStatementFragment> _;};


struct CppInt {int field0;};
struct CppStr {mlc::String field0;};
struct CppBool {bool field0;};
struct CppIdent {mlc::String field0;};
struct CppCall {std::shared_ptr<cpp_ast::CppExpression> field0;mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> field1;};
struct CppMember {std::shared_ptr<cpp_ast::CppExpression> field0;mlc::String field1;bool field2;};
struct CppIndex {std::shared_ptr<cpp_ast::CppExpression> field0;std::shared_ptr<cpp_ast::CppExpression> field1;};
struct CppBinary {mlc::String field0;std::shared_ptr<cpp_ast::CppExpression> field1;std::shared_ptr<cpp_ast::CppExpression> field2;};
struct CppUnary {mlc::String field0;std::shared_ptr<cpp_ast::CppExpression> field1;};
struct CppTernary {std::shared_ptr<cpp_ast::CppExpression> field0;std::shared_ptr<cpp_ast::CppExpression> field1;std::shared_ptr<cpp_ast::CppExpression> field2;};
struct CppLambda {mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> field0;mlc::Array<std::shared_ptr<cpp_ast::CppParam>> field1;std::shared_ptr<cpp_ast::CppType> field2;mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> field3;};
struct CppMutableLambda {mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> field0;mlc::Array<std::shared_ptr<cpp_ast::CppParam>> field1;std::shared_ptr<cpp_ast::CppExpression> field2;};
struct CppInitList {mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> field0;};
struct CppAggregateInit {mlc::String field0;mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> field1;};
struct CppStdVisit {std::shared_ptr<cpp_ast::CppExpression> field0;mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> field1;};
struct CppVisitArmWild {bool field0;std::shared_ptr<cpp_ast::CppExpression> field1;};
struct CppVisitArmBinding {mlc::String field0;bool field1;std::shared_ptr<cpp_ast::CppExpression> field2;};
struct CppVisitArmConstructed {mlc::String field0;mlc::String field1;bool field2;std::shared_ptr<cpp_ast::CppExpression> field3;};
struct CppVisitArmConstructedGeneric {mlc::String field0;mlc::String field1;mlc::String field2;bool field3;std::shared_ptr<cpp_ast::CppExpression> field4;};
struct CppCast {cpp_ast::CppCastKind field0;std::shared_ptr<cpp_ast::CppType> field1;std::shared_ptr<cpp_ast::CppExpression> field2;};
struct CppInvokedWhile {std::shared_ptr<cpp_ast::CppExpression> field0;mlc::String field1;};
struct CppInvokedFor {mlc::String field0;std::shared_ptr<cpp_ast::CppExpression> field1;mlc::String field2;};
struct CppInvokedBlock {mlc::String field0;};
struct CppInvokedBlockWithReturn {mlc::String field0;mlc::String field1;};
struct CppQuestionTry {std::shared_ptr<cpp_ast::CppExpression> field0;};
struct CppWithBlock {std::shared_ptr<cpp_ast::CppExpression> field0;mlc::String field1;mlc::String field2;};
struct CppExpression {std::variant<CppInt, CppStr, CppBool, CppIdent, CppCall, CppMember, CppIndex, CppBinary, CppUnary, CppTernary, CppLambda, CppMutableLambda, CppInitList, CppAggregateInit, CppStdVisit, CppVisitArmWild, CppVisitArmBinding, CppVisitArmConstructed, CppVisitArmConstructedGeneric, CppCast, CppInvokedWhile, CppInvokedFor, CppInvokedBlock, CppInvokedBlockWithReturn, CppQuestionTry, CppWithBlock> _;};


struct CppField {mlc::String type_value;mlc::String name;};

using CppExpressions = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>;

using CppStatements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>;

using CppParameters = mlc::Array<std::shared_ptr<cpp_ast::CppParam>>;

using CppFields = mlc::Array<std::shared_ptr<cpp_ast::CppField>>;

struct CppVariantArm {mlc::String name;mlc::Array<std::shared_ptr<cpp_ast::CppType>> types;};

struct CppInclude;struct CppIfndef;struct CppDefineMacro;struct CppEndif;struct CppUndef;struct CppNamespaceBegin;struct CppNamespaceEnd;struct CppUsing;struct CppUsingNamespace;struct CppStruct;struct CppForwardDecl;struct CppFnProto;struct CppFnDef;struct CppNamespace;struct CppVariant;struct CppConceptRequires;struct CppStaticAssert;struct CppStdHashSpecialization;struct CppDeclarationSequence;struct CppDeclarationEmpty;struct CppBlankLine;struct CppHostEntryMain;struct CppDeclarationFragment;using CppDeclaration = std::variant<CppInclude, CppIfndef, CppDefineMacro, CppEndif, CppUndef, CppNamespaceBegin, CppNamespaceEnd, CppUsing, CppUsingNamespace, CppStruct, CppForwardDecl, CppFnProto, CppFnDef, CppNamespace, CppVariant, CppConceptRequires, CppStaticAssert, CppStdHashSpecialization, CppDeclarationSequence, CppDeclarationEmpty, CppBlankLine, CppHostEntryMain, CppDeclarationFragment>;struct CppInclude {bool field0;mlc::String field1;};struct CppIfndef {mlc::String field0;};struct CppDefineMacro {mlc::String field0;mlc::String field1;};struct CppEndif {mlc::String field0;};struct CppUndef {mlc::String field0;};struct CppNamespaceBegin {mlc::String field0;};struct CppNamespaceEnd {mlc::String field0;};struct CppUsing {mlc::String field0;mlc::String field1;};struct CppUsingNamespace {mlc::String field0;};struct CppStruct {mlc::String field0;mlc::String field1;cpp_ast::CppFields field2;bool field3;};struct CppForwardDecl {mlc::String field0;mlc::String field1;};struct CppFnProto {mlc::String field0;mlc::String field1;mlc::String field2;mlc::Array<mlc::String> field3;};struct CppFnDef {mlc::String field0;mlc::String field1;mlc::String field2;mlc::Array<mlc::String> field3;cpp_ast::CppStatements field4;int field5;};struct CppNamespace {mlc::String field0;mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> field1;};struct CppVariant {mlc::String field0;mlc::String field1;mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> field2;};struct CppConceptRequires {mlc::String field0;mlc::String field1;mlc::String field2;mlc::Array<mlc::String> field3;};struct CppStaticAssert {mlc::String field0;mlc::Array<mlc::String> field1;mlc::String field2;};struct CppStdHashSpecialization {mlc::String field0;cpp_ast::CppStatements field1;};struct CppDeclarationSequence {mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> field0;};struct CppDeclarationEmpty {};struct CppBlankLine {};struct CppHostEntryMain {mlc::String field0;};struct CppDeclarationFragment {mlc::String field0;};

struct CppFile {mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> header;mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> source;};

struct CppProgram {mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations;};

mlc::String cpp_capture_name(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept;

bool cpp_capture_by_reference(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept;

mlc::String cpp_param_name(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept;

std::shared_ptr<cpp_ast::CppType> cpp_param_type(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept;

mlc::String cpp_field_type(std::shared_ptr<cpp_ast::CppField> field) noexcept;

mlc::String cpp_field_name(std::shared_ptr<cpp_ast::CppField> field) noexcept;

mlc::String cpp_variant_arm_name(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppType>> cpp_variant_arm_types(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_file_header(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_file_source(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String print_integer_literal(int integer_value) noexcept;

mlc::String print_string_literal(mlc::String string_value) noexcept;

mlc::String print_boolean_literal(bool boolean_value) noexcept;

mlc::String print_identifier(mlc::String name) noexcept;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

} // namespace cpp_ast

#endif // CPP_AST_HPP
