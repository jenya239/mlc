#define main mlc_user_main
#include "mut_actual_argument.hpp"

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "type_gen.hpp"

namespace mut_actual_argument {

using namespace semantic_ir;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace context;
using namespace type_gen;

bool sexpr_is_stable_reference_for_cpp_mut_parameter(std::shared_ptr<semantic_ir::SemanticExpression> actual_argument) noexcept{
  return [&]() -> bool {
auto __match_subject = actual_argument;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [__0, __1, __2] = semanticExpressionIdent; return true;
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [inner, __1, __2, __3] = semanticExpressionField; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [inner, __1, __2, __3] = semanticExpressionIndex; return sexpr_is_stable_reference_for_cpp_mut_parameter(inner);
}
return false;
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_invoked_lambda() noexcept{
  return std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String("", 0), true});
}
mlc::String wrap_invoke_with_mut_actual_argument_holder_prelude_if_any(mlc::String prelude_block, mlc::String invoke_expression) noexcept{
  if ((prelude_block.length() == 0))   {
    return invoke_expression;
  } else   {
    return ((((mlc::String("[&]() {\n", 8) + mlc::to_string(prelude_block)) + mlc::String("\nreturn ", 8)) + mlc::to_string(invoke_expression)) + mlc::String(";\n}()", 5));
  }
}
std::shared_ptr<cpp_ast::CppExpression> wrap_invoke_with_mut_prelude_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> prelude_statements, std::shared_ptr<cpp_ast::CppExpression> invoke_expression) noexcept{
  if ((prelude_statements.length() == 0))   {
    return invoke_expression;
  } else   {
    auto body_statements = prelude_statements;
    body_statements.push_back(emit_helpers::make_return_cpp_statement(invoke_expression));
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppLambda{mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{reference_capture_for_invoked_lambda()}, {}, std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mlc::String("auto", 4)}), body_statements}), {}});
  }
}

} // namespace mut_actual_argument
