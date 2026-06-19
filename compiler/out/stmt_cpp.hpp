#ifndef STMT_CPP_HPP
#define STMT_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "let_pat_cpp.hpp"
#include "statement_context.hpp"
#include "expr_visitor_cpp.hpp"

namespace stmt_cpp {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct GenStmtsCppWithContext {mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements;context::CodegenContext codegen_context;};

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

stmt_cpp::GenStmtsCppWithContext gen_stmts_cpp_with_context(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_stmts_from_string_output(mlc::String output) noexcept;

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;

} // namespace stmt_cpp

#endif // STMT_CPP_HPP
