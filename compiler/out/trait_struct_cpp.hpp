#ifndef TRAIT_STRUCT_CPP_HPP
#define TRAIT_STRUCT_CPP_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "decl_extend.hpp"
#include "cpp_naming.hpp"
#include "cpp_ast.hpp"
#include "expr.hpp"
namespace trait_struct_cpp {

std::shared_ptr<cpp_ast::CppField> trait_method_field_cpp(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept;
mlc::String trait_struct_template_prefix(mlc::Array<mlc::String> type_parameters) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_field_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_fields_cpp(context::CodegenContext trait_context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_struct_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

} // namespace trait_struct_cpp

#endif // TRAIT_STRUCT_CPP_HPP
