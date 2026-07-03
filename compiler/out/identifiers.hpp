#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
namespace identifiers {

mlc::String resolve_identifier_in_self_context(mlc::String name, context::CodegenContext context) noexcept;
mlc::String gen_identifier(mlc::String name, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_identifier_cpp(mlc::String name, context::CodegenContext context) noexcept;

} // namespace identifiers

#endif // IDENTIFIERS_HPP
