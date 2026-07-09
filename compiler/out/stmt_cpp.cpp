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

struct StmtsCppAccumState {mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements;int next_try;context::CodegenContext codegen_context;};

struct GenStmtCppResult {std::shared_ptr<cpp_ast::CppStatement> statement;int next_try;context::CodegenContext codegen_context;};

mlc::String gen_stmts_for_cpp_codegen(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_expr_cpp_for_stmt_codegen(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

stmt_cpp::GenStmtsCppWithContext gen_stmts_cpp_with_context(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_stmts_from_string_output(mlc::String output) noexcept;

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;

mlc::String print_cpp_statement_fragment_line(mlc::String fragment) noexcept;

mlc::String print_cpp_statement_default_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

mlc::String print_cpp_statement_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_fragment_from_string_output(mlc::String output) noexcept;

mlc::String strip_trailing_newline(mlc::String text) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_typed_hash_map_empty_cpp(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> map_generic_type_arguments(std::shared_ptr<registry::Type> value_type) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_map_new_let_value_cpp(std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::shared_ptr<cpp_ast::CppExpression> fallback) noexcept;

stmt_cpp::GenStmtCppResult gen_for_statement_cpp(mlc::String variable_name, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_while_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept;

bool if_statement_has_unit_else(std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_block_trailing_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, int try_counter) noexcept;

std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block_cpp(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, int try_counter) noexcept;

std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_unit_if_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _if_semantic_type, context::CodegenContext context, int try_counter) noexcept;

mlc::String ident_name_from_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

bool semantic_type_is_unit(std::shared_ptr<registry::Type> semantic_type) noexcept;

stmt_cpp::GenStmtCppResult gen_auto_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_map_new_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_let_method_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> map_object, mlc::String method_name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_let_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_return_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_return_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_expr_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_if_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult gen_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept;

stmt_cpp::StmtsCppAccumState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmts_for_cpp_codegen(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{return print_cpp_statements(gen_stmts_cpp(statements, context));}

std::shared_ptr<cpp_ast::CppExpression> gen_expr_cpp_for_stmt_codegen(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{return expr_visitor_cpp::eval_expr_cpp(expression, context, gen_stmts_for_cpp_codegen);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> CodegenContext_gen_stmts_cpp(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept{return eval_stmts_cpp_with_try(statements, self, 0).statements;}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_stmts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept{return CodegenContext_gen_stmts_cpp(context, statements);}

stmt_cpp::GenStmtsCppWithContext gen_stmts_cpp_with_context(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept{
stmt_cpp::StmtsCppAccumState accumulated = eval_stmts_cpp_with_try(statements, context, try_counter);
return stmt_cpp::GenStmtsCppWithContext{accumulated.statements, accumulated.codegen_context};
}

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_from_string_output(mlc::String output) noexcept{return cpp_stmt_fragment_from_string_output(output);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> cpp_stmts_from_string_output(mlc::String output) noexcept{return output.length() == 0 ? mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{} : [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  statements.push_back(cpp_stmt_from_string_output(output));
  return statements;
 }();}

mlc::String print_cpp_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{return statements.map([](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return print_cpp_statement_line(statement); }).join(mlc::String(""));}

mlc::String print_cpp_statement_fragment_line(mlc::String fragment) noexcept{return fragment.length() >= 1 && fragment.char_at(fragment.length() - 1) == mlc::String("\n") ? fragment : fragment + mlc::String("\n");}

mlc::String print_cpp_statement_default_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return print::print_statement(statement) + mlc::String("\n");}

mlc::String print_cpp_statement_line(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return std::visit(overloaded{
  [&](const CppStatementFragment& cppstatementfragment) -> mlc::String { auto [fragment] = cppstatementfragment; return print_cpp_statement_fragment_line(fragment); },
  [&](const CppAutoDecl& cppautodecl) -> mlc::String { auto [_w0, _w1] = cppautodecl; return print_cpp_statement_default_line(statement); },
  [&](const CppVarDecl& cppvardecl) -> mlc::String { auto [_w0, _w1, _w2] = cppvardecl; return print_cpp_statement_default_line(statement); },
  [&](const CppConstDecl& cppconstdecl) -> mlc::String { auto [_w0, _w1, _w2] = cppconstdecl; return print_cpp_statement_default_line(statement); },
  [&](const CppConstexprAutoDecl& cppconstexprautodecl) -> mlc::String { auto [_w0, _w1] = cppconstexprautodecl; return print_cpp_statement_default_line(statement); },
  [&](const CppReturn& cppreturn) -> mlc::String { auto [_w0] = cppreturn; return print_cpp_statement_default_line(statement); },
  [&](const CppExpressionStatement& cppexpressionstatement) -> mlc::String { auto [_w0] = cppexpressionstatement; return print_cpp_statement_default_line(statement); },
  [&](const CppBlock& cppblock) -> mlc::String { auto [_w0] = cppblock; return print_cpp_statement_default_line(statement); },
  [&](const CppIf& cppif) -> mlc::String { auto [_w0, _w1, _w2] = cppif; return print_cpp_statement_default_line(statement); },
  [&](const CppWhile& cppwhile) -> mlc::String { auto [_w0, _w1] = cppwhile; return print_cpp_statement_default_line(statement); },
  [&](const CppFor& cppfor) -> mlc::String { auto [_w0, _w1, _w2] = cppfor; return print_cpp_statement_default_line(statement); },
  [&](const CppStructuredBinding& cppstructuredbinding) -> mlc::String { auto [_w0, _w1] = cppstructuredbinding; return print_cpp_statement_default_line(statement); }
}, (*statement)._);}

std::shared_ptr<cpp_ast::CppStatement> cpp_stmt_fragment_from_string_output(mlc::String output) noexcept{return emit_helpers::make_fragment_cpp_statement(strip_trailing_newline(output));}

mlc::String strip_trailing_newline(mlc::String text) noexcept{return text.length() >= 1 && text.char_at(text.length() - 1) == mlc::String("\n") ? text.substring(0, text.length() - 1) : text;}

std::shared_ptr<cpp_ast::CppExpression> gen_typed_hash_map_empty_cpp(mlc::String key_type_cpp, mlc::String value_type_cpp) noexcept{return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(mlc::String("mlc::HashMap<") + key_type_cpp + mlc::String(", ") + value_type_cpp + mlc::String(">")), {}));}

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

std::shared_ptr<cpp_ast::CppExpression> gen_map_new_let_value_cpp(std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::shared_ptr<cpp_ast::CppExpression> fallback) noexcept{
return semantic_type_structure::receiver_type_is_map(value_type) ? [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::Array<std::shared_ptr<registry::Type>> type_arguments = map_generic_type_arguments(value_type);
  return type_arguments.size() == 2 ? gen_typed_hash_map_empty_cpp(type_gen::sem_type_to_cpp(context, type_arguments[0]), type_gen::sem_type_to_cpp(context, type_arguments[1])) : fallback;
 }() : fallback;
}

stmt_cpp::GenStmtCppResult gen_for_statement_cpp(mlc::String variable_name, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept{
std::shared_ptr<cpp_ast::CppExpression> iterator_cpp = gen_expr_cpp_for_stmt_codegen(iterator, context);
stmt_cpp::StmtsCppAccumState body_accumulated = eval_stmts_cpp_with_try(body_statements, context, try_counter);
return stmt_cpp::GenStmtCppResult{emit_helpers::make_for_cpp_statement(cpp_naming::cpp_safe(variable_name), iterator_cpp, body_accumulated.statements), body_accumulated.next_try, body_accumulated.codegen_context};
}

stmt_cpp::GenStmtCppResult gen_while_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> body_statements, context::CodegenContext context, int try_counter) noexcept{
std::shared_ptr<cpp_ast::CppExpression> condition_cpp = gen_expr_cpp_for_stmt_codegen(condition, context);
stmt_cpp::StmtsCppAccumState body_accumulated = eval_stmts_cpp_with_try(body_statements, context, try_counter);
return stmt_cpp::GenStmtCppResult{emit_helpers::make_while_cpp_statement(condition_cpp, emit_helpers::make_block_cpp_statement(body_accumulated.statements)), body_accumulated.next_try, body_accumulated.codegen_context};
}

bool if_statement_has_unit_else(std::shared_ptr<semantic_ir::SemanticExpression> else_expression) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return true; } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*else_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*else_expression)._); auto [_w0, result_expression, _w1, _w2] = _v_semanticexpressionblock; return [&]() { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return true; } return false; }(); } return false; }();}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_block_trailing_cpp(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, int try_counter) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output = block_statements;
return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*result_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*result_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return output; } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*result_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*result_expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  output.push_back(gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, final_context, try_counter).statement);
  return output;
 }(); } return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  output.push_back(emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(result_expression, final_context)));
  return output;
 }(); }();
}

