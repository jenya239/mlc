#define main mlc_user_main
#include "stmt_eval.hpp"

#include "semantic_ir.hpp"
#include "let_pat.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "semantic_type_structure.hpp"
#include "expr.hpp"

namespace stmt_eval {

using namespace semantic_ir;
using namespace let_pat;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace expression_support;
using namespace type_gen;
using namespace literals;
using namespace semantic_type_structure;
using namespace expr;

mlc::String constexpr_named_type_cpp(std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context) noexcept{
  return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return context::CodegenContext_resolve(context, type_name); },
[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return mlc::String("", 0); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return mlc::String("", 0); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return mlc::String("", 0); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return mlc::String("", 0); },
[&](const registry::TI32& tI32) { return mlc::String("", 0); },
[&](const registry::TString& tString) { return mlc::String("", 0); },
[&](const registry::TBool& tBool) { return mlc::String("", 0); },
[&](const registry::TUnit& tUnit) { return mlc::String("", 0); },
[&](const registry::TI64& tI64) { return mlc::String("", 0); },
[&](const registry::TF64& tF64) { return mlc::String("", 0); },
[&](const registry::TU8& tU8) { return mlc::String("", 0); },
[&](const registry::TUsize& tUsize) { return mlc::String("", 0); },
[&](const registry::TChar& tChar) { return mlc::String("", 0); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return mlc::String("", 0); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return mlc::String("", 0); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return mlc::String("", 0); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("", 0); }
}, (*binding_semantic_type));
}
mlc::String constexpr_if_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> if_value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = if_value;
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_branch, else_branch, __3, __4] = semanticExpressionIf; return [&]() {
auto cond_code = gen_expr_fn(condition, context);
auto then_code = gen_expr_fn(then_branch, context);
auto else_code = expression_support::generate_conditional_else_with_empty_array_coercion(then_branch, else_branch, binding_semantic_type, context, gen_expr_fn);
auto named_type_cpp = constexpr_named_type_cpp(binding_semantic_type, context);
return [&]() -> mlc::String {
  if (((named_type_cpp != mlc::String("", 0)) && (named_type_cpp != mlc::String("auto", 4))))   {
    return ((((((((mlc::String("[&]() -> ", 9) + mlc::to_string(named_type_cpp)) + mlc::String(" {\nif (", 7)) + mlc::to_string(cond_code)) + mlc::String(") {\nreturn ", 11)) + mlc::to_string(then_code)) + mlc::String(";\n} else {\nreturn ", 18)) + mlc::to_string(else_code)) + mlc::String(";\n}\n}()", 7));
  } else   {
    return expr::ternary_conditional(cond_code, then_code, else_code);
  }
}();
}();
}
return constexpr_binding_default_code(if_value, context, gen_expr_fn);
std::abort();
}();
}
bool semantic_expression_is_if(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return true;
}
return false;
std::abort();
}();
}
bool semantic_expression_is_block(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [__0, __1, __2, __3] = semanticExpressionBlock; return true;
}
return false;
std::abort();
}();
}
bool semantic_expression_is_unit(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return true;
}
return false;
std::abort();
}();
}
bool semantic_expression_is_array(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [__0, __1, __2] = semanticExpressionArray; return true;
}
return false;
std::abort();
}();
}
mlc::Array<std::shared_ptr<registry::Type>> map_generic_type_arguments(std::shared_ptr<registry::Type> value_type) noexcept{
  return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [__0, type_arguments] = tGeneric; return type_arguments; },
