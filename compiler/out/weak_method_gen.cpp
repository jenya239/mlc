#include "weak_method_gen.hpp"

#include "registry.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "type_gen.hpp"
#include "semantic_type_structure.hpp"

namespace weak_method_gen {

using namespace registry;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace expr;
using namespace type_gen;
using namespace semantic_type_structure;

bool type_is_weak_pointer(std::shared_ptr<registry::Type> type_value) noexcept;

bool is_shared_weak_sugar_method(mlc::String method_name) noexcept;

bool is_weak_upgrade_sugar_method(mlc::String method_name) noexcept;

mlc::String gen_method_shared_weak_using_fragments(mlc::String receiver_fragment, std::shared_ptr<registry::Type> receiver_type, context::CodegenContext context) noexcept;

mlc::String gen_method_weak_upgrade_using_fragments(mlc::String receiver_fragment) noexcept;

std::shared_ptr<cpp_ast::CppExpression> make_callee_call_cpp(mlc::String callee_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_weak_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, std::shared_ptr<registry::Type> receiver_type, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_weak_upgrade_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression) noexcept;

bool type_is_weak_pointer(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [name, type_arguments] = _v_tgeneric; return name == mlc::String("Weak") && type_arguments.size() == 1; } return false; }();}

bool is_shared_weak_sugar_method(mlc::String method_name) noexcept{return method_name == mlc::String("weak") || method_name == mlc::String("downgrade");}

bool is_weak_upgrade_sugar_method(mlc::String method_name) noexcept{return method_name == mlc::String("upgrade") || method_name == mlc::String("lock");}

mlc::String gen_method_shared_weak_using_fragments(mlc::String receiver_fragment, std::shared_ptr<registry::Type> receiver_type, context::CodegenContext context) noexcept{
mlc::String inner_type_cpp = type_gen::sem_type_to_cpp(context, semantic_type_structure::shared_pointer_inner_type(receiver_type));
return expr::function_call_parentheses(mlc::String("std::weak_ptr<") + inner_type_cpp + mlc::String(">"), receiver_fragment);
}

mlc::String gen_method_weak_upgrade_using_fragments(mlc::String receiver_fragment) noexcept{return expr::function_call_parentheses(mlc::String("mlc::option::from_nullable"), receiver_fragment + mlc::String(".lock()"));}

std::shared_ptr<cpp_ast::CppExpression> make_callee_call_cpp(mlc::String callee_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> arguments) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(callee_name), arguments));}

std::shared_ptr<cpp_ast::CppExpression> gen_method_shared_weak_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression, std::shared_ptr<registry::Type> receiver_type, context::CodegenContext context) noexcept{
mlc::String inner_type_cpp = type_gen::sem_type_to_cpp(context, semantic_type_structure::shared_pointer_inner_type(receiver_type));
return make_callee_call_cpp(mlc::String("std::weak_ptr<") + inner_type_cpp + mlc::String(">"), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{receiver_expression});
}

std::shared_ptr<cpp_ast::CppExpression> gen_method_weak_upgrade_cpp(std::shared_ptr<cpp_ast::CppExpression> receiver_expression) noexcept{return make_callee_call_cpp(mlc::String("mlc::option::from_nullable"), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember(receiver_expression, mlc::String("lock"), false)), {}))});}

} // namespace weak_method_gen
