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

mlc::String constexpr_named_type_cpp(std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context) noexcept;

mlc::String constexpr_if_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> if_value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String constexpr_binding_default_code(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

bool semantic_expression_is_if(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

bool semantic_expression_is_block(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

bool semantic_expression_is_unit(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

bool semantic_expression_is_array(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> map_generic_type_arguments(std::shared_ptr<registry::Type> value_type) noexcept;

mlc::String constexpr_block_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

bool empty_unit_block_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

mlc::String block_result_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String block_result_expression_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SemanticExpression> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_semantic_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_expr_default_suffix(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_expr_block(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String gen_let_array_value_from_tarray(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> inner_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String gen_let_array_value_fallback(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String gen_let_array_value_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> array_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String gen_let_map_new_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String gen_let_array_value_from_expression(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String gen_let_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String semantic_expression_ident_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

bool semantic_expression_is_method_map_new(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

context::GenStmtResult gen_let_stmt_result_default(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult gen_let_stmt_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult gen_expr_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult gen_expr_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult gen_return_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult gen_return_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

context::GenStmtsWithContext eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;

mlc::String constexpr_named_type_cpp(std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const TNamed& tnamed) -> mlc::String { auto [type_name] = tnamed; return context::CodegenContext_resolve(context, type_name); },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [_w0, _w1] = tgeneric; return mlc::String(""); },
  [&](const TArray& tarray) -> mlc::String { auto [_w0] = tarray; return mlc::String(""); },
  [&](const TTuple& ttuple) -> mlc::String { auto [_w0] = ttuple; return mlc::String(""); },
  [&](const TPair& tpair) -> mlc::String { auto [_w0, _w1] = tpair; return mlc::String(""); },
  [&](const TI32& ti32) -> mlc::String { return mlc::String(""); },
  [&](const TString& tstring) -> mlc::String { return mlc::String(""); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String(""); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String(""); },
  [&](const TI64& ti64) -> mlc::String { return mlc::String(""); },
  [&](const TF64& tf64) -> mlc::String { return mlc::String(""); },
  [&](const TU8& tu8) -> mlc::String { return mlc::String(""); },
  [&](const TUsize& tusize) -> mlc::String { return mlc::String(""); },
  [&](const TChar& tchar) -> mlc::String { return mlc::String(""); },
  [&](const TShared& tshared) -> mlc::String { auto [_w0] = tshared; return mlc::String(""); },
  [&](const TFn& tfn) -> mlc::String { auto [_w0, _w1] = tfn; return mlc::String(""); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [_w0, _w1] = tassoc; return mlc::String(""); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String(""); }
}, (*binding_semantic_type));}

mlc::String constexpr_if_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> if_value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*if_value)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*if_value)._); auto [condition, then_branch, else_branch, _w0, _w1] = _v_semanticexpressionif; return [&]() -> mlc::String { 
  mlc::String cond_code = gen_expr_fn(condition, context);
  mlc::String then_code = gen_expr_fn(then_branch, context);
  mlc::String else_code = expression_support::generate_conditional_else_with_empty_array_coercion(then_branch, else_branch, binding_semantic_type, context, gen_expr_fn);
  mlc::String named_type_cpp = constexpr_named_type_cpp(binding_semantic_type, context);
  return named_type_cpp != mlc::String("") && named_type_cpp != mlc::String("auto") ? mlc::String("[&]() -> ") + named_type_cpp + mlc::String(" {\nif (") + cond_code + mlc::String(") {\nreturn ") + then_code + mlc::String(";\n} else {\nreturn ") + else_code + mlc::String(";\n}\n}()") : expr::ternary_conditional(cond_code, then_code, else_code);
 }(); } return constexpr_binding_default_code(if_value, context, gen_expr_fn); }();}

mlc::String constexpr_binding_default_code(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return gen_expr_fn(value, context);}

bool semantic_expression_is_if(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticexpressionif; return true; } return false; }();}

bool semantic_expression_is_block(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [_w0, _w1, _w2, _w3] = _v_semanticexpressionblock; return true; } return false; }();}

bool semantic_expression_is_unit(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return true; } return false; }();}

bool semantic_expression_is_array(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [_w0, _w1, _w2] = _v_semanticexpressionarray; return true; } return false; }();}

