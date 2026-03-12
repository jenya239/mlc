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

struct InferResult {std::shared_ptr<registry::Type> inferred_type;mlc::Array<mlc::String> errors;};

struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;mlc::Array<mlc::String> errors;};

struct CheckOut {mlc::Array<mlc::String> errors;registry::TypeRegistry registry;};

bool CheckOut_has_errors(infer::CheckOut self) noexcept;

infer::CheckOut check_with_context(ast::Program entry, ast::Program full) noexcept;

infer::CheckOut check(ast::Program prog) noexcept;

} // namespace infer

#endif // INFER_HPP