std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block_cpp(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, int try_counter) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*branch_expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*branch_expression)._); auto [statements, result_expression, _w0, _w1] = _v_semanticexpressionblock; return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { 
  context::CodegenContext final_context = statement_context::stmts_final_ctx(statements, context);
  stmt_cpp::StmtsCppAccumState accumulated = eval_stmts_cpp_with_try(statements, context, try_counter);
  return emit_helpers::make_block_cpp_statement(append_block_trailing_cpp(accumulated.statements, result_expression, final_context, accumulated.next_try));
 }(); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*branch_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*branch_expression)._); auto [condition, then_expression, else_expression, semantic_type, _w0] = _v_semanticexpressionif; return gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, context, try_counter).statement; } return emit_helpers::make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(branch_expression, context))}); }();}

std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch_cpp(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, int try_counter) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*else_expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*else_expression)._); auto [_w0, _w1] = _v_semanticexpressionunit; return emit_helpers::make_block_cpp_statement({}); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*else_expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*else_expression)._); auto [condition, then_expression, nested_else_expression, semantic_type, _w0] = _v_semanticexpressionif; return gen_unit_if_statement_cpp(condition, then_expression, nested_else_expression, semantic_type, context, try_counter).statement; } return unit_if_branch_block_cpp(else_expression, context, try_counter); }();}

