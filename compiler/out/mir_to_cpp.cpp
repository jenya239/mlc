#include "mir_to_cpp.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "mir_types.hpp"
#include "mir_ids.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"

namespace mir_to_cpp {

using namespace ast;
using namespace registry;
using namespace mir_types;
using namespace mir_ids;
using namespace cpp_ast;
using namespace emit_helpers;
using namespace ast_tokens;

std::shared_ptr<registry::Type> mir_to_cpp_unknown_type() noexcept;

mlc::String mir_cpp_safe_name(mlc::String name) noexcept;

mlc::String mir_temp_local_name(mir_types::LocalId local_id) noexcept;

mlc::String mir_type_to_cpp_string(std::shared_ptr<registry::Type> type_value) noexcept;

mir_types::MirLocal mir_find_local(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept;

mir_types::MirBlock mir_block_by_id(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept;

mlc::String mir_local_cpp_name(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept;

std::shared_ptr<cpp_ast::CppExpression> mir_operand_to_cpp_expression(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept;

std::shared_ptr<cpp_ast::CppExpression> mir_rvalue_to_cpp_expression(mir_types::MirFunction function, mir_types::MirRvalue rvalue) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> mir_call_operands_to_cpp_expressions(mir_types::MirFunction function, mlc::Array<mir_types::MirOperand> operands) noexcept;

std::shared_ptr<cpp_ast::CppStatement> mir_stmt_to_cpp_statement(mir_types::MirFunction function, mir_types::MirStmt statement) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_stmts_to_cpp_statements(mir_types::MirFunction function, mlc::Array<mir_types::MirStmt> statements) noexcept;

std::shared_ptr<cpp_ast::CppStatement> mir_return_to_cpp_statement(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_block_to_cpp_statements(mir_types::MirFunction function, mir_types::MirBlock block) noexcept;

bool mir_function_is_single_block_simple(mir_types::MirFunction function) noexcept;

bool mir_function_is_conditional_simple(mir_types::MirFunction function) noexcept;

bool mir_function_is_simple(mir_types::MirFunction function) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_single_block_to_cpp_statements(mir_types::MirFunction function) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_conditional_to_cpp_statements(mir_types::MirFunction function) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_to_cpp_statements(mir_types::MirFunction function) noexcept;

mlc::Array<mlc::String> mir_function_parameter_strings(mir_types::MirFunction function) noexcept;

ast::Result<std::shared_ptr<cpp_ast::CppDeclaration>, mlc::Array<mlc::String>> mir_function_to_cpp_declaration(mir_types::MirFunction function) noexcept;

std::shared_ptr<registry::Type> mir_to_cpp_unknown_type() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

mlc::String mir_cpp_safe_name(mlc::String name) noexcept{return name == mlc::String("") ? mlc::String("__mir_tmp") : name;}

mlc::String mir_temp_local_name(mir_types::LocalId local_id) noexcept{return mlc::String("__mir_local_") + mlc::to_string(mir_ids::mir_local_id_index(local_id));}

mlc::String mir_type_to_cpp_string(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TI32>((*type_value))) {  return mlc::String("int"); } if (std::holds_alternative<registry::TBool>((*type_value))) {  return mlc::String("bool"); } if (std::holds_alternative<registry::TUnit>((*type_value))) {  return mlc::String("void"); } return mlc::String("auto"); }();}

mir_types::MirLocal mir_find_local(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept{
int index = 0;
while (index < function.locals.size()){
{
if (mir_ids::mir_local_id_index(function.locals[index].id) == mir_ids::mir_local_id_index(local_id)){
{
return function.locals[index];
}
}
index = index + 1;
}
}
return mir_types::MirLocal{local_id, mlc::String(""), mir_to_cpp_unknown_type()};
}

mir_types::MirBlock mir_block_by_id(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept{
int index = 0;
while (index < function.blocks.size()){
{
if (mir_ids::mir_block_id_index(function.blocks[index].id) == mir_ids::mir_block_id_index(block_id)){
{
return function.blocks[index];
}
}
index = index + 1;
}
}
return function.blocks[0];
}

mlc::String mir_local_cpp_name(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept{
mir_types::MirLocal local = mir_find_local(function, local_id);
return local.name == mlc::String("") ? mir_temp_local_name(local_id) : mir_cpp_safe_name(local.name);
}

std::shared_ptr<cpp_ast::CppExpression> mir_operand_to_cpp_expression(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept{return std::visit(overloaded{
  [&](const MirOperandConstInt& miroperandconstint) -> std::shared_ptr<cpp_ast::CppExpression> { auto [value] = miroperandconstint; return emit_helpers::make_integer_cpp_expression(value); },
  [&](const MirOperandConstBool& miroperandconstbool) -> std::shared_ptr<cpp_ast::CppExpression> { auto [value] = miroperandconstbool; return emit_helpers::make_boolean_cpp_expression(value); },
  [&](const MirOperandConstStr& miroperandconststr) -> std::shared_ptr<cpp_ast::CppExpression> { auto [value] = miroperandconststr; return emit_helpers::make_string_cpp_expression(value); },
  [&](const MirOperandUnit& miroperandunit) -> std::shared_ptr<cpp_ast::CppExpression> { return emit_helpers::make_integer_cpp_expression(0); },
  [&](const MirOperandLocal& miroperandlocal) -> std::shared_ptr<cpp_ast::CppExpression> { auto [local_id] = miroperandlocal; return emit_helpers::make_identifier_cpp_expression(mir_local_cpp_name(function, local_id)); }
}, operand);}

std::shared_ptr<cpp_ast::CppExpression> mir_rvalue_to_cpp_expression(mir_types::MirFunction function, mir_types::MirRvalue rvalue) noexcept{return std::visit(overloaded{
  [&](const MirRvalueUse& mirrvalueuse) -> std::shared_ptr<cpp_ast::CppExpression> { auto [operand] = mirrvalueuse; return mir_operand_to_cpp_expression(function, operand); },
  [&](const MirRvalueBinary& mirrvaluebinary) -> std::shared_ptr<cpp_ast::CppExpression> { auto [operation, left, right] = mirrvaluebinary; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary(operation, mir_operand_to_cpp_expression(function, left), mir_operand_to_cpp_expression(function, right))); }
}, rvalue);}

mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> mir_call_operands_to_cpp_expressions(mir_types::MirFunction function, mlc::Array<mir_types::MirOperand> operands) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions = {};
int index = 0;
while (index < operands.size()){
{
expressions.push_back(mir_operand_to_cpp_expression(function, operands[index]));
index = index + 1;
}
}
return expressions;
}

std::shared_ptr<cpp_ast::CppStatement> mir_stmt_to_cpp_statement(mir_types::MirFunction function, mir_types::MirStmt statement) noexcept{return std::visit(overloaded{
  [&](const MirAssign& mirassign) -> std::shared_ptr<cpp_ast::CppStatement> { auto [local_id, rvalue] = mirassign; return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { 
  mir_types::MirLocal local = mir_find_local(function, local_id);
  std::shared_ptr<cpp_ast::CppExpression> initializer = mir_rvalue_to_cpp_expression(function, rvalue);
  return local.name == mlc::String("") ? emit_helpers::make_auto_cpp_statement(mir_temp_local_name(local_id), initializer) : emit_helpers::make_variable_cpp_statement(mir_cpp_safe_name(local.name), std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(mir_type_to_cpp_string(local.type_value))), initializer);
 }(); },
  [&](const MirCallAssign& mircallassign) -> std::shared_ptr<cpp_ast::CppStatement> { auto [local_id, callee_name, arguments] = mircallassign; return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { 
  std::shared_ptr<cpp_ast::CppExpression> call_expression = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall(emit_helpers::make_identifier_cpp_expression(callee_name), mir_call_operands_to_cpp_expressions(function, arguments)));
  return emit_helpers::make_auto_cpp_statement(mir_local_cpp_name(function, local_id), call_expression);
 }(); }
}, statement);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_stmts_to_cpp_statements(mir_types::MirFunction function, mlc::Array<mir_types::MirStmt> statements) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> output = {};
int index = 0;
while (index < statements.size()){
{
output.push_back(mir_stmt_to_cpp_statement(function, statements[index]));
index = index + 1;
}
}
return output;
}

std::shared_ptr<cpp_ast::CppStatement> mir_return_to_cpp_statement(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppStatement> { if (std::holds_alternative<mir_types::MirOperandUnit>(operand)) {  return emit_helpers::make_fragment_cpp_statement(mlc::String("return;")); } return emit_helpers::make_return_cpp_statement(mir_operand_to_cpp_expression(function, operand)); }();}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_block_to_cpp_statements(mir_types::MirFunction function, mir_types::MirBlock block) noexcept{return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { if (std::holds_alternative<mir_types::MirReturn>(block.term)) { auto _v_mirreturn = std::get<mir_types::MirReturn>(block.term); auto [operand] = _v_mirreturn; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = mir_stmts_to_cpp_statements(function, block.stmts);
  statements.push_back(mir_return_to_cpp_statement(function, operand));
  return statements;
 }(); } return mir_stmts_to_cpp_statements(function, block.stmts); }();}

bool mir_function_is_single_block_simple(mir_types::MirFunction function) noexcept{
return function.blocks.size() != 1 ? false : [&]() { if (std::holds_alternative<mir_types::MirReturn>(function.blocks[0].term)) { auto _v_mirreturn = std::get<mir_types::MirReturn>(function.blocks[0].term); auto [_w0] = _v_mirreturn; return true; } return false; }();
}

bool mir_function_is_conditional_simple(mir_types::MirFunction function) noexcept{
return function.blocks.size() != 3 ? false : [&]() { if (std::holds_alternative<mir_types::MirCondJump>(function.blocks[0].term)) { auto _v_mircondjump = std::get<mir_types::MirCondJump>(function.blocks[0].term); auto [_w0, then_block, else_block] = _v_mircondjump; return [&]() { if (std::holds_alternative<mir_types::MirReturn>(mir_block_by_id(function, then_block).term)) { auto _v_mirreturn = std::get<mir_types::MirReturn>(mir_block_by_id(function, then_block).term); auto [_w0] = _v_mirreturn; return [&]() { if (std::holds_alternative<mir_types::MirReturn>(mir_block_by_id(function, else_block).term)) { auto _v_mirreturn = std::get<mir_types::MirReturn>(mir_block_by_id(function, else_block).term); auto [_w0] = _v_mirreturn; return true; } return false; }(); } return false; }(); } return false; }();
}

bool mir_function_is_simple(mir_types::MirFunction function) noexcept{return mir_function_is_single_block_simple(function) || mir_function_is_conditional_simple(function);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_single_block_to_cpp_statements(mir_types::MirFunction function) noexcept{return mir_block_to_cpp_statements(function, function.blocks[0]);}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_conditional_to_cpp_statements(mir_types::MirFunction function) noexcept{
mir_types::MirBlock entry_block = function.blocks[0];
return std::visit(overloaded{
  [&](const MirCondJump& mircondjump) -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { auto [condition_operand, then_block_id, else_block_id] = mircondjump; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { 
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> then_statements = mir_block_to_cpp_statements(function, mir_block_by_id(function, then_block_id));
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> else_statements = mir_block_to_cpp_statements(function, mir_block_by_id(function, else_block_id));
  mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements = {};
  statements.push_back(emit_helpers::make_if_cpp_statement(mir_operand_to_cpp_expression(function, condition_operand), emit_helpers::make_block_cpp_statement(then_statements), emit_helpers::make_block_cpp_statement(else_statements)));
  return statements;
 }(); },
  [&](const MirReturn& mirreturn) -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { auto [_w0] = mirreturn; return {}; },
  [&](const MirJump& mirjump) -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { auto [_w0] = mirjump; return {}; },
  [&](const MirUnreachable& mirunreachable) -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> { return {}; }
}, entry_block.term);
}

mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_to_cpp_statements(mir_types::MirFunction function) noexcept{return mir_function_is_single_block_simple(function) ? mir_function_single_block_to_cpp_statements(function) : mir_function_conditional_to_cpp_statements(function);}

mlc::Array<mlc::String> mir_function_parameter_strings(mir_types::MirFunction function) noexcept{
mlc::Array<mlc::String> parameters = {};
int index = 0;
while (index < function.params.size()){
{
mir_types::MirParam parameter = function.params[index];
parameters.push_back(mir_type_to_cpp_string(parameter.type_value) + mlc::String(" ") + mir_cpp_safe_name(parameter.name));
index = index + 1;
}
}
return parameters;
}

ast::Result<std::shared_ptr<cpp_ast::CppDeclaration>, mlc::Array<mlc::String>> mir_function_to_cpp_declaration(mir_types::MirFunction function) noexcept{return !mir_function_is_simple(function) ? ast::Result<std::shared_ptr<cpp_ast::CppDeclaration>, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("mir to cpp: function is not simple")})) : ast::Result<std::shared_ptr<cpp_ast::CppDeclaration>, mlc::Array<mlc::String>>(ast::Ok<std::shared_ptr<cpp_ast::CppDeclaration>>(std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef(mlc::String(""), mir_type_to_cpp_string(function.return_type), mir_cpp_safe_name(function.name), mir_function_parameter_strings(function), mir_function_to_cpp_statements(function), 1))));}

} // namespace mir_to_cpp
