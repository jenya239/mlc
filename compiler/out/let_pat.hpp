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
GenLetPatternResult ctor_pattern_gen_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String temp_name, mlc::String value_code, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept;
GenLetPatternResult gen_let_pattern_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> eval_fn) noexcept;

} // namespace let_pat

#endif // LET_PAT_HPP
