#ifndef MUT_ACTUAL_ARGUMENT_HPP
#define MUT_ACTUAL_ARGUMENT_HPP

#include "mlc.hpp"
#include <variant>
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "type_gen.hpp"
namespace mut_actual_argument {

struct MutActualArgumentMaterializationOutcome {
  mlc::String prelude_block;
  mlc::Array<mlc::String> materialized_argument_fragments;
};
struct MutActualArgumentMaterializationOutcomeCpp {
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements;
  mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> materialized_argument_expressions;
};
bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SemanticExpression> actual_argument) noexcept;
std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_invoked_lambda() noexcept;
mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept;
std::shared_ptr<cpp_ast::CppExpression> wrap_invoke_with_mut_prelude_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements, std::shared_ptr<cpp_ast::CppExpression> invoke_expression) noexcept;
MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;
MutActualArgumentMaterializationOutcomeCpp materialize_positional_actual_arguments_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

} // namespace mut_actual_argument

#endif // MUT_ACTUAL_ARGUMENT_HPP