stmt_cpp::GenStmtCppResult gen_unit_if_statement_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _if_semantic_type, context::CodegenContext context, int try_counter) noexcept{
std::shared_ptr<cpp_ast::CppExpression> condition_cpp = gen_expr_cpp_for_stmt_codegen(condition, context);
return stmt_cpp::GenStmtCppResult{emit_helpers::make_if_cpp_statement(condition_cpp, unit_if_branch_block_cpp(then_expression, context, try_counter), unit_if_else_branch_cpp(else_expression, context, try_counter)), try_counter, context};
}

mlc::String ident_name_from_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return std::visit(overloaded{
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

bool semantic_type_is_unit(std::shared_ptr<registry::Type> semantic_type) noexcept{return [&]() { if (std::holds_alternative<registry::TUnit>((*semantic_type))) {  return true; } return false; }();}

stmt_cpp::GenStmtCppResult gen_auto_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, int try_counter) noexcept{return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context};}

stmt_cpp::GenStmtCppResult gen_map_new_let_stmt_cpp_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept{return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_map_new_let_value_cpp(value_type, context, gen_expr_cpp_for_stmt_codegen(value, context))), try_counter, context};}

stmt_cpp::GenStmtCppResult gen_let_method_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> map_object, mlc::String method_name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept{return method_name == mlc::String("new") && ident_name_from_semantic_expression(map_object) == mlc::String("Map") ? gen_map_new_let_stmt_cpp_result(name, value, value_type, context, try_counter) : gen_auto_let_stmt_cpp_result(name, value, context, try_counter);}

stmt_cpp::GenStmtCppResult gen_let_stmt_cpp(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionquestion; return stmt_cpp::GenStmtCppResult{emit_helpers::make_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter + 1, context}; },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> stmt_cpp::GenStmtCppResult { auto [map_object, method_name, _w0, _w1, _w2, _w3] = semanticexpressionmethod; return gen_let_method_stmt_cpp(name, map_object, method_name, value, value_type, context, try_counter); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionint; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionstr; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionfloat; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressioni64; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionu8; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionusize; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionchar; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionbool; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1] = semanticexpressionunit; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1] = semanticexpressionextern; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionident; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionarray; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressiontuple; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return gen_auto_let_stmt_cpp_result(name, value, context, try_counter); }
}, (*value)._);}

stmt_cpp::GenStmtCppResult gen_return_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{return stmt_cpp::GenStmtCppResult{emit_helpers::make_return_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter, context};}

stmt_cpp::GenStmtCppResult gen_return_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionquestion; return stmt_cpp::GenStmtCppResult{emit_helpers::make_return_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter + 1, context}; },
  [&](const SemanticExpressionInt& semanticexpressionint) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionint; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionstr; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionfloat; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressioni64; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionu8; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionusize; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionchar; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionbool; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1] = semanticexpressionunit; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1] = semanticexpressionextern; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionident; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionarray; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressiontuple; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return gen_return_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return gen_return_stmt_cpp_default(expression, context, try_counter); }
}, (*expression)._);}

stmt_cpp::GenStmtCppResult gen_expr_stmt_cpp_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{return stmt_cpp::GenStmtCppResult{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter, context};}

