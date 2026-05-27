#ifndef DECL_CPP_HPP
#define DECL_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "decl_extend.hpp"
#include "type_gen.hpp"
#include "return_body.hpp"

namespace decl_cpp {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

struct DeclPartsBundleCpp {mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> type_fwds;mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> type_defs;mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> fn_protos;mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> fn_defs;};

std::shared_ptr<cpp_ast::CppDecl> cpp_decl_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept;

mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept;

mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

mlc::String gen_fn_proto_cpp_as_string(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl_cpp_as_string(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_decl_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept;

mlc::String gen_decl_cpp_as_string(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_proto_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept;

mlc::String gen_proto_cpp_as_string(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept;

decl_cpp::DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context, int phase) noexcept;

} // namespace decl_cpp

#endif // DECL_CPP_HPP
