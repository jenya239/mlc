#ifndef LET_PAT_HPP
#define LET_PAT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"
namespace let_pat {

struct GenLetPatternResult {
  mlc::String output;
  context::CodegenContext codegen_context;
};
mlc::String gen_let_array_rest_slice(mlc::String temporary_name, int from_index, mlc::String element_cpp) noexcept;
mlc::String primitive_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code) noexcept;
mlc::String record_codegen_name_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;
bool value_type_is_tuple_or_pair(std::shared_ptr<registry::Type> value_type) noexcept;
mlc::String append_tuple_member_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String member_label, mlc::String temp_name) noexcept;
mlc::String record_ordered_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String record_codegen_name, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;
mlc::String tuple_pattern_output_for_value_type(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;
mlc::String record_or_fallback_tuple_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;
mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept;
mlc::String append_array_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> sub_pattern, mlc::String temp_name, int index) noexcept;
mlc::String append_record_field_ident_binding(mlc::String fragment, std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept;
mlc::String record_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, mlc::String value_code) noexcept;
mlc::String array_pattern_output(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String rest, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, mlc::String value_code, context::CodegenContext codegen_context) noexcept;
mlc::String wild_pattern_output(mlc::String value_code) noexcept;
template<typename __F7>
GenLetPatternResult ctor_pattern_gen_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, __F7 eval_fn) noexcept;
GenLetPatternResult gen_let_pattern_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept;
template<typename __F7>
GenLetPatternResult ctor_pattern_gen_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, __F7 eval_fn) noexcept{
  auto qualified_name = context::CodegenContext_resolve(codegen_context, constructor_name);
  auto second_temp = context::take_temp_name(codegen_context);
  auto variant_binding_name = second_temp.name;
  auto updated_codegen_context = second_temp.codegen_context;
  auto bind = [&]() -> mlc::String {
  if ((sub_patterns.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return expr::tuple_destructure_binding(match_analysis::pattern_bind_names(sub_patterns), variant_binding_name);
  }
}();
  auto is_generic = decl_index::list_contains(updated_codegen_context.generic_variants, constructor_name);
  auto type_arguments = (is_generic ? (mlc::String("<auto>", 6)) : (mlc::String("", 0)));
  auto else_code = (has_else ? (eval_fn(else_body, updated_codegen_context)) : (mlc::String("std::abort();", 13)));
  return GenLetPatternResult{(((((((((((((((((((((((((mlc::String("auto ", 5) + temp_name) + mlc::String(" = ", 3)) + value_code) + mlc::String(";\n", 2)) + mlc::String("if (!std::holds_alternative<", 28)) + qualified_name) + type_arguments) + mlc::String(">(", 2)) + temp_name) + mlc::String(")) { ", 5)) + else_code) + mlc::String(" }\n", 3)) + mlc::String("const ", 6)) + qualified_name) + type_arguments) + mlc::String("& ", 2)) + variant_binding_name) + mlc::String(" = std::get<", 12)) + qualified_name) + type_arguments) + mlc::String(">(", 2)) + temp_name) + mlc::String(");\n", 3)) + bind) + mlc::String("\n", 1)), updated_codegen_context};
}

} // namespace let_pat

#endif // LET_PAT_HPP
