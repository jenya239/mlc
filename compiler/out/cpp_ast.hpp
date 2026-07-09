#ifndef CPP_AST_HPP
#define CPP_AST_HPP

#include "mlc.hpp"
#include <variant>
namespace cpp_ast {

struct CppTypeName;
struct CppTypeTemplate;
struct CppTypeRef;
struct CppTypeRRef;
struct CppTypePtr;
struct CppTypeConst;
using CppType = std::variant<CppTypeName, CppTypeTemplate, CppTypeRef, CppTypeRRef, CppTypePtr, CppTypeConst>;
struct CppCastStatic;
struct CppCastDynamic;
struct CppCastReinterpret;
struct CppCastConst;
using CppCastKind = std::variant<CppCastStatic, CppCastDynamic, CppCastReinterpret, CppCastConst>;
struct CppAutoDecl;
struct CppVarDecl;
struct CppConstDecl;
struct CppConstexprAutoDecl;
struct CppReturn;
struct CppExpressionStatement;
struct CppBlock;
struct CppIf;
struct CppWhile;
struct CppFor;
struct CppStructuredBinding;
struct CppStatementFragment;
using CppStatement = std::variant<CppAutoDecl, CppVarDecl, CppConstDecl, CppConstexprAutoDecl, CppReturn, CppExpressionStatement, CppBlock, CppIf, CppWhile, CppFor, CppStructuredBinding, CppStatementFragment>;
struct CppInt;
struct CppStr;
struct CppCharLiteral;
struct CppFloatLiteral;
struct CppBool;
struct CppIdent;
struct CppCall;
struct CppMember;
struct CppIndex;
struct CppBinary;
struct CppUnary;
struct CppTernary;
struct CppLambda;
struct CppMutableLambda;
struct CppInitList;
struct CppAggregateInit;
struct CppStdVisit;
struct CppVisitArmWild;
struct CppVisitArmBinding;
struct CppVisitArmConstructed;
struct CppVisitArmConstructedGeneric;
struct CppCast;
struct CppInvokedWhile;
struct CppInvokedFor;
struct CppInvokedBlock;
struct CppInvokedBlockWithReturn;
struct CppQuestionTry;
struct CppWithBlock;
using CppExpression = std::variant<CppInt, CppStr, CppCharLiteral, CppFloatLiteral, CppBool, CppIdent, CppCall, CppMember, CppIndex, CppBinary, CppUnary, CppTernary, CppLambda, CppMutableLambda, CppInitList, CppAggregateInit, CppStdVisit, CppVisitArmWild, CppVisitArmBinding, CppVisitArmConstructed, CppVisitArmConstructedGeneric, CppCast, CppInvokedWhile, CppInvokedFor, CppInvokedBlock, CppInvokedBlockWithReturn, CppQuestionTry, CppWithBlock>;
struct CppPublic;
struct CppProtected;
struct CppPrivate;
using CppAccessLevel = std::variant<CppPublic, CppProtected, CppPrivate>;
struct CppClassMemberField;
struct CppClassMemberFunction;
struct CppClassMemberAccess;
struct CppClassMemberTypeAlias;
using CppClassMember = std::variant<CppClassMemberField, CppClassMemberFunction, CppClassMemberAccess, CppClassMemberTypeAlias>;
struct CppInclude;
struct CppIfndef;
struct CppDefineMacro;
struct CppEndif;
struct CppUndef;
struct CppNamespaceBegin;
struct CppNamespaceEnd;
struct CppUsing;
struct CppUsingNamespace;
struct CppStruct;
struct CppForwardDecl;
struct CppFnProto;
struct CppFnDef;
struct CppNamespace;
struct CppVariant;
struct CppConceptRequires;
struct CppStaticAssert;
struct CppStdHashSpecialization;
struct CppDeclarationSequence;
struct CppDeclarationEmpty;
struct CppBlankLine;
struct CppHostEntryMain;
struct CppClassDeclaration;
struct CppTypedefDeclaration;
struct CppTemplateDeclaration;
struct CppFunctionPrototypeDecl;
struct CppExternBlock;
struct CppDeclarationFragment;
using CppDeclaration = std::variant<CppInclude, CppIfndef, CppDefineMacro, CppEndif, CppUndef, CppNamespaceBegin, CppNamespaceEnd, CppUsing, CppUsingNamespace, CppStruct, CppForwardDecl, CppFnProto, CppFnDef, CppNamespace, CppVariant, CppConceptRequires, CppStaticAssert, CppStdHashSpecialization, CppDeclarationSequence, CppDeclarationEmpty, CppBlankLine, CppHostEntryMain, CppClassDeclaration, CppTypedefDeclaration, CppTemplateDeclaration, CppFunctionPrototypeDecl, CppExternBlock, CppDeclarationFragment>;
struct CppTypeName {
  mlc::String field0;
};
struct CppTypeTemplate {
  mlc::String field0;
  mlc::Array<std::shared_ptr<CppType>> field1;
};
struct CppTypeRef {
  std::shared_ptr<CppType> field0;
};
struct CppTypeRRef {
  std::shared_ptr<CppType> field0;
};
struct CppTypePtr {
  std::shared_ptr<CppType> field0;
};
struct CppTypeConst {
  std::shared_ptr<CppType> field0;
};
struct CppCastStatic {};
struct CppCastDynamic {};
struct CppCastReinterpret {};
struct CppCastConst {};
struct CppCapture {
  mlc::String name;
  bool by_reference;
};
struct CppParam {
  mlc::String name;
  std::shared_ptr<CppType> parameter_type;
};
struct CppAutoDecl {
  mlc::String field0;
  std::shared_ptr<CppExpression> field1;
};
struct CppVarDecl {
  mlc::String field0;
  std::shared_ptr<CppType> field1;
  std::shared_ptr<CppExpression> field2;
};
struct CppConstDecl {
  mlc::String field0;
  std::shared_ptr<CppType> field1;
  std::shared_ptr<CppExpression> field2;
};
struct CppConstexprAutoDecl {
  mlc::String field0;
  std::shared_ptr<CppExpression> field1;
};
struct CppReturn {
  std::shared_ptr<CppExpression> field0;
};
struct CppExpressionStatement {
  std::shared_ptr<CppExpression> field0;
};
struct CppBlock {
  mlc::Array<std::shared_ptr<CppStatement>> field0;
};
struct CppIf {
  std::shared_ptr<CppExpression> field0;
  std::shared_ptr<CppStatement> field1;
  std::shared_ptr<CppStatement> field2;
};
struct CppWhile {
  std::shared_ptr<CppExpression> field0;
  std::shared_ptr<CppStatement> field1;
};
struct CppFor {
  mlc::String field0;
  std::shared_ptr<CppExpression> field1;
  mlc::Array<std::shared_ptr<CppStatement>> field2;
};
struct CppStructuredBinding {
  mlc::Array<mlc::String> field0;
  std::shared_ptr<CppExpression> field1;
};
struct CppStatementFragment {
  mlc::String field0;
};
struct CppInt {
  int field0;
};
struct CppStr {
  mlc::String field0;
};
struct CppCharLiteral {
  mlc::String field0;
};
struct CppFloatLiteral {
  mlc::String field0;
};
struct CppBool {
  bool field0;
};
struct CppIdent {
  mlc::String field0;
};
struct CppCall {
  std::shared_ptr<CppExpression> field0;
  mlc::Array<std::shared_ptr<CppExpression>> field1;
};
struct CppMember {
  std::shared_ptr<CppExpression> field0;
  mlc::String field1;
  bool field2;
};
struct CppIndex {
  std::shared_ptr<CppExpression> field0;
  std::shared_ptr<CppExpression> field1;
};
struct CppBinary {
  mlc::String field0;
  std::shared_ptr<CppExpression> field1;
  std::shared_ptr<CppExpression> field2;
};
struct CppUnary {
  mlc::String field0;
  std::shared_ptr<CppExpression> field1;
};
struct CppTernary {
  std::shared_ptr<CppExpression> field0;
  std::shared_ptr<CppExpression> field1;
  std::shared_ptr<CppExpression> field2;
};
struct CppLambda {
  mlc::Array<std::shared_ptr<CppCapture>> field0;
  mlc::Array<std::shared_ptr<CppParam>> field1;
  std::shared_ptr<CppType> field2;
  mlc::Array<std::shared_ptr<CppStatement>> field3;
};
struct CppMutableLambda {
  mlc::Array<std::shared_ptr<CppCapture>> field0;
  mlc::Array<std::shared_ptr<CppParam>> field1;
  std::shared_ptr<CppExpression> field2;
};
struct CppInitList {
  mlc::Array<std::shared_ptr<CppExpression>> field0;
};
struct CppAggregateInit {
  mlc::String field0;
  mlc::Array<std::shared_ptr<CppExpression>> field1;
};
struct CppStdVisit {
  std::shared_ptr<CppExpression> field0;
  mlc::Array<std::shared_ptr<CppExpression>> field1;
};
struct CppVisitArmWild {
  bool field0;
  mlc::String field1;
  std::shared_ptr<CppExpression> field2;
};
struct CppVisitArmBinding {
  mlc::String field0;
  bool field1;
  mlc::String field2;
  std::shared_ptr<CppExpression> field3;
};
struct CppVisitArmConstructed {
  mlc::String field0;
  mlc::String field1;
  bool field2;
  mlc::String field3;
  std::shared_ptr<CppExpression> field4;
};
struct CppVisitArmConstructedGeneric {
  mlc::String field0;
  mlc::String field1;
  mlc::String field2;
  bool field3;
  mlc::String field4;
  std::shared_ptr<CppExpression> field5;
};
struct CppCast {
  CppCastKind field0;
  std::shared_ptr<CppType> field1;
  std::shared_ptr<CppExpression> field2;
};
struct CppInvokedWhile {
  std::shared_ptr<CppExpression> field0;
  mlc::String field1;
};
struct CppInvokedFor {
  mlc::String field0;
  std::shared_ptr<CppExpression> field1;
  mlc::String field2;
};
struct CppInvokedBlock {
  mlc::String field0;
};
struct CppInvokedBlockWithReturn {
  mlc::String field0;
  mlc::String field1;
};
struct CppQuestionTry {
  std::shared_ptr<CppExpression> field0;
};
struct CppWithBlock {
  std::shared_ptr<CppExpression> field0;
  mlc::String field1;
  mlc::String field2;
};
struct CppField {
  mlc::String type_value;
  mlc::String name;
};
struct CppVariantArm {
  mlc::String name;
  mlc::Array<std::shared_ptr<CppType>> types;
};
struct CppFnModifiers {
  bool is_const;
  bool is_virtual;
  bool is_override;
  bool is_static;
  bool is_explicit;
  bool is_noexcept;
  bool is_deleted;
  bool is_defaulted;
  bool is_pure_virtual;
};
struct CppPublic {};
struct CppProtected {};
struct CppPrivate {};
struct CppBaseClass {
  CppAccessLevel access;
  mlc::String name;
};
struct CppFunctionPrototype {
  mlc::String template_prefix;
  std::shared_ptr<CppType> return_type;
  mlc::String name;
  mlc::Array<std::shared_ptr<CppParam>> parameters;
  CppFnModifiers modifiers;
};
struct CppClassMemberField {
  std::shared_ptr<CppType> field0;
  mlc::String field1;
  mlc::String field2;
};
struct CppClassMemberFunction {
  std::shared_ptr<CppFunctionPrototype> field0;
};
struct CppClassMemberAccess {
  CppAccessLevel field0;
};
struct CppClassMemberTypeAlias {
  mlc::String field0;
  std::shared_ptr<CppType> field1;
};
struct CppClassDefinition {
  bool is_struct;
  mlc::String template_prefix;
  mlc::String name;
  mlc::Array<CppBaseClass> base_classes;
  mlc::Array<CppClassMember> members;
};
struct CppInclude {
  bool field0;
  mlc::String field1;
};
struct CppIfndef {
  mlc::String field0;
};
struct CppDefineMacro {
  mlc::String field0;
  mlc::String field1;
};
struct CppEndif {
  mlc::String field0;
};
struct CppUndef {
  mlc::String field0;
};
struct CppNamespaceBegin {
  mlc::String field0;
};
struct CppNamespaceEnd {
  mlc::String field0;
};
struct CppUsing {
  mlc::String field0;
  mlc::String field1;
};
struct CppUsingNamespace {
  mlc::String field0;
};
struct CppStruct {
  mlc::String field0;
  mlc::String field1;
  mlc::Array<std::shared_ptr<CppField>> field2;
  bool field3;
};
struct CppForwardDecl {
  mlc::String field0;
  mlc::String field1;
};
struct CppFnProto {
  mlc::String field0;
  mlc::String field1;
  mlc::String field2;
  mlc::Array<mlc::String> field3;
};
struct CppFnDef {
  mlc::String field0;
  mlc::String field1;
  mlc::String field2;
  mlc::Array<mlc::String> field3;
  mlc::Array<std::shared_ptr<CppStatement>> field4;
  int field5;
};
struct CppNamespace {
  mlc::String field0;
  mlc::Array<std::shared_ptr<CppDeclaration>> field1;
};
struct CppVariant {
  mlc::String field0;
  mlc::String field1;
  mlc::Array<std::shared_ptr<CppVariantArm>> field2;
};
struct CppConceptRequires {
  mlc::String field0;
  mlc::String field1;
  mlc::String field2;
  mlc::Array<mlc::String> field3;
};
struct CppStaticAssert {
  mlc::String field0;
  mlc::Array<mlc::String> field1;
  mlc::String field2;
};
struct CppStdHashSpecialization {
  mlc::String field0;
  mlc::Array<std::shared_ptr<CppStatement>> field1;
};
struct CppDeclarationSequence {
  mlc::Array<std::shared_ptr<CppDeclaration>> field0;
};
struct CppDeclarationEmpty {};
struct CppBlankLine {};
struct CppHostEntryMain {
  mlc::String field0;
};
struct CppClassDeclaration {
  CppClassDefinition field0;
};
struct CppTypedefDeclaration {
  mlc::String field0;
  std::shared_ptr<CppType> field1;
};
struct CppTemplateDeclaration {
  mlc::String field0;
  std::shared_ptr<CppDeclaration> field1;
};
struct CppFunctionPrototypeDecl {
  CppFunctionPrototype field0;
};
struct CppExternBlock {
  mlc::String field0;
  mlc::Array<std::shared_ptr<CppDeclaration>> field1;
};
struct CppDeclarationFragment {
  mlc::String field0;
};
struct CppFile {
  mlc::Array<std::shared_ptr<CppDeclaration>> header;
  mlc::Array<std::shared_ptr<CppDeclaration>> source;
};
struct CppProgram {
  mlc::Array<std::shared_ptr<CppDeclaration>> declarations;
};
CppFnModifiers cpp_fn_modifiers_none() noexcept;
mlc::String cpp_capture_name(std::shared_ptr<CppCapture> capture) noexcept;
bool cpp_capture_by_reference(std::shared_ptr<CppCapture> capture) noexcept;
mlc::String cpp_param_name(std::shared_ptr<CppParam> parameter) noexcept;
std::shared_ptr<CppType> cpp_param_type(std::shared_ptr<CppParam> parameter) noexcept;
mlc::String cpp_field_type(std::shared_ptr<CppField> field) noexcept;
mlc::String cpp_field_name(std::shared_ptr<CppField> field) noexcept;
mlc::String cpp_variant_arm_name(std::shared_ptr<CppVariantArm> arm) noexcept;
mlc::Array<std::shared_ptr<CppType>> cpp_variant_arm_types(std::shared_ptr<CppVariantArm> arm) noexcept;
mlc::Array<std::shared_ptr<CppDeclaration>> cpp_file_header(std::shared_ptr<CppFile> file) noexcept;
mlc::Array<std::shared_ptr<CppDeclaration>> cpp_file_source(std::shared_ptr<CppFile> file) noexcept;

} // namespace cpp_ast

#endif // CPP_AST_HPP
