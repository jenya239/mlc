#define main mlc_user_main
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

std::shared_ptr<registry::Type> mir_to_cpp_unknown_type() noexcept{
  return std::make_shared<registry::Type>(registry::TUnknown{});
}
mlc::String mir_cpp_safe_name(mlc::String name) noexcept{
  if ((name == mlc::String("", 0)))   {
    return mlc::String("__mir_tmp", 9);
  } else   {
    return name;
  }
}
mlc::String mir_temp_local_name(mir_types::LocalId local_id) noexcept{
  return ((mlc::String("__mir_local_", 12) + mlc::to_string(mir_ids::mir_local_id_index(local_id))) + mlc::String("", 0));
}
mlc::String mir_type_to_cpp_string(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return mlc::String("int", 3);
}
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return mlc::String("bool", 4);
}
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return mlc::String("void", 4);
}
return mlc::String("auto", 4);
std::abort();
}();
}
mir_types::MirLocal mir_find_local(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept{
  auto index = 0;
  while ((index < function.locals.length()))   {
    if ((mir_ids::mir_local_id_index(function.locals[index].id) == mir_ids::mir_local_id_index(local_id)))     {
      return function.locals[index];
    }
    (index = (index + 1));
  }
  return mir_types::MirLocal{local_id, mlc::String("", 0), mir_to_cpp_unknown_type()};
}
mir_types::MirBlock mir_block_by_id(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept{
  auto index = 0;
  while ((index < function.blocks.length()))   {
    if ((mir_ids::mir_block_id_index(function.blocks[index].id) == mir_ids::mir_block_id_index(block_id)))     {
      return function.blocks[index];
    }
    (index = (index + 1));
  }
  return function.blocks[0];
}
mlc::String mir_local_cpp_name(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept{
  auto local = mir_find_local(function, local_id);
  if ((local.name == mlc::String("", 0)))   {
    return mir_temp_local_name(local_id);
  } else   {
    return mir_cpp_safe_name(local.name);
  }
}
std::shared_ptr<cpp_ast::CppExpression> mir_operand_to_cpp_expression(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirOperandConstInt& mirOperandConstInt) { auto [value] = mirOperandConstInt; return emit_helpers::make_integer_cpp_expression(value); },
[&](const mir_types::MirOperandConstBool& mirOperandConstBool) { auto [value] = mirOperandConstBool; return emit_helpers::make_boolean_cpp_expression(value); },
[&](const mir_types::MirOperandConstStr& mirOperandConstStr) { auto [value] = mirOperandConstStr; return emit_helpers::make_string_cpp_expression(value); },
[&](const mir_types::MirOperandUnit& mirOperandUnit) { return emit_helpers::make_integer_cpp_expression(0); },
[&](const mir_types::MirOperandLocal& mirOperandLocal) { auto [local_id] = mirOperandLocal; return emit_helpers::make_identifier_cpp_expression(mir_local_cpp_name(function, local_id)); }
}, operand);
}
std::shared_ptr<cpp_ast::CppExpression> mir_rvalue_to_cpp_expression(mir_types::MirFunction function, mir_types::MirRvalue rvalue) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirRvalueUse& mirRvalueUse) { auto [operand] = mirRvalueUse; return mir_operand_to_cpp_expression(function, operand); },
[&](const mir_types::MirRvalueBinary& mirRvalueBinary) { auto [operation, left, right] = mirRvalueBinary; return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{operation, mir_operand_to_cpp_expression(function, left), mir_operand_to_cpp_expression(function, right)}); }
}, rvalue);
}
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> mir_call_operands_to_cpp_expressions(mir_types::MirFunction function, mlc::Array<mir_types::MirOperand> operands) noexcept{
  auto expressions = mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{};
  auto index = 0;
  while ((index < operands.length()))   {
    expressions.push_back(mir_operand_to_cpp_expression(function, operands[index]));
    (index = (index + 1));
  }
  return expressions;
}
std::shared_ptr<cpp_ast::CppStatement> mir_stmt_to_cpp_statement(mir_types::MirFunction function, mir_types::MirStmt statement) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirAssign& mirAssign) { auto [local_id, rvalue] = mirAssign; return [&]() {
auto local = mir_find_local(function, local_id);
auto initializer = mir_rvalue_to_cpp_expression(function, rvalue);
return ((local.name == mlc::String("", 0)) ? (emit_helpers::make_auto_cpp_statement(mir_temp_local_name(local_id), initializer)) : (emit_helpers::make_variable_cpp_statement(mir_cpp_safe_name(local.name), std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mir_type_to_cpp_string(local.type_value)}), initializer)));
}(); },
[&](const mir_types::MirCallAssign& mirCallAssign) { auto [local_id, callee_name, arguments] = mirCallAssign; return [&]() {
auto call_expression = std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(callee_name), mir_call_operands_to_cpp_expressions(function, arguments)});
return emit_helpers::make_auto_cpp_statement(mir_local_cpp_name(function, local_id), call_expression);
}(); }
}, statement);
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_stmts_to_cpp_statements(mir_types::MirFunction function, mlc::Array<mir_types::MirStmt> statements) noexcept{
  auto output = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
  auto index = 0;
  while ((index < statements.length()))   {
    output.push_back(mir_stmt_to_cpp_statement(function, statements[index]));
    (index = (index + 1));
  }
  return output;
}
std::shared_ptr<cpp_ast::CppStatement> mir_return_to_cpp_statement(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppStatement> {
auto __match_subject = operand;
if (std::holds_alternative<mir_types::MirOperandUnit>(__match_subject)) {
const mir_types::MirOperandUnit& mirOperandUnit = std::get<mir_types::MirOperandUnit>(__match_subject);
return emit_helpers::make_fragment_cpp_statement(mlc::String("return;", 7));
}
return emit_helpers::make_return_cpp_statement(mir_operand_to_cpp_expression(function, operand));
std::abort();
}();
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_block_to_cpp_statements(mir_types::MirFunction function, mir_types::MirBlock block) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> {
auto __match_subject = block.term;
if (std::holds_alternative<mir_types::MirReturn>(__match_subject)) {
const mir_types::MirReturn& mirReturn = std::get<mir_types::MirReturn>(__match_subject);
auto [operand] = mirReturn; return [&]() {
auto statements = mir_stmts_to_cpp_statements(function, block.stmts);
statements.push_back(mir_return_to_cpp_statement(function, operand));
return statements;
}();
}
return mir_stmts_to_cpp_statements(function, block.stmts);
std::abort();
}();
}
bool mir_function_is_single_block_simple(mir_types::MirFunction function) noexcept{
  if ((function.blocks.length() != 1))   {
    return false;
  } else   {
    return [&]() -> bool {
auto __match_subject = function.blocks[0].term;
if (std::holds_alternative<mir_types::MirReturn>(__match_subject)) {
const mir_types::MirReturn& mirReturn = std::get<mir_types::MirReturn>(__match_subject);
auto [__0] = mirReturn; return true;
}
return false;
std::abort();
}();
  }
}
bool mir_function_is_conditional_simple(mir_types::MirFunction function) noexcept{
  if ((function.blocks.length() != 3))   {
    return false;
  } else   {
    return [&]() -> bool {
auto __match_subject = function.blocks[0].term;
if (std::holds_alternative<mir_types::MirCondJump>(__match_subject)) {
const mir_types::MirCondJump& mirCondJump = std::get<mir_types::MirCondJump>(__match_subject);
auto [__0, then_block, else_block] = mirCondJump; return [&]() -> bool {
auto __match_subject = mir_block_by_id(function, then_block).term;
if (std::holds_alternative<mir_types::MirReturn>(__match_subject)) {
const mir_types::MirReturn& mirReturn = std::get<mir_types::MirReturn>(__match_subject);
auto [__0] = mirReturn; return [&]() -> bool {
auto __match_subject = mir_block_by_id(function, else_block).term;
if (std::holds_alternative<mir_types::MirReturn>(__match_subject)) {
const mir_types::MirReturn& mirReturn = std::get<mir_types::MirReturn>(__match_subject);
auto [__0] = mirReturn; return true;
}
return false;
std::abort();
}();
}
return false;
std::abort();
}();
}
return false;
std::abort();
}();
  }
}
bool mir_function_is_simple(mir_types::MirFunction function) noexcept{
  return (mir_function_is_single_block_simple(function) || mir_function_is_conditional_simple(function));
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_single_block_to_cpp_statements(mir_types::MirFunction function) noexcept{
  return mir_block_to_cpp_statements(function, function.blocks[0]);
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_conditional_to_cpp_statements(mir_types::MirFunction function) noexcept{
  auto entry_block = function.blocks[0];
  return std::visit(overloaded{[&](const mir_types::MirCondJump& mirCondJump) { auto [condition_operand, then_block_id, else_block_id] = mirCondJump; return [&]() {
auto then_statements = mir_block_to_cpp_statements(function, mir_block_by_id(function, then_block_id));
auto else_statements = mir_block_to_cpp_statements(function, mir_block_by_id(function, else_block_id));
auto statements = mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{};
statements.push_back(emit_helpers::make_if_cpp_statement(mir_operand_to_cpp_expression(function, condition_operand), emit_helpers::make_block_cpp_statement(then_statements), emit_helpers::make_block_cpp_statement(else_statements)));
return statements;
}(); },
[&](const mir_types::MirReturn& mirReturn) { auto [__0] = mirReturn; return mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{}; },
[&](const mir_types::MirJump& mirJump) { auto [__0] = mirJump; return mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{}; },
[&](const mir_types::MirUnreachable& mirUnreachable) { return mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{}; }
}, entry_block.term);
}
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_to_cpp_statements(mir_types::MirFunction function) noexcept{
  if (mir_function_is_single_block_simple(function))   {
    return mir_function_single_block_to_cpp_statements(function);
  } else   {
    return mir_function_conditional_to_cpp_statements(function);
  }
}
mlc::Array<mlc::String> mir_function_parameter_strings(mir_types::MirFunction function) noexcept{
  auto parameters = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < function.params.length()))   {
    auto parameter = function.params[index];
    parameters.push_back(((((mlc::String("", 0) + mlc::to_string(mir_type_to_cpp_string(parameter.type_value))) + mlc::String(" ", 1)) + mlc::to_string(mir_cpp_safe_name(parameter.name))) + mlc::String("", 0)));
    (index = (index + 1));
  }
  return parameters;
}
ast::Result<std::shared_ptr<cpp_ast::CppDeclaration>, mlc::Array<mlc::String>> mir_function_to_cpp_declaration(mir_types::MirFunction function) noexcept{
  if ((!mir_function_is_simple(function)))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("mir to cpp: function is not simple", 34)}};
  } else   {
    return ast::Ok<std::shared_ptr<cpp_ast::CppDeclaration>>{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnDef{mlc::String("", 0), mir_type_to_cpp_string(function.return_type), mir_cpp_safe_name(function.name), mir_function_parameter_strings(function), mir_function_to_cpp_statements(function), 1})};
  }
}

} // namespace mir_to_cpp