mlc::Array<std::shared_ptr<registry::Type>> map_generic_type_arguments(std::shared_ptr<registry::Type> value_type) noexcept{return std::visit(overloaded{
  [&](const TGeneric& tgeneric) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, type_arguments] = tgeneric; return type_arguments; },
  [&](const TI32& ti32) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TString& tstring) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TBool& tbool) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TUnit& tunit) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TI64& ti64) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TF64& tf64) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TU8& tu8) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TUsize& tusize) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TChar& tchar) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TShared& tshared) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tshared; return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TNamed& tnamed) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tnamed; return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TArray& tarray) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tarray; return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TPair& tpair) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tpair; return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TTuple& ttuple) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = ttuple; return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TFn& tfn) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tfn; return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TAssoc& tassoc) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tassoc; return semantic_type_structure::empty_type_parameter_list(); },
  [&](const TUnknown& tunknown) -> mlc::Array<std::shared_ptr<registry::Type>> { return semantic_type_structure::empty_type_parameter_list(); }
}, (*value_type));}

mlc::String constexpr_block_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> mlc::String { auto [inner_statements, result_expression, _w0, _w1] = semanticexpressionblock; return inner_statements.size() == 0 ? constexpr_binding_value_code(result_expression, binding_semantic_type, context, gen_expr_fn) : constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionint; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionstr; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionfloat; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressioni64; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionu8; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionusize; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionchar; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionbool; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> mlc::String { auto [_w0, _w1] = semanticexpressionunit; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> mlc::String { auto [_w0, _w1] = semanticexpressionextern; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionident; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionarray; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressiontuple; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionquestion; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return constexpr_binding_default_code(value, context, gen_expr_fn); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return constexpr_binding_default_code(value, context, gen_expr_fn); }
}, (*value)._);}

mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return semantic_expression_is_if(value) ? constexpr_if_binding_value_code(value, binding_semantic_type, context, gen_expr_fn) : semantic_expression_is_block(value) ? constexpr_block_binding_value_code(value, binding_semantic_type, context, gen_expr_fn) : constexpr_binding_default_code(value, context, gen_expr_fn);}

mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
mlc::String inner_code = gen_expr_fn(inner_expression, context);
return expr::try_unwrap_result_block(try_identifier, inner_code, success_line);
}

bool empty_unit_block_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> bool { auto [statements, result_expression, _w0, _w1] = semanticexpressionblock; return statements.size() == 0 && semantic_expression_is_unit(result_expression); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> bool { auto [_w0, _w1, _w2] = semanticexpressionint; return false; },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> bool { auto [_w0, _w1, _w2] = semanticexpressionstr; return false; },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> bool { auto [_w0, _w1, _w2] = semanticexpressionfloat; return false; },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> bool { auto [_w0, _w1, _w2] = semanticexpressioni64; return false; },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> bool { auto [_w0, _w1, _w2] = semanticexpressionu8; return false; },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> bool { auto [_w0, _w1, _w2] = semanticexpressionusize; return false; },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> bool { auto [_w0, _w1, _w2] = semanticexpressionchar; return false; },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> bool { auto [_w0, _w1, _w2] = semanticexpressionbool; return false; },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> bool { auto [_w0, _w1] = semanticexpressionunit; return false; },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> bool { auto [_w0, _w1] = semanticexpressionextern; return false; },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> bool { auto [_w0, _w1, _w2] = semanticexpressionident; return false; },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return false; },
  [&](const SemanticExpressionUn& semanticexpressionun) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return false; },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return false; },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> bool { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return false; },
  [&](const SemanticExpressionField& semanticexpressionfield) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return false; },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return false; },
  [&](const SemanticExpressionIf& semanticexpressionif) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return false; },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return false; },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return false; },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return false; },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return false; },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return false; },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> bool { auto [_w0, _w1, _w2] = semanticexpressionarray; return false; },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> bool { auto [_w0, _w1, _w2] = semanticexpressiontuple; return false; },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> bool { auto [_w0, _w1, _w2] = semanticexpressionquestion; return false; },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return false; },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return false; }
}, (*expression)._);}

mlc::String block_result_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return semantic_expression_is_if(result_expression) ? statements_code + eval_stmt_if_nested(result_expression, final_context, gen_expr_fn) : semantic_expression_is_unit(result_expression) ? statements_code : block_result_expression_trailing_code(statements_code, result_expression, final_context, gen_expr_fn);}

