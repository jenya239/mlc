#ifndef CPP_AST_HPP
#define CPP_AST_HPP

#include "mlc.hpp"
#include <variant>

namespace cpp_ast {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

struct CppTypeName;struct CppTypeTemplate;struct CppTypeRef;using CppType = std::variant<CppTypeName, CppTypeTemplate, CppTypeRef>;struct CppTypeName {mlc::String field0;};struct CppTypeTemplate {mlc::String field0;mlc::Array<std::shared_ptr<cpp_ast::CppType>> field1;};struct CppTypeRef {std::shared_ptr<cpp_ast::CppType> field0;};

struct CppCastStatic {};struct CppCastDynamic {};struct CppCastReinterpret {};struct CppCastConst {};using CppCastKind = std::variant<CppCastStatic, CppCastDynamic, CppCastReinterpret, CppCastConst>;

struct CppCapture {mlc::String name;bool by_reference;};

struct CppParam {mlc::String name;std::shared_ptr<cpp_ast::CppType> parameter_type;};

struct CppAutoDecl {mlc::String field0;std::shared_ptr<cpp_ast::CppExpr> field1;};
struct CppConstDecl {mlc::String field0;std::shared_ptr<cpp_ast::CppType> field1;std::shared_ptr<cpp_ast::CppExpr> field2;};
struct CppReturn {std::shared_ptr<cpp_ast::CppExpr> field0;};
struct CppExprStmt {std::shared_ptr<cpp_ast::CppExpr> field0;};
struct CppBlock {mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> field0;};
struct CppIf {std::shared_ptr<cpp_ast::CppExpr> field0;std::shared_ptr<cpp_ast::CppStmt> field1;std::shared_ptr<cpp_ast::CppStmt> field2;};
struct CppWhile {std::shared_ptr<cpp_ast::CppExpr> field0;std::shared_ptr<cpp_ast::CppStmt> field1;};
struct CppFor {mlc::String field0;std::shared_ptr<cpp_ast::CppExpr> field1;mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> field2;};
struct CppStmt {std::variant<CppAutoDecl, CppConstDecl, CppReturn, CppExprStmt, CppBlock, CppIf, CppWhile, CppFor> _;};


struct CppInt {int field0;};
struct CppStr {mlc::String field0;};
struct CppBool {bool field0;};
struct CppIdent {mlc::String field0;};
struct CppCall {std::shared_ptr<cpp_ast::CppExpr> field0;mlc::Array<std::shared_ptr<cpp_ast::CppExpr>> field1;};
struct CppMember {std::shared_ptr<cpp_ast::CppExpr> field0;mlc::String field1;bool field2;};
struct CppIndex {std::shared_ptr<cpp_ast::CppExpr> field0;std::shared_ptr<cpp_ast::CppExpr> field1;};
struct CppBinary {mlc::String field0;std::shared_ptr<cpp_ast::CppExpr> field1;std::shared_ptr<cpp_ast::CppExpr> field2;};
struct CppUnary {mlc::String field0;std::shared_ptr<cpp_ast::CppExpr> field1;};
struct CppTernary {std::shared_ptr<cpp_ast::CppExpr> field0;std::shared_ptr<cpp_ast::CppExpr> field1;std::shared_ptr<cpp_ast::CppExpr> field2;};
struct CppLambda {mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> field0;mlc::Array<std::shared_ptr<cpp_ast::CppParam>> field1;std::shared_ptr<cpp_ast::CppType> field2;mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> field3;};
struct CppInitList {mlc::Array<std::shared_ptr<cpp_ast::CppExpr>> field0;};
struct CppStdVisit {std::shared_ptr<cpp_ast::CppExpr> field0;mlc::Array<std::shared_ptr<cpp_ast::CppExpr>> field1;};
struct CppCast {cpp_ast::CppCastKind field0;std::shared_ptr<cpp_ast::CppType> field1;std::shared_ptr<cpp_ast::CppExpr> field2;};
struct CppInvokedWhile {std::shared_ptr<cpp_ast::CppExpr> field0;mlc::String field1;};
struct CppInvokedFor {mlc::String field0;std::shared_ptr<cpp_ast::CppExpr> field1;mlc::String field2;};
struct CppQuestionTry {std::shared_ptr<cpp_ast::CppExpr> field0;};
struct CppWithBlock {std::shared_ptr<cpp_ast::CppExpr> field0;mlc::String field1;mlc::String field2;};
struct CppExpr {std::variant<CppInt, CppStr, CppBool, CppIdent, CppCall, CppMember, CppIndex, CppBinary, CppUnary, CppTernary, CppLambda, CppInitList, CppStdVisit, CppCast, CppInvokedWhile, CppInvokedFor, CppQuestionTry, CppWithBlock> _;};


struct CppField {mlc::String typ;mlc::String name;};

struct CppVariantArm {mlc::String name;mlc::Array<std::shared_ptr<cpp_ast::CppType>> types;};

struct CppInclude;struct CppUsing;struct CppStruct;struct CppFnProto;struct CppFnDef;struct CppNamespace;struct CppVariant;struct CppDeclFragment;using CppDecl = std::variant<CppInclude, CppUsing, CppStruct, CppFnProto, CppFnDef, CppNamespace, CppVariant, CppDeclFragment>;struct CppInclude {bool field0;mlc::String field1;};struct CppUsing {mlc::String field0;mlc::String field1;};struct CppStruct {mlc::String field0;mlc::Array<std::shared_ptr<cpp_ast::CppField>> field1;};struct CppFnProto {mlc::String field0;mlc::String field1;mlc::Array<mlc::String> field2;};struct CppFnDef {mlc::String field0;mlc::String field1;mlc::Array<mlc::String> field2;mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> field3;};struct CppNamespace {mlc::String field0;mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> field1;};struct CppVariant {mlc::String field0;mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> field1;};struct CppDeclFragment {mlc::String field0;};

struct CppFile {mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> header;mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> source;};

struct CppProgram {mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> decls;};

mlc::String cpp_capture_name(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept;

bool cpp_capture_by_reference(std::shared_ptr<cpp_ast::CppCapture> capture) noexcept;

mlc::String cpp_param_name(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept;

std::shared_ptr<cpp_ast::CppType> cpp_param_type(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept;

mlc::String cpp_field_type(std::shared_ptr<cpp_ast::CppField> field) noexcept;

mlc::String cpp_field_name(std::shared_ptr<cpp_ast::CppField> field) noexcept;

mlc::String cpp_variant_arm_name(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppType>> cpp_variant_arm_types(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_file_header(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_file_source(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

} // namespace cpp_ast

#endif // CPP_AST_HPP
