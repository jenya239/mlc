#ifndef MIR_TO_CPP_HPP
#define MIR_TO_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "mir_types.hpp"
#include "mir_ids.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"

namespace mir_to_cpp {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool mir_function_is_simple(mir_types::MirFunction function) noexcept;

ast::Result<std::shared_ptr<cpp_ast::CppDeclaration>, mlc::Array<mlc::String>> mir_function_to_cpp_declaration(mir_types::MirFunction function) noexcept;

} // namespace mir_to_cpp

#endif // MIR_TO_CPP_HPP