mlc::String block_result_expression_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
mlc::String result_code = gen_expr_fn(result_expression, final_context);
return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
}

mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SemanticExpression> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*if_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*if_expression)._); auto [condition_expression, then_expression, else_expression, _w0, _w1] = _v_semanticexpressionif; return [&]() -> mlc::String { 
  mlc::String opening = expr::if_brace_block(gen_expr_fn(condition_expression, context), eval_block_body(then_expression, context, gen_expr_fn));
  mlc::String else_suffix = semantic_expression_is_unit(else_expression) ? mlc::String("") : semantic_expression_is_if(else_expression) ? expr::else_fragment_raw(eval_stmt_if_nested(else_expression, context, gen_expr_fn)) : semantic_expression_is_block(else_expression) && empty_unit_block_expression(else_expression) ? mlc::String("") : expr::else_brace_block(eval_block_body(else_expression, context, gen_expr_fn));
  return expr::fragment_with_newline(expr::prefix_with_optional_suffix(opening, else_suffix));
 }(); } return expr::if_always_true_block(eval_block_body(if_expression, context, gen_expr_fn)); }();}

mlc::String eval_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return semantic_expression_is_block(expression) ? eval_semantic_block_body(expression, context, gen_expr_fn) : semantic_expression_is_if(expression) ? eval_stmt_if_nested(expression, context, gen_expr_fn) : expr::suffix_semicolon_newline(gen_expr_fn(expression, context));}

mlc::String eval_semantic_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> mlc::String { auto [statements, result_expression, _w0, _w1] = semanticexpressionblock; return [&]() -> mlc::String { 
  context::GenStmtsWithContext statements_with_context = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
  mlc::String statements_code = context::GenStmtsResult_joined_code(statements_with_context.statements_parsed);
  context::CodegenContext final_context = statements_with_context.codegen_context;
  return block_result_trailing_code(statements_code, result_expression, final_context, gen_expr_fn);
 }(); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionint; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionstr; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionfloat; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressioni64; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionu8; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionusize; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionchar; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionbool; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> mlc::String { auto [_w0, _w1] = semanticexpressionunit; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> mlc::String { auto [_w0, _w1] = semanticexpressionextern; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionident; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionarray; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressiontuple; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionquestion; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return expr::suffix_semicolon_newline(gen_expr_fn(expression, context)); }
}, (*expression)._);}

mlc::String eval_stmt_expr_default_suffix(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return expr::suffix_semicolon_newline(gen_expr_fn(expression, context));}

mlc::String eval_stmt_expr_block(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
context::GenStmtsWithContext statements_with_context = eval_stmts_str_with_try(statements, context, 0, gen_expr_fn);
mlc::String statements_code = context::GenStmtsResult_joined_code(statements_with_context.statements_parsed);
mlc::String result_code = gen_expr_fn(result_expression, statements_with_context.codegen_context);
return result_code == literals::gen_unit_literal() ? statements_code : expr::append_trailing_expression_statement(statements_code, result_code);
}

mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionBin& semanticexpressionbin) -> mlc::String { auto [operation, left_expression, right_expression, _w0, _w1] = semanticexpressionbin; return operation == mlc::String("=") ? expr::assignment_statement(gen_expr_fn(left_expression, context), gen_expr_fn(right_expression, context)) : expr::expression_operation_statement(operation, gen_expr_fn(left_expression, context), gen_expr_fn(right_expression, context)); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return eval_stmt_if_nested(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> mlc::String { auto [condition, statements, _w0, _w1] = semanticexpressionwhile; return expr::while_loop_statement(gen_expr_fn(condition, context), eval_stmts_str(statements, context, gen_expr_fn)); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> mlc::String { auto [variable, iterator, statements, _w0, _w1] = semanticexpressionfor; return expr::for_loop_statement(cpp_naming::cpp_safe(variable), gen_expr_fn(iterator, context), eval_stmts_str(statements, context, gen_expr_fn)); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> mlc::String { auto [resource, binder, with_statements, _w0, _w1] = semanticexpressionwith; return expr::with_block_statement(gen_expr_fn(resource, context), cpp_naming::cpp_safe(binder), eval_stmts_str(with_statements, context, gen_expr_fn)); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> mlc::String { auto [statements, result_expression, _w0, _w1] = semanticexpressionblock; return eval_stmt_expr_block(statements, result_expression, context, gen_expr_fn); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionint; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionstr; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionfloat; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressioni64; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionu8; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionusize; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionchar; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionbool; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> mlc::String { auto [_w0, _w1] = semanticexpressionunit; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> mlc::String { auto [_w0, _w1] = semanticexpressionextern; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionident; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionarray; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressiontuple; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionquestion; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return eval_stmt_expr_default_suffix(expression, context, gen_expr_fn); }
}, (*expression)._);}

mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return elements.map([gen_expr_fn, context](std::shared_ptr<semantic_ir::SemanticExpression> element) mutable { return gen_expr_fn(element, context); }).join(mlc::String(", "));}

mlc::String gen_let_array_value_from_tarray(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> inner_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
mlc::String element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
return elements.size() == 0 ? expr::typed_array_empty_or_untyped_empty(element_cpp) : element_cpp == mlc::String("auto") ? expr::array_literal(eval_elements_code(elements, context, gen_expr_fn)) : expr::typed_array_braced_initializer(element_cpp, eval_elements_code(elements, context, gen_expr_fn));
}

mlc::String gen_let_array_value_fallback(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return elements.size() == 0 ? expr::array_string_fallback_empty() : expr::constructor_call_braces(mlc::String("mlc::Array"), eval_elements_code(elements, context, gen_expr_fn));}

mlc::String gen_let_array_value_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> array_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return semantic_type_structure::type_is_array(array_type) ? gen_let_array_value_from_tarray(elements, semantic_type_structure::array_element_type_from_array_type(array_type), context, gen_expr_fn) : gen_let_array_value_fallback(elements, context, gen_expr_fn);}

mlc::String gen_let_map_new_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> type_arguments = map_generic_type_arguments(value_type);
return semantic_type_structure::receiver_type_is_map(value_type) && type_arguments.size() == 2 ? expr::hash_map_empty_instantiation(type_gen::sem_type_to_cpp(context, type_arguments[0]), type_gen::sem_type_to_cpp(context, type_arguments[1])) : gen_expr_fn(value, context);
}

mlc::String gen_let_array_value_from_expression(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionArray& semanticexpressionarray) -> mlc::String { auto [elements, array_type, _w0] = semanticexpressionarray; return gen_let_array_value_code(elements, array_type, context, gen_expr_fn); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionint; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionstr; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionfloat; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressioni64; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionu8; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionusize; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionchar; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionbool; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> mlc::String { auto [_w0, _w1] = semanticexpressionunit; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> mlc::String { auto [_w0, _w1] = semanticexpressionextern; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionident; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressiontuple; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionquestion; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return gen_expr_fn(value, context); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return gen_expr_fn(value, context); }
}, (*value)._);}

mlc::String gen_let_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return semantic_expression_is_array(value) ? gen_let_array_value_from_expression(value, context, gen_expr_fn) : semantic_expression_is_method_map_new(value) ? gen_let_map_new_value_code(value, value_type, context, gen_expr_fn) : gen_expr_fn(value, context);}

mlc::String semantic_expression_ident_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionIdent& semanticexpressionident) -> mlc::String { auto [name, _w0, _w1] = semanticexpressionident; return name; },
  [&](const SemanticExpressionInt& semanticexpressionint) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionint; return mlc::String(""); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionstr; return mlc::String(""); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionfloat; return mlc::String(""); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressioni64; return mlc::String(""); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionu8; return mlc::String(""); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionusize; return mlc::String(""); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionchar; return mlc::String(""); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionbool; return mlc::String(""); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> mlc::String { auto [_w0, _w1] = semanticexpressionunit; return mlc::String(""); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> mlc::String { auto [_w0, _w1] = semanticexpressionextern; return mlc::String(""); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return mlc::String(""); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return mlc::String(""); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return mlc::String(""); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return mlc::String(""); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return mlc::String(""); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return mlc::String(""); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return mlc::String(""); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return mlc::String(""); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return mlc::String(""); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return mlc::String(""); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return mlc::String(""); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return mlc::String(""); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return mlc::String(""); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionarray; return mlc::String(""); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressiontuple; return mlc::String(""); },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> mlc::String { auto [_w0, _w1, _w2] = semanticexpressionquestion; return mlc::String(""); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return mlc::String(""); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return mlc::String(""); }
}, (*expression)._);}

