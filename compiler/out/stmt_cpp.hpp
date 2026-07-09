#ifndef STMT_CPP_HPP
#define STMT_CPP_HPP

#include "mlc.hpp"
#include <variant>
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "semantic_type_structure.hpp"
#include "let_pat_cpp.hpp"
#include "statement_context.hpp"
#include "expr_visitor_cpp.hpp"
namespace stmt_cpp {

struct StmtsCppAccumState {
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements;
  int next_try;
  context::CodegenContext codegen_context;
};
struct GenStmtCppResult {
  std::shared_ptr<cpp_ast::CppStatement> statement;
  int next_try;
  context::CodegenContext codegen_context;
};
struct GenStmtsCppWithContext {
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements;
  context::CodegenContext codegen_context;
};
mlc::String gen_stmts_for_cpp_codegen(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_expr_cpp_for_stmt_codegen(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;
GenStmtsCppWithContext gen_stmts_cpp_with_context(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;
std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_from_string_output(mlc::String output) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_stmts_from_string_output(mlc::String output) noexcept;
mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;
mlc::String print_cpp_statement_fragment_line(mlc::String fragment) noexcept;
mlc::String print_cpp_statement_default_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;
mlc::String print_cpp_statement_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;
std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_fragment_from_string_output(mlc::String output) noexcept;
mlc::String strip_trailing_newline(mlc::String text) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_typed_hash_map_empty_cpp(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> map_generic_type_arguments(std::shared_ptr<registry::Type> value_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_map_new_let_value_cpp(std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::shared_ptr<cpp_ast::CppExpression> fallback) noexcept;
GenStmtCppResult gen_for_statement_cpp(mlc::String variable_name, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_while_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept;
bool if_statement_has_unit_else(std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_block_trailing_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, int try_counter) noexcept;
std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block_cpp(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, int try_counter) noexcept;
std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_unit_if_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _if_semantic_type, context::CodegenContext context, int try_counter) noexcept;
mlc::String ident_name_from_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
bool semantic_type_is_unit(std::shared_ptr<registry::Type> semantic_type) noexcept;
GenStmtCppResult gen_auto_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_map_new_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_let_method_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> map_object, mlc::String method_name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_let_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_return_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_return_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_expr_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_if_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult gen_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;
GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept;
StmtsCppAccumState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

} // namespace stmt_cpp

#endif // STMT_CPP_HPP
