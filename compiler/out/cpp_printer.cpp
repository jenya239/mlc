#include "cpp_printer.hpp"

#include "cpp_ast.hpp"
#include "cpp_ast.hpp"

namespace cpp_printer {

using namespace cpp_ast;
using namespace cpp_ast;

mlc::String escape_cpp_string_content(mlc::String input) noexcept;

mlc::String print_comma_separated_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpr>> expressions) noexcept;

mlc::String print_comma_separated_types(mlc::Array<std::shared_ptr<cpp_ast::CppType>> types) noexcept;

mlc::String print_comma_separated_strings(mlc::Array<mlc::String> strings) noexcept;

mlc::String type_template(mlc::String name, mlc::String arguments_code) noexcept;

mlc::String type_reference(mlc::String inner_code) noexcept;

mlc::String print_type(std::shared_ptr<cpp_ast::CppType> type_node) noexcept;

mlc::String render_ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept;

mlc::String render_parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept;

mlc::String render_parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept;

mlc::String render_index_subscript(mlc::String object_code, mlc::String index_code) noexcept;

mlc::String render_function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept;

mlc::String pointer_field_access(mlc::String object_code, mlc::String field_name) noexcept;

mlc::String value_field_access(mlc::String object_code, mlc::String field_name) noexcept;

mlc::String init_list_braces(mlc::String element_list) noexcept;

mlc::String std_visit_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept;

mlc::String cast_expression(mlc::String kind_prefix, mlc::String type_code, mlc::String operand_code) noexcept;

mlc::String print_cast_kind_prefix(cpp_ast::CppCastKind kind) noexcept;

mlc::String print_comma_separated_expressions_multiline(mlc::Array<std::shared_ptr<cpp_ast::CppExpr>> expressions) noexcept;

mlc::String print_capture_list(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures) noexcept;

mlc::String print_parameter_list(mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters) noexcept;

mlc::String block_braces(mlc::String body_code) noexcept;

