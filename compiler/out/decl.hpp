#ifndef DECL_HPP
#define DECL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "return_body.hpp"
#include "expr.hpp"
#include "decl_extend.hpp"
#include "decl_index.hpp"

namespace decl {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct DeclPartsBundle {mlc::Array<mlc::String> type_fwds;mlc::Array<mlc::String> type_defs;mlc::Array<mlc::String> fn_protos;mlc::Array<mlc::String> fn_defs;};

context::CodegenContext prototype_context_for_self_parameter(context::CodegenContext context, std::shared_ptr<ast::Param> first_parameter) noexcept;

context::CodegenContext prototype_context_for_function(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> parameters, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;

mlc::String gen_decl(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;

mlc::String extend_cpp_forward_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept;

mlc::Array<mlc::String> exported_type_names_from_decls(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;

mlc::Array<mlc::String> extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

decl::DeclPartsBundle collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;

} // namespace decl

#endif // DECL_HPP
