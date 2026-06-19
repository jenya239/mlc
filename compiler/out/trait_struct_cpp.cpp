#include "trait_struct_cpp.hpp"

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

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace type_gen;
using namespace decl_extend;
using namespace cpp_naming;
using namespace cpp_ast;
using namespace expr;
using namespace ast_tokens;

std::shared_ptr<cpp_ast::CppField> trait_method_field_cpp(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept;

mlc::String trait_struct_template_prefix(mlc::Array<mlc::String> type_parameters) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_field_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_fields_cpp(context::CodegenContext trait_context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_struct_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

std::shared_ptr<cpp_ast::CppField> trait_method_field_cpp(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept{return std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{expr::cpp_std_function_type(type_gen::sem_type_to_cpp(context, return_type), decl_extend::non_self_parameter_types_cpp(context, parameters).join(mlc::String(", "))), cpp_naming::cpp_safe(method_name)});}

mlc::String trait_struct_template_prefix(mlc::Array<mlc::String> type_parameters) noexcept{return type_parameters.size() > 0 ? expr::cpp_template_typename_header_line(type_parameters.join(mlc::String(", "))) : mlc::String("");}

mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_field_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppField>> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [mangled, _w0, _w1, parameters, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppField>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppField>> method_fields = {};
  method_fields.push_back(trait_method_field_cpp(trait_context, decl_extend::extract_method_name(mangled, trait_name), parameters, return_type));
  return method_fields;
 }(); } return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppField>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppField>> empty_method_fields = {};
  return empty_method_fields;
 }(); }();}

mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_fields_cpp(context::CodegenContext trait_context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return mlc::collections::flat_map(methods, [trait_context, trait_name](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return trait_struct_field_for_method(trait_context, trait_name, method); });}

std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_struct_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(trait_struct_template_prefix(type_parameters), expr::trait_vtable_struct_cpp_name(cpp_naming::cpp_safe(trait_name)), trait_struct_fields_cpp(context::CodegenContext_for_type_body(context, trait_name), trait_name, methods), false));}

} // namespace trait_struct_cpp
