#include "decl_cpp.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl.hpp"

namespace decl_cpp {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace cpp_printer;
using namespace emit_helpers;
using namespace context;
using namespace decl;
using namespace ast_tokens;

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

std::shared_ptr<cpp_ast::CppDecl> cpp_decl_from_string_output(mlc::String output) noexcept{return emit_helpers::make_fragment_cpp_declaration(output);}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept{return parts.map([](mlc::String part) mutable { return cpp_decl_from_string_output(part); });}

mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept{return cpp_printer::print_decl(declaration);}

mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations) noexcept{return declarations.map([](std::shared_ptr<cpp_ast::CppDecl> declaration) mutable { return print_cpp_declaration(declaration); }).join(mlc::String(""));}

std::shared_ptr<cpp_ast::CppDecl> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{return cpp_decl_from_string_output(decl::gen_fn_proto(name, type_params, type_bounds, params, return_type, context));}

mlc::String gen_fn_proto_cpp_as_string(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{return print_cpp_declaration(gen_fn_proto_cpp(name, type_params, type_bounds, params, return_type, context));}

std::shared_ptr<cpp_ast::CppDecl> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept{return cpp_decl_from_string_output(decl::gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context));}

mlc::String gen_fn_decl_cpp_as_string(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept{return print_cpp_declaration(gen_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, context));}

std::shared_ptr<cpp_ast::CppDecl> gen_decl_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept{return cpp_decl_from_string_output(decl::gen_decl(declaration, context));}

mlc::String gen_decl_cpp_as_string(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept{return print_cpp_declaration(gen_decl_cpp(declaration, context));}

std::shared_ptr<cpp_ast::CppDecl> gen_proto_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept{return cpp_decl_from_string_output(decl::gen_proto(declaration, context));}

mlc::String gen_proto_cpp_as_string(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept{return print_cpp_declaration(gen_proto_cpp(declaration, context));}

decl_cpp::DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context) noexcept{
decl::DeclPartsBundle string_bundle = decl::collect_all_decl_parts(declarations, context);
return decl_cpp::DeclPartsBundleCpp{cpp_decls_from_string_parts(string_bundle.type_fwds), cpp_decls_from_string_parts(string_bundle.type_defs), cpp_decls_from_string_parts(string_bundle.fn_protos), cpp_decls_from_string_parts(string_bundle.fn_defs)};
}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context, int phase) noexcept{return cpp_decls_from_string_parts(decl::collect_decl_parts(declarations, context, phase));}

} // namespace decl_cpp