[&](const registry::TI32& tI32) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TString& tString) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TBool& tBool) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TUnit& tUnit) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TI64& tI64) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TF64& tF64) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TU8& tU8) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TUsize& tUsize) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TChar& tChar) { return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TNamed& tNamed) { auto [__0] = tNamed; return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TTuple& tTuple) { auto [__0] = tTuple; return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return semantic_type_structure::empty_type_parameter_list(); },
[&](const registry::TUnknown& tUnknown) { return semantic_type_structure::empty_type_parameter_list(); }
}, (*value_type));
}
mlc::String constexpr_block_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [inner_statements, result_expression, __2, __3] = semanticExpressionBlock; return ((inner_statements.length() == 0) ? (constexpr_binding_value_code(result_expression, binding_semantic_type, context, gen_expr_fn)) : (constexpr_binding_default_code(value, context, gen_expr_fn))); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [__0, __1, __2] = semanticExpressionArray; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return constexpr_binding_default_code(value, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return constexpr_binding_default_code(value, context, gen_expr_fn); }
}, (*value));
}
mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  if (semantic_expression_is_if(value))   {
    return constexpr_if_binding_value_code(value, binding_semantic_type, context, gen_expr_fn);
  } else if (semantic_expression_is_block(value))   {
    return constexpr_block_binding_value_code(value, binding_semantic_type, context, gen_expr_fn);
  } else   {
    return constexpr_binding_default_code(value, context, gen_expr_fn);
  }
}
bool empty_unit_block_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return ((statements.length() == 0) && semantic_expression_is_unit(result_expression)); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return false; },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return false; },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return false; },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return false; },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return false; },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return false; },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return false; },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return false; },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return false; },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return false; },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return false; },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return false; },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return false; },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return false; },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return false; },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return false; },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return false; },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return false; },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return false; },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return false; },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return false; },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return false; },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return false; },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [__0, __1, __2] = semanticExpressionArray; return false; },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return false; },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return false; },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return false; },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return false; }
}, (*expression));
}
mlc::String block_result_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  if (semantic_expression_is_if(result_expression))   {
    return (statements_code + eval_stmt_if_nested(result_expression, final_context, gen_expr_fn));
  } else if (semantic_expression_is_unit(result_expression))   {
    return statements_code;
  } else   {
    return block_result_expression_trailing_code(statements_code, result_expression, final_context, gen_expr_fn);
  }
}
mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SemanticExpression> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = if_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition_expression, then_expression, else_expression, __3, __4] = semanticExpressionIf; return [&]() {
auto opening = expr::if_brace_block(gen_expr_fn(condition_expression, context), eval_block_body(then_expression, context, gen_expr_fn));
auto else_suffix = [&]() -> mlc::String {
  if (semantic_expression_is_unit(else_expression))   {
    return mlc::String("", 0);
  } else   {
    return (semantic_expression_is_if(else_expression) ? (expr::else_fragment_raw(eval_stmt_if_nested(else_expression, context, gen_expr_fn))) : ([&]() -> mlc::String {
  if ((semantic_expression_is_block(else_expression) && empty_unit_block_expression(else_expression)))   {
    return mlc::String("", 0);
  } else   {
    return expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn));
  }
}()));
  }
}();
return expr::fragment_with_newline(expr::prefix_with_optional_suffix(opening, else_suffix));
}();
}
return expr::if_always_true_block(eval_block_body(if_expression, context, gen_expr_fn));
std::abort();
}();
}
mlc::String eval_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  if (semantic_expression_is_block(expression))   {
    return eval_semantic_block_body(expression, context, gen_expr_fn);
  } else if (semantic_expression_is_if(expression))   {
    return eval_stmt_if_nested(expression, context, gen_expr_fn);
  } else   {
    return expr::suffix_semicolon_newline(gen_expr_fn(expression, context));
  }
}
mlc::String eval_semantic_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return [&]() {
auto statements_with_context = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
auto statements_code = context::GenStmtsResult_joined_code(statements_with_context.statements_parsed);
auto final_context = statements_with_context.codegen_context;
return block_result_trailing_code(statements_code, result_expression, final_context, gen_expr_fn);
}(); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [__0, __1, __2] = semanticExpressionArray; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); }
}, (*expression));
}
mlc::String eval_stmt_expr_block(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  auto statements_with_context = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
  auto statements_code = context::GenStmtsResult_joined_code(statements_with_context.statements_parsed);
  auto result_code = gen_expr_fn(result_expression, statements_with_context.codegen_context);
  if ((result_code == literals::gen_unit_literal()))   {
    return statements_code;
  } else   {
    return expr::append_trailing_expression_statement(statements_code, result_code);
  }
}
mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [operation, left_expression, right_expression, __3, __4] = semanticExpressionBin; return ((operation == mlc::String("=", 1)) ? (expr::assignment_statement(gen_expr_fn(left_expression, context), gen_expr_fn(right_expression, context))) : (expr::expression_operation_statement(operation, gen_expr_fn(left_expression, context), gen_expr_fn(right_expression, context)))); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return eval_stmt_if_nested(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [condition, statements, __2, __3] = semanticExpressionWhile; return expr::while_loop_statement(gen_expr_fn(condition, context), eval_stmts_str(statements, context, gen_expr_fn)); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [variable, iterator, statements, __3, __4] = semanticExpressionFor; return expr::for_loop_statement(cpp_naming::cpp_safe(variable), gen_expr_fn(iterator, context), eval_stmts_str(statements, context, gen_expr_fn)); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [resource, binder, with_statements, __3, __4] = semanticExpressionWith; return expr::with_block_statement(gen_expr_fn(resource, context), cpp_naming::cpp_safe(binder), eval_stmts_str(with_statements, context, gen_expr_fn)); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return eval_stmt_expr_block(statements, result_expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [__0, __1, __2] = semanticExpressionArray; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); }
}, (*expression));
}
mlc::String gen_let_array_value_from_tarray(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> inner_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  auto element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
  if ((elements.length() == 0))   {
    return expr::typed_array_empty_or_untyped_empty(element_cpp);
  } else if ((element_cpp == mlc::String("auto", 4)))   {
    return expr::array_literal(eval_elements_code(elements, context, gen_expr_fn));
  } else   {
    return expr::typed_array_braced_initializer(element_cpp, eval_elements_code(elements, context, gen_expr_fn));
  }
}
mlc::String gen_let_array_value_fallback(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  if ((elements.length() == 0))   {
    return expr::array_string_fallback_empty();
  } else   {
    return expr::constructor_call_braces(mlc::String("mlc::Array", 10), eval_elements_code(elements, context, gen_expr_fn));
  }
}
mlc::String gen_let_array_value_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> array_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  if (semantic_type_structure::type_is_array(array_type))   {
    return gen_let_array_value_from_tarray(elements, semantic_type_structure::array_element_type_from_array_type(array_type), context, gen_expr_fn);
  } else   {
    return gen_let_array_value_fallback(elements, context, gen_expr_fn);
  }
}
mlc::String gen_let_array_value_from_expression(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [elements, array_type, __2] = semanticExpressionArray; return gen_let_array_value_code(elements, array_type, context, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [__0, __1, __2, __3] = semanticExpressionBlock; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return gen_expr_fn(value, context); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return gen_expr_fn(value, context); }
}, (*value));
}
mlc::String gen_let_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  if (semantic_expression_is_array(value))   {
    return gen_let_array_value_from_expression(value, context, gen_expr_fn);
  } else if (semantic_expression_is_method_map_new(value))   {
    return gen_let_map_new_value_code(value, value_type, context, gen_expr_fn);
  } else   {
    return gen_expr_fn(value, context);
  }
}
mlc::String semantic_expression_ident_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [name, __1, __2] = semanticExpressionIdent; return name; },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [__0, __1, __2, __3] = semanticExpressionBlock; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [__0, __1, __2] = semanticExpressionArray; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return mlc::String("", 0); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return mlc::String("", 0); }
}, (*expression));
}
bool semantic_expression_is_method_map_new(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) { auto [map_object, method_name, __2, __3, __4, __5] = semanticExpressionMethod; return ((method_name == mlc::String("new", 3)) && (semantic_expression_ident_name(map_object) == mlc::String("Map", 3))); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) { auto [__0, __1, __2] = semanticExpressionInt; return false; },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) { auto [__0, __1, __2] = semanticExpressionStr; return false; },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) { auto [__0, __1, __2] = semanticExpressionFloat; return false; },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) { auto [__0, __1, __2] = semanticExpressionI64; return false; },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) { auto [__0, __1, __2] = semanticExpressionU8; return false; },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) { auto [__0, __1, __2] = semanticExpressionUsize; return false; },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) { auto [__0, __1, __2] = semanticExpressionChar; return false; },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) { auto [__0, __1, __2] = semanticExpressionBool; return false; },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) { auto [__0, __1] = semanticExpressionUnit; return false; },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return false; },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) { auto [__0, __1, __2] = semanticExpressionIdent; return false; },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return false; },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) { auto [__0, __1, __2, __3] = semanticExpressionUn; return false; },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return false; },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) { auto [__0, __1, __2, __3] = semanticExpressionField; return false; },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) { auto [__0, __1, __2, __3] = semanticExpressionIndex; return false; },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return false; },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) { auto [__0, __1, __2, __3] = semanticExpressionBlock; return false; },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) { auto [__0, __1, __2, __3] = semanticExpressionWhile; return false; },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return false; },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) { auto [__0, __1, __2, __3] = semanticExpressionMatch; return false; },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) { auto [__0, __1, __2, __3] = semanticExpressionRecord; return false; },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return false; },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) { auto [__0, __1, __2] = semanticExpressionArray; return false; },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) { auto [__0, __1, __2] = semanticExpressionTuple; return false; },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) { auto [__0, __1, __2] = semanticExpressionQuestion; return false; },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) { auto [__0, __1, __2, __3] = semanticExpressionLambda; return false; },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return false; }
}, (*expression));
}
context::GenStmtResult gen_let_stmt_result_default(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), gen_let_value_code(value, value_type, context, gen_expr_fn)), try_counter, context};
}
context::GenStmtResult gen_let_stmt_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) -> context::GenStmtResult { auto [inner_expression, __1, __2] = semanticExpressionQuestion; return context::GenStmtResult{eval_try_unwrap(inner_expression, context, ((mlc::String("__try_", 6) + mlc::to_string(mlc::to_string(try_counter))) + mlc::String("", 0)), expr::let_from_try_ok_field0(cpp_naming::cpp_safe(name), ((mlc::String("__try_", 6) + mlc::to_string(mlc::to_string(try_counter))) + mlc::String("", 0))), gen_expr_fn), mlc::arith::checked_add(try_counter, 1), context}; },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionInt; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionStr; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionFloat; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionI64; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionU8; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionUsize; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionChar; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionBool; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) -> context::GenStmtResult { auto [__0, __1] = semanticExpressionUnit; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionIdent; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionUn; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionField; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionIndex; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionBlock; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionWhile; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionMatch; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionRecord; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionArray; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionTuple; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionLambda; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); }
}, (*value));
}
context::GenStmtResult gen_expr_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return context::GenStmtResult{eval_stmt_expr(expression, context, gen_expr_fn), try_counter, context};
}
context::GenStmtResult gen_expr_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) -> context::GenStmtResult { auto [inner_expression, __1, __2] = semanticExpressionQuestion; return context::GenStmtResult{eval_try_unwrap(inner_expression, context, ((mlc::String("__try_", 6) + mlc::to_string(mlc::to_string(try_counter))) + mlc::String("", 0)), expr::discard_try_ok_field0_statement(((mlc::String("__try_", 6) + mlc::to_string(mlc::to_string(try_counter))) + mlc::String("", 0))), gen_expr_fn), mlc::arith::checked_add(try_counter, 1), context}; },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionInt; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionStr; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionFloat; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionI64; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionU8; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionUsize; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionChar; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionBool; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) -> context::GenStmtResult { auto [__0, __1] = semanticExpressionUnit; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionIdent; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionUn; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionField; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionIndex; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionBlock; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionWhile; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionMatch; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionRecord; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionArray; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionTuple; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionLambda; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); }
}, (*expression));
}
context::GenStmtResult gen_return_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) -> context::GenStmtResult { auto [inner_expression, __1, __2] = semanticExpressionQuestion; return context::GenStmtResult{eval_try_unwrap(inner_expression, context, ((mlc::String("__try_ret_", 10) + mlc::to_string(mlc::to_string(try_counter))) + mlc::String("", 0)), expr::return_try_ok_field0_statement(((mlc::String("__try_ret_", 10) + mlc::to_string(mlc::to_string(try_counter))) + mlc::String("", 0))), gen_expr_fn), mlc::arith::checked_add(try_counter, 1), context}; },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionInt; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionStr; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionFloat; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionI64; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionU8; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionUsize; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionChar; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionBool; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) -> context::GenStmtResult { auto [__0, __1] = semanticExpressionUnit; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionIdent; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionUn; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionField; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionIndex; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionBlock; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionWhile; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionMatch; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionRecord; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionArray; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) -> context::GenStmtResult { auto [__0, __1, __2] = semanticExpressionTuple; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) -> context::GenStmtResult { auto [__0, __1, __2, __3] = semanticExpressionLambda; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) -> context::GenStmtResult { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); }
}, (*expression));
}
context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticStatementLet& semanticStatementLet) -> context::GenStmtResult { auto [name, __1, value, value_type, __4] = semanticStatementLet; return gen_let_stmt_result(name, value, value_type, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticStatementLetPattern& semanticStatementLetPattern) -> context::GenStmtResult { auto [pattern, __1, value, value_type, has_else, else_body, __6] = semanticStatementLetPattern; return [&]() {
auto pattern_parsed = let_pat::gen_let_pattern_statement(pattern, value, value_type, has_else, else_body, context, gen_expr_fn);
return context::GenStmtResult{pattern_parsed.output, try_counter, pattern_parsed.codegen_context};
}(); },
[&](const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst) -> context::GenStmtResult { auto [name, value, binding_semantic_type, __3] = semanticStatementLetConst; return context::GenStmtResult{expr::constexpr_auto_binding_statement(cpp_naming::cpp_safe(name), constexpr_binding_value_code(value, binding_semantic_type, context, gen_expr_fn)), try_counter, context}; },
[&](const semantic_ir::SemanticStatementExpr& semanticStatementExpr) -> context::GenStmtResult { auto [expression, __1] = semanticStatementExpr; return gen_expr_stmt_result(expression, context, try_counter, gen_expr_fn); },
[&](const semantic_ir::SemanticStatementBreak& semanticStatementBreak) -> context::GenStmtResult { auto [__0] = semanticStatementBreak; return context::GenStmtResult{expr::break_line(), try_counter, context}; },
[&](const semantic_ir::SemanticStatementContinue& semanticStatementContinue) -> context::GenStmtResult { auto [__0] = semanticStatementContinue; return context::GenStmtResult{expr::continue_line(), try_counter, context}; },
[&](const semantic_ir::SemanticStatementReturn& semanticStatementReturn) -> context::GenStmtResult { auto [expression, __1] = semanticStatementReturn; return gen_return_stmt_result(expression, context, try_counter, gen_expr_fn); }
}, (*statement));
}
context::GenStmtsWithContext eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  auto statements_parsed = context::GenStmtsResult{{}, try_counter};
  auto codegen_context = context;
  auto index = 0;
  while ((index < statements.length()))   {
    auto statement = statements[index];
    auto statement_parsed = eval_stmt_with_try(statement, codegen_context, statements_parsed.next_try, gen_expr_fn);
    (statements_parsed = context::GenStmtsResult_append_stmt(statements_parsed, statement_parsed));
    (codegen_context = statement_parsed.codegen_context);
    context::mutate_context_from_statement(statement, codegen_context);
    (index = mlc::arith::checked_add(index, 1));
  }
  return context::GenStmtsWithContext{statements_parsed, codegen_context};
}
mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
  return context::GenStmtsResult_joined_code(eval_stmts_str_with_try(statements, context, 0, gen_expr_fn).statements_parsed);
}

} // namespace stmt_eval