bool semantic_expression_is_method_map_new(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> bool { auto [map_object, method_name, _w0, _w1, _w2, _w3] = semanticexpressionmethod; return method_name == mlc::String("new") && semantic_expression_ident_name(map_object) == mlc::String("Map"); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> bool { auto [_w0, _w1, _w2] = semanticexpressionint; return false; },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> bool { auto [_w0, _w1, _w2] = semanticexpressionstr; return false; },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> bool { auto [_w0, _w1, _w2] = semanticexpressionfloat; return false; },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> bool { auto [_w0, _w1, _w2] = semanticexpressioni64; return false; },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> bool { auto [_w0, _w1, _w2] = semanticexpressionu8; return false; },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> bool { auto [_w0, _w1, _w2] = semanticexpressionusize; return false; },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> bool { auto [_w0, _w1, _w2] = semanticexpressionchar; return false; },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> bool { auto [_w0, _w1, _w2] = semanticexpressionbool; return false; },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> bool { auto [_w0, _w1] = semanticexpressionunit; return false; },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> bool { auto [_w0, _w1] = semanticexpressionextern; return false; },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> bool { auto [_w0, _w1, _w2] = semanticexpressionident; return false; },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return false; },
  [&](const SemanticExpressionUn& semanticexpressionun) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return false; },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return false; },
  [&](const SemanticExpressionField& semanticexpressionfield) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return false; },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return false; },
  [&](const SemanticExpressionIf& semanticexpressionif) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return false; },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return false; },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return false; },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return false; },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return false; },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return false; },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return false; },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> bool { auto [_w0, _w1, _w2] = semanticexpressionarray; return false; },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> bool { auto [_w0, _w1, _w2] = semanticexpressiontuple; return false; },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> bool { auto [_w0, _w1, _w2] = semanticexpressionquestion; return false; },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> bool { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return false; },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> bool { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return false; }
}, (*expression)._);}

context::GenStmtResult gen_let_stmt_result_default(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return context::GenStmtResult{expr::auto_binding_statement(cpp_naming::cpp_safe(name), gen_let_value_code(value, value_type, context, gen_expr_fn)), try_counter, context};}

context::GenStmtResult gen_let_stmt_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> context::GenStmtResult { auto [inner_expression, _w0, _w1] = semanticexpressionquestion; return context::GenStmtResult{eval_try_unwrap(inner_expression, context, mlc::String("__try_") + mlc::to_string(try_counter), expr::let_from_try_ok_field0(cpp_naming::cpp_safe(name), mlc::String("__try_") + mlc::to_string(try_counter)), gen_expr_fn), try_counter + 1, context}; },
  [&](const SemanticExpressionInt& semanticexpressionint) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionint; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionstr; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionfloat; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressioni64; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionu8; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionusize; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionchar; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionbool; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> context::GenStmtResult { auto [_w0, _w1] = semanticexpressionunit; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> context::GenStmtResult { auto [_w0, _w1] = semanticexpressionextern; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionident; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionarray; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressiontuple; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return gen_let_stmt_result_default(name, value, value_type, context, try_counter, gen_expr_fn); }
}, (*value)._);}

context::GenStmtResult gen_expr_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return context::GenStmtResult{eval_stmt_expr(expression, context, gen_expr_fn), try_counter, context};}

context::GenStmtResult gen_expr_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> context::GenStmtResult { auto [inner_expression, _w0, _w1] = semanticexpressionquestion; return context::GenStmtResult{eval_try_unwrap(inner_expression, context, mlc::String("__try_") + mlc::to_string(try_counter), expr::discard_try_ok_field0_statement(mlc::String("__try_") + mlc::to_string(try_counter)), gen_expr_fn), try_counter + 1, context}; },
  [&](const SemanticExpressionInt& semanticexpressionint) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionint; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionstr; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionfloat; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressioni64; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionu8; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionusize; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionchar; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionbool; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> context::GenStmtResult { auto [_w0, _w1] = semanticexpressionunit; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> context::GenStmtResult { auto [_w0, _w1] = semanticexpressionextern; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionident; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionarray; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressiontuple; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return gen_expr_stmt_result_default(expression, context, try_counter, gen_expr_fn); }
}, (*expression)._);}

