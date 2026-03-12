#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace codegen {

struct Expr;
struct Stmt;

mlc::String gen_program(ast::Program prog) noexcept;

} // namespace codegen

#endif // CODEGEN_HPP
