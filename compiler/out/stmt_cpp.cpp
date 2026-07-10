#define main mlc_user_main
#include "stmt_cpp.hpp"

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "semantic_type_structure.hpp"
#include "let_pat_cpp.hpp"
#include "statement_context.hpp"
#include "expr_visitor_cpp.hpp"

namespace stmt_cpp {

using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace print;
using namespace emit_helpers;
using namespace context;
using namespace cpp_naming;
using namespace type_gen;
using namespace semantic_type_structure;
using namespace let_pat_cpp;
using namespace statement_context;
using namespace expr_visitor_cpp;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;
mlc::String gen_stmts_for_cpp_codegen(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{
  return print_cpp_statements(gen_stmts_cpp(statements, context));
}
std::shared_ptr<cpp_ast::CppExpression> gen_expr_cpp_for_stmt_codegen(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{
  return expr_visitor_cpp::eval_expr_cpp(expression, context, gen_stmts_for_cpp_codegen);
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept{
return eval_stmts_cpp_with_try(statements, self, 0).statements;
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{
  return CodegenContext_gen_stmts_cpp(context, statements);
}
GenStmtsCppWithContext gen_stmts_cpp_with_context(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept{
  auto accumulated = eval_stmts_cpp_with_try(statements, context, try_counter);
  return GenStmtsCppWithContext{accumulated.statements, accumulated.codegen_context};
}
std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_from_string_output(mlc::String output) noexcept{
  return cpp_stmt_fragment_from_string_output(output);
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_stmts_from_string_output(mlc::String output) noexcept{
  if ((output.length() == 0))   {
    return mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  } else   {
    auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
    statements.push_back(cpp_stmt_from_string_output(output));
    return statements;
  }
}
mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
  return statements.map([=](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return print_cpp_statement_line(statement); }).join(mlc::String("", 0));
}
mlc::String print_cpp_statement_fragment_line(mlc::String fragment) noexcept{
  if (((fragment.length() >= 1) && (fragment.char_at(mlc::arith::checked_sub(fragment.length(), 1)) == mlc::String("\n", 1))))   {
    return fragment;
  } else   {
    return (fragment + mlc::String("\n", 1));
  }
}
mlc::String print_cpp_statement_default_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{
  return (print::print_statement(statement) + mlc::String("\n", 1));
}
mlc::String print_cpp_statement_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppStatementFragment& cppStatementFragment) { auto [fragment] = cppStatementFragment; return print_cpp_statement_fragment_line(fragment); },
[&](const cpp_ast::CppAutoDecl& cppAutoDecl) { auto [__0, __1] = cppAutoDecl; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppVarDecl& cppVarDecl) { auto [__0, __1, __2] = cppVarDecl; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppConstDecl& cppConstDecl) { auto [__0, __1, __2] = cppConstDecl; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppConstexprAutoDecl& cppConstexprAutoDecl) { auto [__0, __1] = cppConstexprAutoDecl; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppReturn& cppReturn) { auto [__0] = cppReturn; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppExpressionStatement& cppExpressionStatement) { auto [__0] = cppExpressionStatement; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppBlock& cppBlock) { auto [__0] = cppBlock; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppIf& cppIf) { auto [__0, __1, __2] = cppIf; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppWhile& cppWhile) { auto [__0, __1] = cppWhile; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppFor& cppFor) { auto [__0, __1, __2] = cppFor; return print_cpp_statement_default_line(statement); },
[&](const cpp_ast::CppStructuredBinding& cppStructuredBinding) { auto [__0, __1] = cppStructuredBinding; return print_cpp_statement_default_line(statement); }
}, (*statement));
}
std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_fragment_from_string_output(mlc::String output) noexcept{
  return emit_helpers::make_fragment_cpp_statement(strip_trailing_newline(output));
}
mlc::String strip_trailing_newline(mlc::String text) noexcept{
  if (((text.length() >= 1) && (text.char_at(mlc::arith::checked_sub(text.length(), 1)) == mlc::String("\n", 1))))   {
    return text.substring(0, mlc::arith::checked_sub(text.length(), 1));
  } else   {
    return text;
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_typed_hash_map_empty_cpp(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(((((mlc::String("mlc::HashMap<", 13) + mlc::to_string(key_type_cpp)) + mlc::String(", ", 2)) + mlc::to_string(value_type_cpp)) + mlc::String(">", 1))), {}});
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
std::shared_ptr<cpp_ast::CppExpression> gen_map_new_let_value_cpp(std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::shared_ptr<cpp_ast::CppExpression> fallback) noexcept{
  if (semantic_type_structure::receiver_type_is_map(value_type))   {
    auto type_arguments = map_generic_type_arguments(value_type);
    if ((type_arguments.length() == 2))     {
      return gen_typed_hash_map_empty_cpp(type_gen::sem_type_to_cpp(context, type_arguments[0]), type_gen::sem_type_to_cpp(context, type_arguments[1]));
    } else     {
      return fallback;
    }
  } else   {
    return fallback;
  }
}
GenStmtCppResult gen_for_statement_cpp(mlc::String variable_name, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept{
  auto iterator_cpp = gen_expr_cpp_for_stmt_codegen(iterator, context);
  auto body_accumulated = eval_stmts_cpp_with_try(body_statements, context, try_counter);
  return GenStmtCppResult{emit_helpers::make_for_cpp_statement(cpp_naming::cpp_safe(variable_name), iterator_cpp, body_accumulated.statements), body_accumulated.next_try, body_accumulated.codegen_context};
}
GenStmtCppResult gen_while_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept{
  auto condition_cpp = gen_expr_cpp_for_stmt_codegen(condition, context);
  auto body_accumulated = eval_stmts_cpp_with_try(body_statements, context, try_counter);
  return GenStmtCppResult{emit_helpers::make_while_cpp_statement(condition_cpp, emit_helpers::make_block_cpp_statement(body_accumulated.statements)), body_accumulated.next_try, body_accumulated.codegen_context};
}
bool if_statement_has_unit_else(std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = else_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return true;
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [__0, result_expression, __2, __3] = semanticExpressionBlock; return [&]() -> bool {
auto __match_subject = result_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return true;
}
return false;
std::abort();
}();
}
return false;
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_block_trailing_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, int try_counter) noexcept{
  auto output = block_statements;
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> {
auto __match_subject = result_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return output;
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, semantic_type, __4] = semanticExpressionIf; return [&]() {
output.push_back(gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, final_context, try_counter).statement);
return output;
}();
}
return [&]() {
output.push_back(emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(result_expression, final_context)));
return output;
}();
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block_cpp(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, int try_counter) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppStatement> {
auto __match_subject = branch_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return [&]() {
auto final_context = statement_context::stmts_final_ctx(statements, context);
auto accumulated = eval_stmts_cpp_with_try(statements, context, try_counter);
return emit_helpers::make_block_cpp_statement(append_block_trailing_cpp(accumulated.statements, result_expression, final_context, accumulated.next_try));
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, semantic_type, __4] = semanticExpressionIf; return gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, context, try_counter).statement;
}
return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(branch_expression, context))});
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, int try_counter) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppStatement> {
auto __match_subject = else_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return emit_helpers::make_block_cpp_statement({});
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, nested_else_expression, semantic_type, __4] = semanticExpressionIf; return gen_unit_if_statement_cpp(condition, then_expression, nested_else_expression, semantic_type, context, try_counter).statement;
}
return unit_if_branch_block_cpp(else_expression, context, try_counter);
std::abort();
}();
}
GenStmtCppResult gen_unit_if_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _if_semantic_type, context::CodegenContext context, int try_counter) noexcept{
  auto condition_cpp = gen_expr_cpp_for_stmt_codegen(condition, context);
  return GenStmtCppResult{emit_helpers::make_if_cpp_statement(condition_cpp, unit_if_branch_block_cpp(then_expression, context, try_counter), unit_if_else_branch_cpp(else_expression, context, try_counter)), try_counter, context};
}
mlc::String ident_name_from_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
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
bool semantic_type_is_unit(std::shared_ptr<registry::Type> semantic_type) noexcept{
  return [&]() -> bool {
auto __match_subject = semantic_type;
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
GenStmtCppResult gen_auto_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, int try_counter) noexcept{
  return GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context};
}
GenStmtCppResult gen_map_new_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept{
  return GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_map_new_let_value_cpp(value_type, context, gen_expr_cpp_for_stmt_codegen(value, context))), try_counter, context};
}
GenStmtCppResult gen_let_method_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> map_object, mlc::String method_name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept{
  if (((method_name == mlc::String("new", 3)) && (ident_name_from_semantic_expression(map_object) == mlc::String("Map", 3))))   {
    return gen_map_new_let_stmt_cpp_result(name, value, value_type, context, try_counter);
  } else   {
    return gen_auto_let_stmt_cpp_result(name, value, context, try_counter);
  }
}
GenStmtCppResult gen_let_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionQuestion; return GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), mlc::arith::checked_add(try_counter, 1), context}; },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) -> GenStmtCppResult { auto [map_object, method_name, __2, __3, __4, __5] = semanticExpressionMethod; return gen_let_method_stmt_cpp(name, map_object, method_name, value, value_type, context, try_counter); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionInt; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionStr; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionFloat; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionI64; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionU8; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionUsize; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionChar; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionBool; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) -> GenStmtCppResult { auto [__0, __1] = semanticExpressionUnit; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionIdent; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionUn; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionField; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionIndex; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionBlock; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionWhile; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionMatch; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionRecord; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionArray; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionTuple; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionLambda; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); }
}, (*value));
}
GenStmtCppResult gen_return_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{
  return GenStmtCppResult{emit_helpers::make_return_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter, context};
}
GenStmtCppResult gen_return_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionQuestion; return GenStmtCppResult{emit_helpers::make_return_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), mlc::arith::checked_add(try_counter, 1), context}; },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionInt; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionStr; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionFloat; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionI64; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionU8; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionUsize; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionChar; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionBool; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) -> GenStmtCppResult { auto [__0, __1] = semanticExpressionUnit; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionIdent; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionUn; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionField; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionIndex; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionIf; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionBlock; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionWhile; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionFor; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionMatch; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionRecord; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionArray; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionTuple; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionLambda; return gen_return_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return gen_return_stmt_cpp_default(expression, context, try_counter); }
}, (*expression));
}
GenStmtCppResult gen_expr_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{
  return GenStmtCppResult{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter, context};
}
GenStmtCppResult gen_if_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{
  if ((if_statement_has_unit_else(else_expression) || semantic_type_is_unit(semantic_type)))   {
    return gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, context, try_counter);
  } else   {
    return gen_expr_stmt_cpp_default(expression, context, try_counter);
  }
}
GenStmtCppResult gen_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionQuestion; return GenStmtCppResult{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), mlc::arith::checked_add(try_counter, 1), context}; },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) -> GenStmtCppResult { auto [condition, body_statements, __2, __3] = semanticExpressionWhile; return gen_while_statement_cpp(condition, body_statements, context, try_counter); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) -> GenStmtCppResult { auto [variable_name, iterator, body_statements, __3, __4] = semanticExpressionFor; return gen_for_statement_cpp(variable_name, iterator, body_statements, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) -> GenStmtCppResult { auto [condition, then_expression, else_expression, semantic_type, __4] = semanticExpressionIf; return gen_if_expr_stmt_cpp(condition, then_expression, else_expression, semantic_type, expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionInt; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionStr; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionFloat; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionI64; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionU8; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionUsize; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionChar; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionBool; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) -> GenStmtCppResult { auto [__0, __1] = semanticExpressionUnit; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionExtern; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionIdent; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionBin; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionUn; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionCall; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4, __5] = semanticExpressionMethod; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionField; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionIndex; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionBlock; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionMatch; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionRecord; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionRecordUpdate; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionArray; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) -> GenStmtCppResult { auto [__0, __1, __2] = semanticExpressionTuple; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) -> GenStmtCppResult { auto [__0, __1, __2, __3] = semanticExpressionLambda; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) -> GenStmtCppResult { auto [__0, __1, __2, __3, __4] = semanticExpressionWith; return gen_expr_stmt_cpp_default(expression, context, try_counter); }
}, (*expression));
}
GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticStatementBreak& semanticStatementBreak) -> GenStmtCppResult { auto [__0] = semanticStatementBreak; return GenStmtCppResult{emit_helpers::make_break_cpp_statement(), try_counter, context}; },
[&](const semantic_ir::SemanticStatementContinue& semanticStatementContinue) -> GenStmtCppResult { auto [__0] = semanticStatementContinue; return GenStmtCppResult{emit_helpers::make_continue_cpp_statement(), try_counter, context}; },
[&](const semantic_ir::SemanticStatementReturn& semanticStatementReturn) -> GenStmtCppResult { auto [expression, __1] = semanticStatementReturn; return gen_return_stmt_cpp(expression, context, try_counter); },
[&](const semantic_ir::SemanticStatementLet& semanticStatementLet) -> GenStmtCppResult { auto [name, __1, value, value_type, __4] = semanticStatementLet; return gen_let_stmt_cpp(name, value, value_type, context, try_counter); },
[&](const semantic_ir::SemanticStatementLetPattern& semanticStatementLetPattern) -> GenStmtCppResult { auto [pat, __1, value, value_type, has_else, else_body, __6] = semanticStatementLetPattern; return [&]() {
auto pattern_parsed = let_pat_cpp::gen_let_pattern_cpp_statement(pat, value, value_type, has_else, else_body, context, gen_expr_cpp_for_stmt_codegen);
return GenStmtCppResult{pattern_parsed.statement, try_counter, pattern_parsed.codegen_context};
}(); },
[&](const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst) -> GenStmtCppResult { auto [name, value, __2, __3] = semanticStatementLetConst; return GenStmtCppResult{emit_helpers::make_constexpr_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; },
[&](const semantic_ir::SemanticStatementExpr& semanticStatementExpr) -> GenStmtCppResult { auto [expression, __1] = semanticStatementExpr; return gen_expr_stmt_cpp(expression, context, try_counter); }
}, (*statement));
}
StmtsCppAccumState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept{
  auto output_statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  auto next_try = try_counter;
  auto codegen_context = context;
  auto index = 0;
  while ((index < statements.length()))   {
    auto statement = statements[index];
    auto statement_parsed = eval_stmt_cpp(statement, codegen_context, next_try);
    output_statements.push_back(statement_parsed.statement);
    (codegen_context = statement_parsed.codegen_context);
    context::mutate_context_from_statement(statement, codegen_context);
    (next_try = statement_parsed.next_try);
    (index = mlc::arith::checked_add(index, 1));
  }
  return StmtsCppAccumState{output_statements, next_try, codegen_context};
}

} // namespace stmt_cpp
