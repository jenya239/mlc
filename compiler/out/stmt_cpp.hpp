#ifndef STMT_CPP_HPP
#define STMT_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "stmt_eval.hpp"
#include "expr_visitor_cpp.hpp"

namespace stmt_cpp {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_stmts_cpp_as_string(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppStmt> cpp_stmt_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> cpp_stmts_from_string_output(mlc::String output) noexcept;

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept;

} // namespace stmt_cpp

#endif // STMT_CPP_HPP
