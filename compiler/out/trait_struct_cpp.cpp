#define main mlc_user_main
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

std::shared_ptr<cpp_ast::CppField> trait_method_field_cpp(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept{
  return std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{expr::cpp_std_function_type(type_gen::sem_type_to_cpp(context, return_type), decl_extend::non_self_parameter_types_cpp(context, parameters).join(mlc::String(", ", 2))), cpp_naming::cpp_safe(method_name)});
}
mlc::String trait_struct_template_prefix(mlc::Array<mlc::String> type_parameters) noexcept{
  if ((type_parameters.length() > 0))   {
    return expr::cpp_template_typename_header_line(type_parameters.join(mlc::String(", ", 2)));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_field_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppField>> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, __5, __6, __7, __8] = semanticDeclarationFn; return [&]() {
auto method_fields = mlc::Array<std::shared_ptr<cpp_ast::CppField>>{};
method_fields.push_back(trait_method_field_cpp(trait_context, decl_extend::extract_method_name(mangled, trait_name), parameters, return_type));
return method_fields;
}();
}
return [&]() {
auto empty_method_fields = mlc::Array<std::shared_ptr<cpp_ast::CppField>>{};
return empty_method_fields;
}();
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppField>> trait_struct_fields_cpp(context::CodegenContext trait_context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  return methods.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return trait_struct_field_for_method(trait_context, trait_name, method); });
}
std::shared_ptr<cpp_ast::CppDeclaration> gen_trait_struct_cpp(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{trait_struct_template_prefix(type_parameters), expr::trait_vtable_struct_cpp_name(cpp_naming::cpp_safe(trait_name)), trait_struct_fields_cpp(context::CodegenContext_for_type_body(context, trait_name), trait_name, methods), false});
}

} // namespace trait_struct_cpp