stmt_cpp::GenStmtCppResult gen_if_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{return if_statement_has_unit_else(else_expression) || semantic_type_is_unit(semantic_type) ? gen_unit_if_statement_cpp(condition, then_expression, else_expression, semantic_type, context, try_counter) : gen_expr_stmt_cpp_default(expression, context, try_counter);}

stmt_cpp::GenStmtCppResult gen_expr_stmt_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionquestion; return stmt_cpp::GenStmtCppResult{emit_helpers::make_expression_cpp_statement(gen_expr_cpp_for_stmt_codegen(expression, context)), try_counter + 1, context}; },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> stmt_cpp::GenStmtCppResult { auto [condition, body_statements, _w0, _w1] = semanticexpressionwhile; return gen_while_statement_cpp(condition, body_statements, context, try_counter); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> stmt_cpp::GenStmtCppResult { auto [variable_name, iterator, body_statements, _w0, _w1] = semanticexpressionfor; return gen_for_statement_cpp(variable_name, iterator, body_statements, context, try_counter); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> stmt_cpp::GenStmtCppResult { auto [condition, then_expression, else_expression, semantic_type, _w0] = semanticexpressionif; return gen_if_expr_stmt_cpp(condition, then_expression, else_expression, semantic_type, expression, context, try_counter); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionint; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionstr; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionfloat; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressioni64; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionu8; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionusize; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionchar; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionbool; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1] = semanticexpressionunit; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1] = semanticexpressionextern; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionident; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressionarray; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2] = semanticexpressiontuple; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return gen_expr_stmt_cpp_default(expression, context, try_counter); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> stmt_cpp::GenStmtCppResult { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return gen_expr_stmt_cpp_default(expression, context, try_counter); }
}, (*expression)._);}

stmt_cpp::GenStmtCppResult eval_stmt_cpp(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const SemanticStatementBreak& semanticstatementbreak) -> stmt_cpp::GenStmtCppResult { auto [_w0] = semanticstatementbreak; return stmt_cpp::GenStmtCppResult{emit_helpers::make_break_cpp_statement(), try_counter, context}; },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> stmt_cpp::GenStmtCppResult { auto [_w0] = semanticstatementcontinue; return stmt_cpp::GenStmtCppResult{emit_helpers::make_continue_cpp_statement(), try_counter, context}; },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> stmt_cpp::GenStmtCppResult { auto [expression, _w0] = semanticstatementreturn; return gen_return_stmt_cpp(expression, context, try_counter); },
  [&](const SemanticStatementLet& semanticstatementlet) -> stmt_cpp::GenStmtCppResult { auto [name, _w0, value, value_type, _w1] = semanticstatementlet; return gen_let_stmt_cpp(name, value, value_type, context, try_counter); },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> stmt_cpp::GenStmtCppResult { auto [pat, _w0, value, value_type, has_else, else_body, _w1] = semanticstatementletpattern; return [&]() -> stmt_cpp::GenStmtCppResult { 
  let_pat_cpp::GenLetPatternCppResult pattern_parsed = let_pat_cpp::gen_let_pattern_cpp_statement(pat, value, value_type, has_else, else_body, context, gen_expr_cpp_for_stmt_codegen);
  return stmt_cpp::GenStmtCppResult{pattern_parsed.statement, try_counter, pattern_parsed.codegen_context};
 }(); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> stmt_cpp::GenStmtCppResult { auto [name, value, _w0, _w1] = semanticstatementletconst; return stmt_cpp::GenStmtCppResult{emit_helpers::make_constexpr_auto_cpp_statement(cpp_naming::cpp_safe(name), gen_expr_cpp_for_stmt_codegen(value, context)), try_counter, context}; },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> stmt_cpp::GenStmtCppResult { auto [expression, _w0] = semanticstatementexpr; return gen_expr_stmt_cpp(expression, context, try_counter); }
}, (*statement)._);}

stmt_cpp::StmtsCppAccumState eval_stmts_cpp_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output_statements = {};
int next_try = try_counter;
context::CodegenContext codegen_context = std::move(context);
int index = 0;
while (index < statements.size()){
{
std::shared_ptr<semantic_ir::SemanticStatement> statement = statements[index];
stmt_cpp::GenStmtCppResult statement_parsed = eval_stmt_cpp(statement, codegen_context, next_try);
output_statements.push_back(statement_parsed.statement);
codegen_context = statement_parsed.codegen_context;
context::mutate_context_from_statement(statement, codegen_context);
next_try = statement_parsed.next_try;
index = index + 1;
}
}
return stmt_cpp::StmtsCppAccumState{output_statements, next_try, codegen_context};
}

} // namespace stmt_cpp
