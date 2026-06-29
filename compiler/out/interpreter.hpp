#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "lower_program.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "execute.hpp"
#include "runtime.hpp"
#include "outcome.hpp"

namespace interpreter {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

ast::Result<int, mlc::Array<mlc::String>> interpret_mir_program(mir_types::MirProgram program, bool trace_enabled) noexcept;

ast::Result<int, mlc::Array<mlc::String>> run_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, bool trace_enabled) noexcept;

ast::Result<value::VmValue, mlc::String> interpret_mir_function(mir_types::MirFunction function) noexcept;

} // namespace interpreter

#endif // INTERPRETER_HPP
