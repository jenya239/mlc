#ifndef CODEGEN_HARNESS_HPP
#define CODEGEN_HARNESS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "load_item.hpp"
#include "module.hpp"
#include "program_to_semantic.hpp"

namespace codegen_harness {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String gen_program(ast::Program program) noexcept;

} // namespace codegen_harness

#endif // CODEGEN_HARNESS_HPP
