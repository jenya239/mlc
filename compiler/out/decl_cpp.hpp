#ifndef DECL_CPP_HPP
#define DECL_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "decl_extend.hpp"
#include "decl_index.hpp"
#include "expr.hpp"
#include "type_gen.hpp"
#include "derive_methods_cpp.hpp"
#include "trait_struct_cpp.hpp"
#include "return_body.hpp"
#include "cpp_naming.hpp"

namespace decl_cpp {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct DeclPartsBundleCpp {mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> type_fwds;mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> type_defs;mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> fn_protos;mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> fn_defs;};

std::shared_ptr<cpp_ast::CppDeclaration> cpp_decl_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept;

mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

mlc::Array<mlc::String> main_program_parameter_items_cpp() noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_fwd_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_type_decl_body_cpp(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_decl_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> main_set_args_preamble_cpp() noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> gen_decl_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> gen_proto_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_method_forward_segments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_helper_protos_for_exported_types_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_trait_suffix_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_extend_extern_method_cpp(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_methods_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_decl_extend_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_extend_wrapper_protos_cpp(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> extend_cpp_forward_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> decl_segment_cpp(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context, int phase) noexcept;

decl_cpp::DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> collect_fn_defs_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

} // namespace decl_cpp

#endif // DECL_CPP_HPP