mlc::String if_statement(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept;

mlc::String while_statement(mlc::String condition_code, mlc::String body_code) noexcept;

mlc::String range_for_statement(mlc::String variable_name, mlc::String range_code, mlc::String body_code) noexcept;

mlc::String auto_declaration(mlc::String name, mlc::String initializer_code) noexcept;

mlc::String const_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept;

mlc::String include_angle(mlc::String path) noexcept;

mlc::String include_quoted(mlc::String path) noexcept;

mlc::String using_alias(mlc::String alias, mlc::String type_code) noexcept;

mlc::String struct_definition(mlc::String name, mlc::String fields_code) noexcept;

mlc::String struct_field_line(mlc::String type_code, mlc::String field_name) noexcept;

mlc::String function_prototype(mlc::String return_type, mlc::String name, mlc::String parameters_code) noexcept;

mlc::String function_definition(mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String body_code) noexcept;

mlc::String namespace_block(mlc::String name, mlc::String contents_code) noexcept;

mlc::String variant_type_alias(mlc::String name, mlc::String types_code) noexcept;

mlc::String print_struct_fields(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept;

mlc::String print_variant_types(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept;

mlc::String print_statement_body(std::shared_ptr<cpp_ast::CppStmt> statement) noexcept;

mlc::String print_statement_node(std::shared_ptr<cpp_ast::CppStmt> statement) noexcept;

mlc::String print_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept;

mlc::String print_decl_node(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept;

mlc::String print_decls(mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations) noexcept;

mlc::String print_file_node(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String invoked_while_expression(std::shared_ptr<cpp_ast::CppExpr> condition, mlc::String body_statements) noexcept;

mlc::String invoked_for_expression(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpr> iterator, mlc::String body_statements) noexcept;

mlc::String question_try_expression(std::shared_ptr<cpp_ast::CppExpr> inner) noexcept;

mlc::String with_block_expression(std::shared_ptr<cpp_ast::CppExpr> resource, mlc::String binder, mlc::String body_statements) noexcept;

mlc::String lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String return_type_code, mlc::String body_code) noexcept;

mlc::String print_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> body) noexcept;

mlc::String print_member_expression(std::shared_ptr<cpp_ast::CppExpr> object, mlc::String member_name, bool is_pointer) noexcept;

mlc::String print_integer_literal(int integer_value) noexcept;

mlc::String print_string_literal(mlc::String string_value) noexcept;

mlc::String print_boolean_literal(bool boolean_value) noexcept;

mlc::String print_identifier(mlc::String name) noexcept;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpr> expression) noexcept;

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStmt> statement) noexcept;

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept;

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String escape_cpp_string_content(mlc::String input) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < input.length()){
{
mlc::String character = input.char_at(index);
if (character == mlc::String("\\")){
{
parts.push_back(mlc::String("\\\\"));
}
} else {
{
if (character == mlc::String("\"")){
parts.push_back(mlc::String("\\\""));
} else {
if (character == mlc::String("\n")){
parts.push_back(mlc::String("\\n"));
} else {
if (character == mlc::String("\r")){
parts.push_back(mlc::String("\\r"));
} else {
if (character == mlc::String("\t")){
parts.push_back(mlc::String("\\t"));
} else {
if (character == mlc::String("\0", 1)){
parts.push_back(mlc::String("\\0"));
} else {
parts.push_back(character);
}
}
}
}
}
}
}
index = index + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String print_comma_separated_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpr>> expressions) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < expressions.size()){
{
parts.push_back(print_expr(expressions[index]));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String print_comma_separated_types(mlc::Array<std::shared_ptr<cpp_ast::CppType>> types) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < types.size()){
{
parts.push_back(print_type(types[index]));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String print_comma_separated_strings(mlc::Array<mlc::String> strings) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < strings.size()){
{
parts.push_back(strings[index]);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String type_template(mlc::String name, mlc::String arguments_code) noexcept{return name + mlc::String("<") + arguments_code + mlc::String(">");}

mlc::String type_reference(mlc::String inner_code) noexcept{return inner_code + mlc::String("&");}

mlc::String print_type(std::shared_ptr<cpp_ast::CppType> type_node) noexcept{return std::visit(overloaded{
  [&](const CppTypeName& cpptypename) -> mlc::String { auto [name] = cpptypename; return name; },
  [&](const CppTypeTemplate& cpptypetemplate) -> mlc::String { auto [name, arguments] = cpptypetemplate; return type_template(name, print_comma_separated_types(arguments)); },
  [&](const CppTypeRef& cpptyperef) -> mlc::String { auto [inner] = cpptyperef; return type_reference(print_type(inner)); }
}, (*type_node));}

mlc::String render_ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{return mlc::String("(") + condition_code + mlc::String(" ? (") + then_code + mlc::String(") : (") + else_code + mlc::String("))");}

mlc::String render_parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept{return mlc::String("(") + left_code + mlc::String(" ") + operation + mlc::String(" ") + right_code + mlc::String(")");}

mlc::String render_parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept{return mlc::String("(") + operation + inner_code + mlc::String(")");}

mlc::String render_index_subscript(mlc::String object_code, mlc::String index_code) noexcept{return object_code + mlc::String("[") + index_code + mlc::String("]");}

mlc::String render_function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept{return callee_code + mlc::String("(") + argument_list + mlc::String(")");}

mlc::String pointer_field_access(mlc::String object_code, mlc::String field_name) noexcept{return object_code + mlc::String("->") + field_name;}

mlc::String value_field_access(mlc::String object_code, mlc::String field_name) noexcept{return object_code + mlc::String(".") + field_name;}

mlc::String init_list_braces(mlc::String element_list) noexcept{return element_list.length() == 0 ? mlc::String("{}") : mlc::String("{") + element_list + mlc::String("}");}

mlc::String std_visit_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept{return mlc::String("std::visit(overloaded{") + arm_lambdas + mlc::String("\n}, ") + visit_subject + mlc::String(")");}

mlc::String cast_expression(mlc::String kind_prefix, mlc::String type_code, mlc::String operand_code) noexcept{return kind_prefix + mlc::String("<") + type_code + mlc::String(">(") + operand_code + mlc::String(")");}

mlc::String print_cast_kind_prefix(cpp_ast::CppCastKind kind) noexcept{return std::visit(overloaded{
  [&](const CppCastStatic& cppcaststatic) -> mlc::String { return mlc::String("static_cast"); },
  [&](const CppCastDynamic& cppcastdynamic) -> mlc::String { return mlc::String("dynamic_cast"); },
  [&](const CppCastReinterpret& cppcastreinterpret) -> mlc::String { return mlc::String("reinterpret_cast"); },
  [&](const CppCastConst& cppcastconst) -> mlc::String { return mlc::String("const_cast"); }
}, kind);}

mlc::String print_comma_separated_expressions_multiline(mlc::Array<std::shared_ptr<cpp_ast::CppExpr>> expressions) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < expressions.size()){
{
parts.push_back(print_expr(expressions[index]));
index = index + 1;
}
}
return parts.join(mlc::String(",\n"));
}

mlc::String print_capture_list(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < captures.size()){
{
std::shared_ptr<cpp_ast::CppCapture> capture_entry = captures[index];
if (cpp_ast::cpp_capture_by_reference(capture_entry)){
{
parts.push_back(mlc::String("&") + cpp_ast::cpp_capture_name(capture_entry));
}
} else {
{
parts.push_back(cpp_ast::cpp_capture_name(capture_entry));
}
}
index = index + 1;
}
}
return mlc::String("[") + parts.join(mlc::String(", ")) + mlc::String("]");
}

mlc::String print_parameter_list(mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < parameters.size()){
{
std::shared_ptr<cpp_ast::CppParam> parameter = parameters[index];
parts.push_back(print_type(cpp_ast::cpp_param_type(parameter)) + mlc::String(" ") + cpp_ast::cpp_param_name(parameter));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String block_braces(mlc::String body_code) noexcept{return mlc::String("{") + body_code + mlc::String("}");}

mlc::String if_statement(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{return mlc::String("if (") + condition_code + mlc::String(") ") + then_code + mlc::String(" else ") + else_code;}

mlc::String while_statement(mlc::String condition_code, mlc::String body_code) noexcept{return mlc::String("while (") + condition_code + mlc::String(") ") + body_code;}

mlc::String range_for_statement(mlc::String variable_name, mlc::String range_code, mlc::String body_code) noexcept{return mlc::String("for (auto ") + variable_name + mlc::String(" : ") + range_code + mlc::String(") ") + body_code;}

mlc::String auto_declaration(mlc::String name, mlc::String initializer_code) noexcept{return mlc::String("auto ") + name + mlc::String(" = ") + initializer_code + mlc::String(";");}

mlc::String const_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept{return mlc::String("const ") + type_code + mlc::String(" ") + name + mlc::String(" = ") + initializer_code + mlc::String(";");}

mlc::String include_angle(mlc::String path) noexcept{return mlc::String("#include <") + path + mlc::String(">");}

mlc::String include_quoted(mlc::String path) noexcept{return mlc::String("#include \"") + path + mlc::String("\"");}

mlc::String using_alias(mlc::String alias, mlc::String type_code) noexcept{return mlc::String("using ") + alias + mlc::String(" = ") + type_code + mlc::String(";");}

mlc::String struct_definition(mlc::String name, mlc::String fields_code) noexcept{return mlc::String("struct ") + name + mlc::String(" {\n") + fields_code + mlc::String("\n};");}

mlc::String struct_field_line(mlc::String type_code, mlc::String field_name) noexcept{return type_code + mlc::String(" ") + field_name + mlc::String(";");}

mlc::String function_prototype(mlc::String return_type, mlc::String name, mlc::String parameters_code) noexcept{return return_type + mlc::String(" ") + name + mlc::String("(") + parameters_code + mlc::String(");");}

mlc::String function_definition(mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String body_code) noexcept{return return_type + mlc::String(" ") + name + mlc::String("(") + parameters_code + mlc::String(") ") + body_code;}

mlc::String namespace_block(mlc::String name, mlc::String contents_code) noexcept{return mlc::String("namespace ") + name + mlc::String(" {\n") + contents_code + mlc::String("\n}");}

mlc::String variant_type_alias(mlc::String name, mlc::String types_code) noexcept{return mlc::String("using ") + name + mlc::String(" = std::variant<") + types_code + mlc::String(">;");}

mlc::String print_struct_fields(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < fields.size()){
{
std::shared_ptr<cpp_ast::CppField> field_entry = fields[index];
parts.push_back(struct_field_line(cpp_ast::cpp_field_type(field_entry), cpp_ast::cpp_field_name(field_entry)));
index = index + 1;
}
}
return parts.join(mlc::String("\n"));
}

mlc::String print_variant_types(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept{
mlc::Array<mlc::String> parts = {};
int arm_index = 0;
while (arm_index < arms.size()){
{
mlc::Array<std::shared_ptr<cpp_ast::CppType>> types = cpp_ast::cpp_variant_arm_types(arms[arm_index]);
int type_index = 0;
while (type_index < types.size()){
{
parts.push_back(print_type(types[type_index]));
type_index = type_index + 1;
}
}
arm_index = arm_index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String print_statement_body(std::shared_ptr<cpp_ast::CppStmt> statement) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_ast::CppBlock>((*statement)._)) { auto _v_cppblock = std::get<cpp_ast::CppBlock>((*statement)._); auto [statements] = _v_cppblock; return block_braces(print_statements(statements)); } return print_statement_node(statement); }();}

mlc::String print_statement_node(std::shared_ptr<cpp_ast::CppStmt> statement) noexcept{return std::visit(overloaded{
  [&](const CppAutoDecl& cppautodecl) -> mlc::String { auto [name, initializer] = cppautodecl; return auto_declaration(name, print_expr(initializer)); },
  [&](const CppConstDecl& cppconstdecl) -> mlc::String { auto [name, type_node, initializer] = cppconstdecl; return const_declaration(print_type(type_node), name, print_expr(initializer)); },
  [&](const CppReturn& cppreturn) -> mlc::String { auto [expression] = cppreturn; return mlc::String("return ") + print_expr(expression) + mlc::String(";"); },
  [&](const CppExprStmt& cppexprstmt) -> mlc::String { auto [expression] = cppexprstmt; return print_expr(expression) + mlc::String(";"); },
  [&](const CppBlock& cppblock) -> mlc::String { auto [statements] = cppblock; return block_braces(print_statements(statements)); },
  [&](const CppIf& cppif) -> mlc::String { auto [condition, then_branch, else_branch] = cppif; return if_statement(print_expr(condition), print_statement_body(then_branch), print_statement_body(else_branch)); },
  [&](const CppWhile& cppwhile) -> mlc::String { auto [condition, body] = cppwhile; return while_statement(print_expr(condition), print_statement_body(body)); },
  [&](const CppFor& cppfor) -> mlc::String { auto [variable_name, range_expression, body] = cppfor; return range_for_statement(variable_name, print_expr(range_expression), block_braces(print_statements(body))); }
}, (*statement)._);}

mlc::String print_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < statements.size()){
{
parts.push_back(print_statement_node(statements[index]));
index = index + 1;
}
}
return parts.join(mlc::String("\n"));
}

mlc::String print_decl_node(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept{return std::visit(overloaded{
  [&](const CppInclude& cppinclude) -> mlc::String { auto [is_angle, path] = cppinclude; return is_angle ? include_angle(path) : include_quoted(path); },
  [&](const CppUsing& cppusing) -> mlc::String { auto [alias, type_code] = cppusing; return using_alias(alias, type_code); },
  [&](const CppStruct& cppstruct) -> mlc::String { auto [name, fields] = cppstruct; return struct_definition(name, print_struct_fields(fields)); },
  [&](const CppFnProto& cppfnproto) -> mlc::String { auto [return_type, name, parameters] = cppfnproto; return function_prototype(return_type, name, print_comma_separated_strings(parameters)); },
  [&](const CppFnDef& cppfndef) -> mlc::String { auto [return_type, name, parameters, body] = cppfndef; return function_definition(return_type, name, print_comma_separated_strings(parameters), block_braces(print_statements(body))); },
  [&](const CppNamespace& cppnamespace) -> mlc::String { auto [name, declarations] = cppnamespace; return namespace_block(name, print_decls(declarations)); },
  [&](const CppVariant& cppvariant) -> mlc::String { auto [name, arms] = cppvariant; return variant_type_alias(name, print_variant_types(arms)); },
  [&](const CppDeclFragment& cppdeclfragment) -> mlc::String { auto [code] = cppdeclfragment; return code; }
}, (*declaration));}

mlc::String print_decls(mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < declarations.size()){
{
parts.push_back(print_decl_node(declarations[index]));
index = index + 1;
}
}
return parts.join(mlc::String("\n\n"));
}

mlc::String print_file_node(std::shared_ptr<cpp_ast::CppFile> file) noexcept{
mlc::String header_code = print_decls(cpp_ast::cpp_file_header(file));
mlc::String source_code = print_decls(cpp_ast::cpp_file_source(file));
return header_code.length() == 0 ? source_code : source_code.length() == 0 ? header_code : header_code + mlc::String("\n\n") + source_code;
}

mlc::String invoked_while_expression(std::shared_ptr<cpp_ast::CppExpr> condition, mlc::String body_statements) noexcept{return mlc::String("[&]() {\nwhile (") + print_expr(condition) + mlc::String(") {\n") + body_statements + mlc::String("}\n}()");}

mlc::String invoked_for_expression(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpr> iterator, mlc::String body_statements) noexcept{return mlc::String("[&]() {\nfor (auto ") + variable_name + mlc::String(" : ") + print_expr(iterator) + mlc::String(") {\n") + body_statements + mlc::String("}\n}()");}

mlc::String question_try_expression(std::shared_ptr<cpp_ast::CppExpr> inner) noexcept{return mlc::String("({ auto __q = ") + print_expr(inner) + mlc::String("; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })");}

mlc::String with_block_expression(std::shared_ptr<cpp_ast::CppExpr> resource, mlc::String binder, mlc::String body_statements) noexcept{return mlc::String("{\nauto ") + binder + mlc::String(" = (") + print_expr(resource) + mlc::String(");\n") + body_statements + binder + mlc::String(".drop();\n}\n");}

mlc::String lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String return_type_code, mlc::String body_code) noexcept{return capture_prefix + mlc::String("(") + parameters_code + mlc::String(") -> ") + return_type_code + mlc::String(" { ") + body_code + mlc::String(" }");}

mlc::String print_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> body) noexcept{
mlc::String capture_prefix = print_capture_list(captures);
mlc::String parameters_code = print_parameter_list(parameters);
mlc::String return_type_code = print_type(return_type);
mlc::String body_code = print_statements(body);
return lambda_expression(capture_prefix, parameters_code, return_type_code, body_code);
}