context::GenStmtResult gen_return_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return context::GenStmtResult{expr::return_line(gen_expr_fn(expression, context)), try_counter, context};}

context::GenStmtResult gen_return_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> context::GenStmtResult { auto [inner_expression, _w0, _w1] = semanticexpressionquestion; return context::GenStmtResult{eval_try_unwrap(inner_expression, context, mlc::String("__try_ret_") + mlc::to_string(try_counter), expr::return_try_ok_field0_statement(mlc::String("__try_ret_") + mlc::to_string(try_counter)), gen_expr_fn), try_counter + 1, context}; },
  [&](const SemanticExpressionInt& semanticexpressionint) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionint; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionstr; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionfloat; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressioni64; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionu8; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionusize; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionchar; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionbool; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> context::GenStmtResult { auto [_w0, _w1] = semanticexpressionunit; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> context::GenStmtResult { auto [_w0, _w1] = semanticexpressionextern; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionident; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressionarray; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> context::GenStmtResult { auto [_w0, _w1, _w2] = semanticexpressiontuple; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> context::GenStmtResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return gen_return_stmt_result_default(expression, context, try_counter, gen_expr_fn); }
}, (*expression)._);}

context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return std::visit(overloaded{
  [&](const SemanticStatementLet& semanticstatementlet) -> context::GenStmtResult { auto [name, _w0, value, value_type, _w1] = semanticstatementlet; return gen_let_stmt_result(name, value, value_type, context, try_counter, gen_expr_fn); },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> context::GenStmtResult { auto [pattern, _w0, value, value_type, has_else, else_body, _w1] = semanticstatementletpattern; return [&]() -> context::GenStmtResult { 
  let_pat::GenLetPatternResult pattern_parsed = let_pat::gen_let_pattern_statement(pattern, value, value_type, has_else, else_body, context, gen_expr_fn);
  return context::GenStmtResult{pattern_parsed.output, try_counter, pattern_parsed.codegen_context};
 }(); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> context::GenStmtResult { auto [name, value, binding_semantic_type, _w0] = semanticstatementletconst; return context::GenStmtResult{expr::constexpr_auto_binding_statement(cpp_naming::cpp_safe(name), constexpr_binding_value_code(value, binding_semantic_type, context, gen_expr_fn)), try_counter, context}; },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> context::GenStmtResult { auto [expression, _w0] = semanticstatementexpr; return gen_expr_stmt_result(expression, context, try_counter, gen_expr_fn); },
  [&](const SemanticStatementBreak& semanticstatementbreak) -> context::GenStmtResult { auto [_w0] = semanticstatementbreak; return context::GenStmtResult{expr::break_line(), try_counter, context}; },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> context::GenStmtResult { auto [_w0] = semanticstatementcontinue; return context::GenStmtResult{expr::continue_line(), try_counter, context}; },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> context::GenStmtResult { auto [expression, _w0] = semanticstatementreturn; return gen_return_stmt_result(expression, context, try_counter, gen_expr_fn); }
}, (*statement)._);}

context::GenStmtsWithContext eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{
context::GenStmtsResult statements_parsed = context::GenStmtsResult{{}, try_counter};
context::CodegenContext codegen_context = std::move(context);
int index = 0;
while (index < statements.size()){
{
std::shared_ptr<semantic_ir::SemanticStatement> statement = statements[index];
context::GenStmtResult statement_parsed = eval_stmt_with_try(statement, codegen_context, statements_parsed.next_try, gen_expr_fn);
statements_parsed = context::GenStmtsResult_append_stmt(statements_parsed, statement_parsed);
codegen_context = statement_parsed.codegen_context;
context::mutate_context_from_statement(statement, codegen_context);
index = index + 1;
}
}
return context::GenStmtsWithContext{statements_parsed, codegen_context};
}

mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept{return context::GenStmtsResult_joined_code(eval_stmts_str_with_try(statements, context, 0, gen_expr_fn).statements_parsed);}

} // namespace stmt_eval
