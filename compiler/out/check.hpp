#ifndef CHECK_HPP
#define CHECK_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "ast.hpp"
#include "names.hpp"
#include "check_mutations.hpp"
#include "registry.hpp"
#include "trait_param_expand.hpp"
#include "param_destructure_expand.hpp"
#include "derive_validation.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"
#include "record_field_default_validate.hpp"
#include "partial_application_desugar.hpp"
#include "cycle_lint.hpp"

namespace check {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CheckOut {mlc::Array<mlc::String> errors;mlc::Array<mlc::String> warnings;registry::TypeRegistry registry;};

bool CheckOut_has_errors(check::CheckOut self) noexcept;

bool CheckOut_has_warnings(check::CheckOut self) noexcept;

mlc::Array<ast::Diagnostic> program_diagnostics(ast::Program program) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

} // namespace check

#endif // CHECK_HPP