mlc::String print_member_expression(std::shared_ptr<cpp_ast::CppExpr> object, mlc::String member_name, bool is_pointer) noexcept{return is_pointer ? pointer_field_access(print_expr(object), member_name) : value_field_access(print_expr(object), member_name);}

mlc::String print_integer_literal(int integer_value) noexcept{return mlc::to_string(integer_value);}

mlc::String print_string_literal(mlc::String string_value) noexcept{return mlc::String("\"") + escape_cpp_string_content(string_value) + mlc::String("\"");}

mlc::String print_boolean_literal(bool boolean_value) noexcept{return boolean_value ? mlc::String("true") : mlc::String("false");}

mlc::String print_identifier(mlc::String name) noexcept{return name;}

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpr> expression) noexcept{return std::visit(overloaded{
  [&](const CppInt& cppint) -> mlc::String { auto [integer_value] = cppint; return print_integer_literal(integer_value); },
  [&](const CppStr& cppstr) -> mlc::String { auto [string_value] = cppstr; return print_string_literal(string_value); },
  [&](const CppBool& cppbool) -> mlc::String { auto [boolean_value] = cppbool; return print_boolean_literal(boolean_value); },
  [&](const CppIdent& cppident) -> mlc::String { auto [name] = cppident; return print_identifier(name); },
  [&](const CppCall& cppcall) -> mlc::String { auto [callee, arguments] = cppcall; return render_function_call_parentheses(print_expr(callee), print_comma_separated_expressions(arguments)); },
  [&](const CppMember& cppmember) -> mlc::String { auto [object, member_name, is_pointer] = cppmember; return print_member_expression(object, member_name, is_pointer); },
  [&](const CppIndex& cppindex) -> mlc::String { auto [object, index] = cppindex; return render_index_subscript(print_expr(object), print_expr(index)); },
  [&](const CppBinary& cppbinary) -> mlc::String { auto [operation, left, right] = cppbinary; return render_parenthesized_binary(print_expr(left), operation, print_expr(right)); },
  [&](const CppUnary& cppunary) -> mlc::String { auto [operation, inner] = cppunary; return render_parenthesized_unary(operation, print_expr(inner)); },
  [&](const CppTernary& cppternary) -> mlc::String { auto [condition, then_branch, else_branch] = cppternary; return render_ternary_conditional(print_expr(condition), print_expr(then_branch), print_expr(else_branch)); },
  [&](const CppInitList& cppinitlist) -> mlc::String { auto [elements] = cppinitlist; return init_list_braces(print_comma_separated_expressions(elements)); },
  [&](const CppStdVisit& cppstdvisit) -> mlc::String { auto [subject, handlers] = cppstdvisit; return std_visit_expression(print_comma_separated_expressions_multiline(handlers), print_expr(subject)); },
  [&](const CppCast& cppcast) -> mlc::String { auto [kind, target_type, operand] = cppcast; return cast_expression(print_cast_kind_prefix(kind), print_type(target_type), print_expr(operand)); },
  [&](const CppLambda& cpplambda) -> mlc::String { auto [captures, parameters, return_type, body] = cpplambda; return print_lambda_expression(captures, parameters, return_type, body); },
  [&](const CppInvokedWhile& cppinvokedwhile) -> mlc::String { auto [condition, body_statements] = cppinvokedwhile; return invoked_while_expression(condition, body_statements); },
  [&](const CppInvokedFor& cppinvokedfor) -> mlc::String { auto [variable_name, iterator, body_statements] = cppinvokedfor; return invoked_for_expression(variable_name, iterator, body_statements); },
  [&](const CppQuestionTry& cppquestiontry) -> mlc::String { auto [inner] = cppquestiontry; return question_try_expression(inner); },
  [&](const CppWithBlock& cppwithblock) -> mlc::String { auto [resource, binder, body_statements] = cppwithblock; return with_block_expression(resource, binder, body_statements); }
}, (*expression)._);}

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStmt> statement) noexcept{return print_statement_node(statement);}

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept{return print_decl_node(declaration);}

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept{return print_file_node(file);}

} // namespace cpp_printer
