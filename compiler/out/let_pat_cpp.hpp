#ifndef LET_PAT_CPP_HPP
#define LET_PAT_CPP_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "print.hpp"
namespace let_pat_cpp {

struct GenLetPatternCppResult {
  std::shared_ptr<cpp_ast::CppStatement> statement;
  context::CodegenContext codegen_context;
};
mlc::String print_statements_as_fragment(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;
std::shared_ptr<cpp_ast::CppExpression> member_access(mlc::String object_name, mlc::String field_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> index_access(mlc::String object_name, int index) noexcept;
std::shared_ptr<cpp_ast::CppExpression> method_call(mlc::String object_name, mlc::String method_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> variant_holds_alternative_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> variant_get_expression(mlc::String variant_type_name, mlc::String subject_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> array_rest_slice_expression(mlc::String array_name, int from_index, mlc::String element_cpp_type) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> extra) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> constructor_mismatch_else_statements(bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept;
std::shared_ptr<cpp_ast::CppStatement> variant_get_binding_statement(mlc::String variant_type_name, mlc::String binding_name, mlc::String subject_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> tuple_element_access(mlc::String source_name, int element_index) noexcept;
bool pattern_is_identifier(std::shared_ptr<ast::Pattern> pattern) noexcept;
bool tuple_subpatterns_are_simple_identifiers(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> simple_tuple_ident_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept;
std::shared_ptr<cpp_ast::CppStatement> tuple_ident_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String source_name, int index) noexcept;
std::shared_ptr<cpp_ast::CppStatement> tuple_structured_binding_statement(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String source_name) noexcept;
std::shared_ptr<cpp_ast::CppStatement> let_pattern_block(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, std::shared_ptr<cpp_ast::CppExpression> value_expression, mlc::String temp_name) noexcept;
mlc::String record_codegen_name_from_value_type(std::shared_ptr<registry::Type> value_type) noexcept;
bool value_type_is_tuple_or_pair(std::shared_ptr<registry::Type> value_type) noexcept;
mlc::String array_element_cpp_for_value_type(std::shared_ptr<registry::Type> value_type, context::CodegenContext codegen_context) noexcept;
GenLetPatternCppResult tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> record_member_binding_statements(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::Array<mlc::String> ordered_member_labels, mlc::String temp_name) noexcept;
std::shared_ptr<cpp_ast::CppStatement> record_member_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String member_label, mlc::String temp_name) noexcept;
GenLetPatternCppResult record_or_fallback_tuple_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;
GenLetPatternCppResult record_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> field_patterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;
std::shared_ptr<cpp_ast::CppStatement> record_field_binding_statement(std::shared_ptr<ast::Pattern> field_pattern, mlc::String temp_name) noexcept;
GenLetPatternCppResult array_pattern_cpp_result(mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String rest_name, std::shared_ptr<registry::Type> value_type, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, context::CodegenContext codegen_context) noexcept;
std::shared_ptr<cpp_ast::CppStatement> array_element_binding_statement(std::shared_ptr<ast::Pattern> subpattern, mlc::String temp_name, int index) noexcept;
GenLetPatternCppResult ctor_pattern_cpp_result(mlc::String constructor_name, mlc::Array<std::shared_ptr<ast::Pattern>> subpatterns, mlc::String temp_name, std::shared_ptr<cpp_ast::CppExpression> value_expression, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext codegen_context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept;
GenLetPatternCppResult gen_let_pattern_cpp_statement(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, bool has_else, std::shared_ptr<semantic_ir::SemanticExpression> else_body, context::CodegenContext context, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> expression_codegen) noexcept;

} // namespace let_pat_cpp

#endif // LET_PAT_CPP_HPP
