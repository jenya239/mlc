#ifndef INFER_HPP
#define INFER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "names.hpp"
#include "registry.hpp"

namespace infer {

struct Expr;
struct Stmt;

struct CheckOut {mlc::Array<mlc::String> errors;registry::TypeRegistry registry;};

infer::CheckOut check(ast::Program prog) noexcept;

} // namespace infer

#endif // INFER_HPP
