#ifndef MUT_ACTUAL_ARGUMENT_HPP
#define MUT_ACTUAL_ARGUMENT_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "context.hpp"
#include "type_gen.hpp"

namespace mut_actual_argument {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct MutActualArgumentMaterializationOutcome {mlc::String prelude_block;mlc::Array<mlc::String> materialized_argument_fragments;};

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SExpr> actual_argument) noexcept;

mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept;

mut_actual_argument::MutActualArgumentMaterializationOutcome materialize_positional_actual_arguments_maybe_holding_mut_references(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> positional_argument_expressions, mlc::Array<int> mut_parameter_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

} // namespace mut_actual_argument

#endif // MUT_ACTUAL_ARGUMENT_HPP
