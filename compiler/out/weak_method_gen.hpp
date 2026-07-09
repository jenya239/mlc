#ifndef WEAK_METHOD_GEN_HPP
#define WEAK_METHOD_GEN_HPP

#include "mlc.hpp"
#include <variant>
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "type_gen.hpp"
#include "semantic_type_structure.hpp"
namespace weak_method_gen {

bool type_is_weak_pointer(std::shared_ptr<registry::Type> type_value) noexcept;
bool is_shared_weak_sugar_method(mlc::String method_name) noexcept;
bool is_weak_upgrade_sugar_method(mlc::String method_name) noexcept;
mlc::String gen_method_shared_weak_using_fragments(mlc::String receiver_fragment, std::shared_ptr<registry::Type> receiver_type, context::CodegenContext context) noexcept;
mlc::String gen_method_weak_upgrade_using_fragments(mlc::String receiver_fragment) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_callee_call_cpp(mlc::String callee_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_weak_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, std::shared_ptr<registry::Type> receiver_type, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_method_weak_upgrade_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression) noexcept;

} // namespace weak_method_gen

#endif // WEAK_METHOD_GEN_HPP
