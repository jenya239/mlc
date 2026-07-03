#define main mlc_user_main
#include "print.hpp"

#include "cpp_ast.hpp"

namespace print {

using namespace cpp_ast;

mlc::String printer_indent_unit() noexcept{
  return mlc::String("  ", 2);
}
mlc::Array<int> zero_based_indices(int count) noexcept{
  if ((count <= 0))   {
    return mlc::Array<int>{};
  } else   {
    return zero_based_indices((count - 1)).concat(mlc::Array<int>{(count - 1)});
  }
}
mlc::String indent_text(int depth) noexcept{
  return zero_based_indices(depth).map([=](int _) mutable { return printer_indent_unit(); }).join(mlc::String("", 0));
}
mlc::String escape_cpp_character(mlc::String character) noexcept{
  if ((character == mlc::String("\\", 1)))   {
    return mlc::String("\\\\", 2);
  } else if ((character == mlc::String("\"", 1)))   {
    return mlc::String("\\\"", 2);
  } else if ((character == mlc::String("\n", 1)))   {
    return mlc::String("\\n", 2);
  } else if ((character == mlc::String("\r", 1)))   {
    return mlc::String("\\r", 2);
  } else if ((character == mlc::String("\t", 1)))   {
    return mlc::String("\\t", 2);
  } else if ((character == mlc::String("\0", 1)))   {
    return mlc::String("\\0", 2);
  } else   {
    return character;
  }
}
mlc::String escape_cpp_string_content(mlc::String input) noexcept{
  return zero_based_indices(input.length()).map([=](int index) mutable { return escape_cpp_character(input.char_at(index)); }).join(mlc::String("", 0));
}
mlc::String formatted_block(mlc::String statements_code, int depth) noexcept{
  return (((((indent_text(depth) + mlc::String("{\n", 2)) + statements_code) + mlc::String("\n", 1)) + indent_text(depth)) + mlc::String("}", 1));
}
mlc::String print_comma_separated_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{
  return expressions.map([=](std::shared_ptr<cpp_ast::CppExpression> expression) mutable { return print_expr(expression); }).join(mlc::String(", ", 2));
}
mlc::String print_comma_separated_types(mlc::Array<std::shared_ptr<cpp_ast::CppType>> types) noexcept{
  return types.map([=](std::shared_ptr<cpp_ast::CppType> type_node) mutable { return print_type(type_node); }).join(mlc::String(", ", 2));
}
mlc::String print_comma_separated_strings(mlc::Array<mlc::String> strings) noexcept{
  return strings.join(mlc::String(", ", 2));
}
mlc::String type_template(mlc::String name, mlc::String arguments_code) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(name)) + mlc::String("<", 1)) + mlc::to_string(arguments_code)) + mlc::String(">", 1));
}
mlc::String type_reference(mlc::String inner_code) noexcept{
  return ((mlc::String("", 0) + mlc::to_string(inner_code)) + mlc::String("&", 1));
}
mlc::String type_rvalue_reference(mlc::String inner_code) noexcept{
  return ((mlc::String("", 0) + mlc::to_string(inner_code)) + mlc::String("&&", 2));
}
mlc::String type_pointer(mlc::String inner_code) noexcept{
  return ((mlc::String("", 0) + mlc::to_string(inner_code)) + mlc::String("*", 1));
}
mlc::String type_const_prefix(mlc::String inner_code) noexcept{
  return ((mlc::String("const ", 6) + mlc::to_string(inner_code)) + mlc::String("", 0));
}
mlc::String print_cpp_type(std::shared_ptr<cpp_ast::CppType> type_node) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppTypeName& cppTypeName) { auto [name] = cppTypeName; return name; },
[&](const cpp_ast::CppTypeTemplate& cppTypeTemplate) { auto [name, arguments] = cppTypeTemplate; return type_template(name, print_comma_separated_types(arguments)); },
[&](const cpp_ast::CppTypeRef& cppTypeRef) { auto [inner] = cppTypeRef; return type_reference(print_cpp_type(inner)); },
[&](const cpp_ast::CppTypeRRef& cppTypeRRef) { auto [inner] = cppTypeRRef; return type_rvalue_reference(print_cpp_type(inner)); },
[&](const cpp_ast::CppTypePtr& cppTypePtr) { auto [inner] = cppTypePtr; return type_pointer(print_cpp_type(inner)); },
[&](const cpp_ast::CppTypeConst& cppTypeConst) { auto [inner] = cppTypeConst; return type_const_prefix(print_cpp_type(inner)); }
}, (*type_node));
}
mlc::String print_type(std::shared_ptr<cpp_ast::CppType> type_node) noexcept{
  return print_cpp_type(type_node);
}
mlc::String render_ternary_conditional(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{
  return ((((((mlc::String("(", 1) + mlc::to_string(condition_code)) + mlc::String(" ? (", 4)) + mlc::to_string(then_code)) + mlc::String(") : (", 5)) + mlc::to_string(else_code)) + mlc::String("))", 2));
}
mlc::String render_parenthesized_binary(mlc::String left_code, mlc::String operation, mlc::String right_code) noexcept{
  return ((((((mlc::String("(", 1) + mlc::to_string(left_code)) + mlc::String(" ", 1)) + mlc::to_string(operation)) + mlc::String(" ", 1)) + mlc::to_string(right_code)) + mlc::String(")", 1));
}
mlc::String render_parenthesized_unary(mlc::String operation, mlc::String inner_code) noexcept{
  return ((((mlc::String("(", 1) + mlc::to_string(operation)) + mlc::String("", 0)) + mlc::to_string(inner_code)) + mlc::String(")", 1));
}
mlc::String render_index_subscript(mlc::String object_code, mlc::String index_code) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(object_code)) + mlc::String("[", 1)) + mlc::to_string(index_code)) + mlc::String("]", 1));
}
mlc::String render_function_call_parentheses(mlc::String callee_code, mlc::String argument_list) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(callee_code)) + mlc::String("(", 1)) + mlc::to_string(argument_list)) + mlc::String(")", 1));
}
mlc::String pointer_field_access(mlc::String object_code, mlc::String field_name) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(object_code)) + mlc::String("->", 2)) + mlc::to_string(field_name)) + mlc::String("", 0));
}
mlc::String value_field_access(mlc::String object_code, mlc::String field_name) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(object_code)) + mlc::String(".", 1)) + mlc::to_string(field_name)) + mlc::String("", 0));
}
mlc::String init_list_braces(mlc::String element_list) noexcept{
  if ((element_list.length() == 0))   {
    return mlc::String("{}", 2);
  } else   {
    return ((mlc::String("{", 1) + mlc::to_string(element_list)) + mlc::String("}", 1));
  }
}
mlc::String aggregate_initializer_expression(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> elements) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(type_name)) + mlc::String("{", 1)) + mlc::to_string(print_comma_separated_expressions(elements))) + mlc::String("}", 1));
}
mlc::String std_visit_expression(mlc::String arm_lambdas, mlc::String visit_subject) noexcept{
  return ((((mlc::String("std::visit(overloaded{", 22) + mlc::to_string(arm_lambdas)) + mlc::String("\n}, ", 4)) + mlc::to_string(visit_subject)) + mlc::String(")", 1));
}
mlc::String print_visit_arm_wild(bool void_return, mlc::String return_cpp, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{
  if (void_return)   {
    return ((mlc::String("[&](const auto& __v) -> void { ", 31) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else if ((return_cpp.length() > 0))   {
    return ((((mlc::String("[&](const auto& __v) -> ", 24) + mlc::to_string(return_cpp)) + mlc::String(" { return ", 10)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else   {
    return ((mlc::String("[&](const auto& __v) { return ", 30) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  }
}
mlc::String print_visit_arm_binding(mlc::String parameter_name, bool void_return, mlc::String return_cpp, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{
  if (void_return)   {
    return ((((mlc::String("[&](const auto& ", 16) + mlc::to_string(parameter_name)) + mlc::String(") -> void { ", 12)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else if ((return_cpp.length() > 0))   {
    return ((((((mlc::String("[&](const auto& ", 16) + mlc::to_string(parameter_name)) + mlc::String(") -> ", 5)) + mlc::to_string(return_cpp)) + mlc::String(" { return ", 10)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else   {
    return ((((mlc::String("[&](const auto& ", 16) + mlc::to_string(parameter_name)) + mlc::String(") { return ", 11)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  }
}
mlc::String print_visit_arm_constructed(mlc::String parameter_declaration, mlc::String binding_prefix, bool void_return, mlc::String return_cpp, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{
  if (void_return)   {
    return ((((((mlc::String("[&](", 4) + mlc::to_string(parameter_declaration)) + mlc::String(") -> void { ", 12)) + mlc::to_string(binding_prefix)) + mlc::String("", 0)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else if ((return_cpp.length() > 0))   {
    return ((((((((mlc::String("[&](", 4) + mlc::to_string(parameter_declaration)) + mlc::String(") -> ", 5)) + mlc::to_string(return_cpp)) + mlc::String(" { ", 3)) + mlc::to_string(binding_prefix)) + mlc::String("return ", 7)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else   {
    return ((((((mlc::String("[&](", 4) + mlc::to_string(parameter_declaration)) + mlc::String(") { ", 4)) + mlc::to_string(binding_prefix)) + mlc::String("return ", 7)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  }
}
mlc::String print_visit_arm_constructed_generic(mlc::String type_name, mlc::String binding_name, mlc::String binding_prefix, bool void_return, mlc::String return_cpp, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{
  if (void_return)   {
    return ((((((((mlc::String("[&]<typename __mlcT>(const ", 27) + mlc::to_string(type_name)) + mlc::String("<__mlcT>& ", 10)) + mlc::to_string(binding_name)) + mlc::String(") -> void { ", 12)) + mlc::to_string(binding_prefix)) + mlc::String("", 0)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else if ((return_cpp.length() > 0))   {
    return ((((((((((mlc::String("[&]<typename __mlcT>(const ", 27) + mlc::to_string(type_name)) + mlc::String("<__mlcT>& ", 10)) + mlc::to_string(binding_name)) + mlc::String(") -> ", 5)) + mlc::to_string(return_cpp)) + mlc::String(" { ", 3)) + mlc::to_string(binding_prefix)) + mlc::String("return ", 7)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  } else   {
    return ((((((((mlc::String("[&]<typename __mlcT>(const ", 27) + mlc::to_string(type_name)) + mlc::String("<__mlcT>& ", 10)) + mlc::to_string(binding_name)) + mlc::String(") { ", 4)) + mlc::to_string(binding_prefix)) + mlc::String("return ", 7)) + mlc::to_string(print_expr(body))) + mlc::String("; }", 3));
  }
}
mlc::String cast_expression(mlc::String kind_prefix, mlc::String type_code, mlc::String operand_code) noexcept{
  return ((((((mlc::String("", 0) + mlc::to_string(kind_prefix)) + mlc::String("<", 1)) + mlc::to_string(type_code)) + mlc::String(">(", 2)) + mlc::to_string(operand_code)) + mlc::String(")", 1));
}
mlc::String print_cast_kind_prefix(cpp_ast::CppCastKind kind) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppCastStatic& cppCastStatic) { return mlc::String("static_cast", 11); },
[&](const cpp_ast::CppCastDynamic& cppCastDynamic) { return mlc::String("dynamic_cast", 12); },
[&](const cpp_ast::CppCastReinterpret& cppCastReinterpret) { return mlc::String("reinterpret_cast", 16); },
[&](const cpp_ast::CppCastConst& cppCastConst) { return mlc::String("const_cast", 10); }
}, kind);
}
mlc::String print_comma_separated_expressions_multiline(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept{
  return expressions.map([=](std::shared_ptr<cpp_ast::CppExpression> expression) mutable { return print_expr(expression); }).join(mlc::String(",\n", 2));
}
mlc::String print_capture_entry(std::shared_ptr<cpp_ast::CppCapture> capture_entry) noexcept{
  if (cpp_ast::cpp_capture_by_reference(capture_entry))   {
    return (mlc::String("&", 1) + cpp_ast::cpp_capture_name(capture_entry));
  } else   {
    return cpp_ast::cpp_capture_name(capture_entry);
  }
}
mlc::String print_capture_list(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures) noexcept{
  return ((mlc::String("[", 1) + captures.map([=](std::shared_ptr<cpp_ast::CppCapture> capture_entry) mutable { return print_capture_entry(capture_entry); }).join(mlc::String(", ", 2))) + mlc::String("]", 1));
}
mlc::String print_parameter_entry(std::shared_ptr<cpp_ast::CppParam> parameter) noexcept{
  return ((print_type(cpp_ast::cpp_param_type(parameter)) + mlc::String(" ", 1)) + cpp_ast::cpp_param_name(parameter));
}
mlc::String print_parameter_list(mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters) noexcept{
  return parameters.map([=](std::shared_ptr<cpp_ast::CppParam> parameter) mutable { return print_parameter_entry(parameter); }).join(mlc::String(", ", 2));
}
mlc::String block_braces(mlc::String body_code) noexcept{
  return ((mlc::String("{", 1) + mlc::to_string(body_code)) + mlc::String("}", 1));
}
mlc::String print_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, int depth) noexcept{
  return statements.map([=](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return print_statement_at_depth(statement, depth); }).join(mlc::String("\n", 1));
}
mlc::String print_statement_at_depth(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = statement;
if (std::holds_alternative<cpp_ast::CppStatementFragment>((*__match_subject))) {
const cpp_ast::CppStatementFragment& cppStatementFragment = std::get<cpp_ast::CppStatementFragment>((*__match_subject));
auto [fragment] = cppStatementFragment; return fragment;
}
return (indent_text(depth) + print_statement_node(statement, depth));
std::abort();
}();
}
mlc::String print_statement_body(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = statement;
if (std::holds_alternative<cpp_ast::CppBlock>((*__match_subject))) {
const cpp_ast::CppBlock& cppBlock = std::get<cpp_ast::CppBlock>((*__match_subject));
auto [statements] = cppBlock; return formatted_block(print_statements(statements, (depth + 1)), depth);
}
if (std::holds_alternative<cpp_ast::CppIf>((*__match_subject))) {
const cpp_ast::CppIf& cppIf = std::get<cpp_ast::CppIf>((*__match_subject));
auto [__0, __1, __2] = cppIf; return print_statement_node(statement, depth);
}
return formatted_block((indent_text((depth + 1)) + print_statement_node(statement, (depth + 1))), depth);
std::abort();
}();
}
mlc::String if_statement(mlc::String condition_code, mlc::String then_code, mlc::String else_code) noexcept{
  return ((((((mlc::String("if (", 4) + mlc::to_string(condition_code)) + mlc::String(") ", 2)) + mlc::to_string(then_code)) + mlc::String(" else ", 6)) + mlc::to_string(else_code)) + mlc::String("", 0));
}
mlc::String while_statement(mlc::String condition_code, mlc::String body_code) noexcept{
  return ((((mlc::String("while (", 7) + mlc::to_string(condition_code)) + mlc::String(") ", 2)) + mlc::to_string(body_code)) + mlc::String("", 0));
}
mlc::String range_for_statement(mlc::String variable_name, mlc::String range_code, mlc::String body_code) noexcept{
  return ((((((mlc::String("for (auto ", 10) + mlc::to_string(variable_name)) + mlc::String(" : ", 3)) + mlc::to_string(range_code)) + mlc::String(") ", 2)) + mlc::to_string(body_code)) + mlc::String("", 0));
}
mlc::String auto_declaration(mlc::String name, mlc::String initializer_code) noexcept{
  return ((((mlc::String("auto ", 5) + mlc::to_string(name)) + mlc::String(" = ", 3)) + mlc::to_string(initializer_code)) + mlc::String(";", 1));
}
mlc::String structured_binding_declaration(mlc::Array<mlc::String> binding_names, mlc::String initializer_code) noexcept{
  return ((((mlc::String("auto [", 6) + mlc::to_string(print_comma_separated_strings(binding_names))) + mlc::String("] = ", 4)) + mlc::to_string(initializer_code)) + mlc::String(";", 1));
}
mlc::String constexpr_auto_declaration(mlc::String name, mlc::String initializer_code) noexcept{
  return ((((mlc::String("constexpr auto ", 15) + mlc::to_string(name)) + mlc::String(" = ", 3)) + mlc::to_string(initializer_code)) + mlc::String(";", 1));
}
mlc::String const_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept{
  return ((((((mlc::String("const ", 6) + mlc::to_string(type_code)) + mlc::String(" ", 1)) + mlc::to_string(name)) + mlc::String(" = ", 3)) + mlc::to_string(initializer_code)) + mlc::String(";", 1));
}
mlc::String variable_declaration(mlc::String type_code, mlc::String name, mlc::String initializer_code) noexcept{
  return ((((((mlc::String("", 0) + mlc::to_string(type_code)) + mlc::String(" ", 1)) + mlc::to_string(name)) + mlc::String(" = ", 3)) + mlc::to_string(initializer_code)) + mlc::String(";", 1));
}
mlc::String include_angle(mlc::String path) noexcept{
  return ((mlc::String("#include <", 10) + mlc::to_string(path)) + mlc::String(">\n", 2));
}
mlc::String include_quoted(mlc::String path) noexcept{
  return ((((mlc::String("#include ", 9) + mlc::String("\"", 1)) + path) + mlc::String("\"", 1)) + mlc::String("\n", 1));
}
mlc::String ifndef_directive(mlc::String macro) noexcept{
  return ((mlc::String("#ifndef ", 8) + mlc::to_string(macro)) + mlc::String("\n", 1));
}
mlc::String define_macro_directive(mlc::String macro, mlc::String value) noexcept{
  if ((value.length() == 0))   {
    return ((mlc::String("#define ", 8) + mlc::to_string(macro)) + mlc::String("\n", 1));
  } else   {
    return ((((mlc::String("#define ", 8) + mlc::to_string(macro)) + mlc::String(" ", 1)) + mlc::to_string(value)) + mlc::String("\n", 1));
  }
}
mlc::String endif_directive(mlc::String comment) noexcept{
  return ((mlc::String("#endif // ", 10) + mlc::to_string(comment)) + mlc::String("\n", 1));
}
mlc::String undef_directive(mlc::String macro) noexcept{
  return ((mlc::String("#undef ", 7) + mlc::to_string(macro)) + mlc::String("\n", 1));
}
mlc::String namespace_begin_directive(mlc::String name) noexcept{
  return ((mlc::String("namespace ", 10) + mlc::to_string(name)) + mlc::String(" {\n", 3));
}
mlc::String namespace_end_directive(mlc::String name) noexcept{
  return ((mlc::String("} // namespace ", 15) + mlc::to_string(name)) + mlc::String("\n", 1));
}
mlc::String using_alias(mlc::String alias, mlc::String type_code) noexcept{
  return ((((mlc::String("using ", 6) + mlc::to_string(alias)) + mlc::String(" = ", 3)) + mlc::to_string(type_code)) + mlc::String(";", 1));
}
mlc::String using_namespace_directive(mlc::String namespace_identifier) noexcept{
  return ((mlc::String("using namespace ", 16) + mlc::to_string(namespace_identifier)) + mlc::String(";\n", 2));
}
mlc::String host_entry_main_epilogue(mlc::String qualified_namespace) noexcept{
  return ((((((((((((mlc::String("\n#undef main\n\n", 14) + mlc::String("static void mlc_cli_set_args(int argc, char** argv) {\n", 54)) + mlc::String("  std::vector<mlc::String> arguments;\n", 38)) + mlc::String("  arguments.reserve(argc > 0 ? argc - 1 : 0);\n", 46)) + mlc::String("  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n", 70)) + mlc::String("  mlc::io::set_args(std::move(arguments));\n", 43)) + mlc::String("}\n\n", 3)) + mlc::String("int main(int argc, char** argv) {\n", 34)) + mlc::String("  mlc_cli_set_args(argc, argv);\n", 32)) + mlc::String("  return ::", 11)) + qualified_namespace) + mlc::String("::mlc_user_main(argc, argv);\n", 29)) + mlc::String("}\n", 2));
}
mlc::String struct_forward_declaration(mlc::String template_prefix, mlc::String name) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("struct ", 7)) + mlc::to_string(name)) + mlc::String(";\n", 2));
}
mlc::String class_forward_declaration(mlc::String name) noexcept{
  return ((mlc::String("class ", 6) + mlc::to_string(name)) + mlc::String(";\n", 2));
}
mlc::String enum_forward_declaration(mlc::String name) noexcept{
  return ((mlc::String("enum ", 5) + mlc::to_string(name)) + mlc::String(";\n", 2));
}
mlc::String print_forward_declaration(mlc::String kind, mlc::String name) noexcept{
  if ((kind == mlc::String("class", 5)))   {
    return class_forward_declaration(name);
  } else if ((kind == mlc::String("enum", 4)))   {
    return enum_forward_declaration(name);
  } else   {
    return struct_forward_declaration(mlc::String("", 0), name);
  }
}
mlc::String print_struct_empty_definition(mlc::String template_prefix, mlc::String name) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("struct ", 7)) + mlc::to_string(name)) + mlc::String(" {};\n", 5));
}
mlc::String struct_definition(mlc::String template_prefix, mlc::String name, mlc::String fields_code, bool forward_only) noexcept{
  if ((fields_code.length() == 0))   {
    if (forward_only)     {
      return struct_forward_declaration(template_prefix, name);
    } else     {
      return print_struct_empty_definition(template_prefix, name);
    }
  } else   {
    return ((((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("struct ", 7)) + mlc::to_string(name)) + mlc::String(" {\n", 3)) + mlc::to_string(fields_code)) + mlc::String("\n};\n", 4));
  }
}
mlc::String struct_field_line(mlc::String type_code, mlc::String field_name) noexcept{
  return ((((mlc::String("", 0) + mlc::to_string(type_code)) + mlc::String(" ", 1)) + mlc::to_string(field_name)) + mlc::String(";", 1));
}
mlc::String function_prototype(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code) noexcept{
  return ((((((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("", 0)) + mlc::to_string(return_type)) + mlc::String(" ", 1)) + mlc::to_string(name)) + mlc::String("(", 1)) + mlc::to_string(parameters_code)) + mlc::String(") noexcept;\n", 12));
}
mlc::String compact_noexcept_function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String return_expression_code) noexcept{
  return ((((((((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("", 0)) + mlc::to_string(return_type)) + mlc::String(" ", 1)) + mlc::to_string(name)) + mlc::String("(", 1)) + mlc::to_string(parameters_code)) + mlc::String(") noexcept { return ", 20)) + mlc::to_string(return_expression_code)) + mlc::String("; }\n", 4));
}
mlc::String function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::String body_code) noexcept{
  return ((((((((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("", 0)) + mlc::to_string(return_type)) + mlc::String(" ", 1)) + mlc::to_string(name)) + mlc::String("(", 1)) + mlc::to_string(parameters_code)) + mlc::String(") noexcept{\n", 12)) + mlc::to_string(body_code)) + mlc::String("\n}\n", 3));
}
mlc::String print_function_definition(mlc::String template_prefix, mlc::String return_type, mlc::String name, mlc::String parameters_code, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body, int body_statement_depth) noexcept{
  if (((body_statement_depth == (-1)) && (body.length() == 1)))   {
    return [&]() -> mlc::String {
auto __match_subject = body[0];
if (std::holds_alternative<cpp_ast::CppReturn>((*__match_subject))) {
const cpp_ast::CppReturn& cppReturn = std::get<cpp_ast::CppReturn>((*__match_subject));
auto [expression] = cppReturn; return compact_noexcept_function_definition(template_prefix, return_type, name, parameters_code, print_expr(expression));
}
return function_definition(template_prefix, return_type, name, parameters_code, print_statements(body, 1));
std::abort();
}();
  } else   {
    return function_definition(template_prefix, return_type, name, parameters_code, print_statements(body, body_statement_depth));
  }
}
mlc::String namespace_block(mlc::String name, mlc::String contents_code) noexcept{
  return ((((mlc::String("namespace ", 10) + mlc::to_string(name)) + mlc::String(" {\n", 3)) + mlc::to_string(contents_code)) + mlc::String("\n}", 2));
}
mlc::String variant_type_alias(mlc::String template_prefix, mlc::String name, mlc::String types_code) noexcept{
  return ((((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("using ", 6)) + mlc::to_string(name)) + mlc::String(" = std::variant<", 16)) + mlc::to_string(types_code)) + mlc::String(">;\n", 3));
}
mlc::String concept_requires_definition(mlc::String template_prefix, mlc::String trait_name, mlc::String self_parameter_name, mlc::Array<mlc::String> requires_expressions) noexcept{
  return ((((((((mlc::String("", 0) + mlc::to_string(template_prefix)) + mlc::String("concept ", 8)) + mlc::to_string(trait_name)) + mlc::String(" = requires(const ", 18)) + mlc::to_string(self_parameter_name)) + mlc::String("& self) { ", 10)) + mlc::to_string(requires_expressions.join(mlc::String("; ", 2)))) + mlc::String("; };\n", 5));
}
mlc::String static_assert_declaration(mlc::String concept_cpp, mlc::Array<mlc::String> type_arguments, mlc::String message) noexcept{
  return ((((((((mlc::String("static_assert(", 14) + mlc::to_string(concept_cpp)) + mlc::String("<", 1)) + mlc::to_string(print_comma_separated_strings(type_arguments))) + mlc::String(">, ", 3)) + mlc::String("\"", 1)) + message) + mlc::String("\"", 1)) + mlc::String(");\n", 3));
}
mlc::String print_hash_specialization_body(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
  return statements.map([=](std::shared_ptr<cpp_ast::CppStatement> statement) mutable { return (mlc::String("    ", 4) + print_statement(statement)); }).join(mlc::String("\n", 1));
}
mlc::String std_hash_specialization_definition(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{
  return ((((((((((((mlc::String("namespace std {\n", 16) + mlc::String("template<>\n", 11)) + mlc::String("struct hash<", 12)) + type_name) + mlc::String("> {\n", 4)) + mlc::String("  size_t operator()(const ", 26)) + type_name) + mlc::String("& self) const noexcept {\n", 25)) + print_hash_specialization_body(body_statements)) + mlc::String("\n", 1)) + mlc::String("  }\n", 4)) + mlc::String("};\n", 3)) + mlc::String("}\n", 2));
}
mlc::String print_struct_field_line(std::shared_ptr<cpp_ast::CppField> field_entry) noexcept{
  return (indent_text(1) + struct_field_line(cpp_ast::cpp_field_type(field_entry), cpp_ast::cpp_field_name(field_entry)));
}
mlc::String print_struct_fields(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept{
  return fields.map([=](std::shared_ptr<cpp_ast::CppField> field_entry) mutable { return print_struct_field_line(field_entry); }).join(mlc::String("\n", 1));
}
mlc::String print_access_specifier_for_base(cpp_ast::CppAccessLevel access) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppPublic& cppPublic) { return mlc::String("public", 6); },
[&](const cpp_ast::CppProtected& cppProtected) { return mlc::String("protected", 9); },
[&](const cpp_ast::CppPrivate& cppPrivate) { return mlc::String("private", 7); }
}, access);
}
mlc::String print_access_label(cpp_ast::CppAccessLevel access) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppPublic& cppPublic) { return mlc::String("public:", 7); },
[&](const cpp_ast::CppProtected& cppProtected) { return mlc::String("protected:", 10); },
[&](const cpp_ast::CppPrivate& cppPrivate) { return mlc::String("private:", 8); }
}, access);
}
mlc::String print_base_class_entry(cpp_ast::CppBaseClass base_class) noexcept{
  return ((print_access_specifier_for_base(base_class.access) + mlc::String(" ", 1)) + base_class.name);
}
mlc::String print_base_class_list(mlc::Array<cpp_ast::CppBaseClass> base_classes) noexcept{
  if ((base_classes.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return (mlc::String(" : ", 3) + base_classes.map([=](cpp_ast::CppBaseClass base_class) mutable { return print_base_class_entry(base_class); }).join(mlc::String(", ", 2)));
  }
}
mlc::String print_function_modifiers_prefix(cpp_ast::CppFnModifiers modifiers) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  if (modifiers.is_static)   {
    parts.push_back(mlc::String("static", 6));
  }
  if (modifiers.is_virtual)   {
    parts.push_back(mlc::String("virtual", 7));
  }
  if (modifiers.is_explicit)   {
    parts.push_back(mlc::String("explicit", 8));
  }
  if ((parts.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return (parts.join(mlc::String(" ", 1)) + mlc::String(" ", 1));
  }
}
mlc::String print_function_modifiers_suffix(cpp_ast::CppFnModifiers modifiers) noexcept{
  if (modifiers.is_pure_virtual)   {
    return mlc::String(" = 0", 4);
  } else if (modifiers.is_deleted)   {
    return mlc::String(" = delete", 9);
  } else if (modifiers.is_defaulted)   {
    return mlc::String(" = default", 10);
  } else   {
    auto const_part = (modifiers.is_const ? (mlc::String(" const", 6)) : (mlc::String("", 0)));
    auto override_part = (modifiers.is_override ? (mlc::String(" override", 9)) : (mlc::String("", 0)));
    return (const_part + override_part);
  }
}
mlc::String print_function_prototype_core(std::shared_ptr<cpp_ast::CppFunctionPrototype> prototype, int indent_depth) noexcept{
  auto modifiers_prefix = print_function_modifiers_prefix(prototype->modifiers);
  auto return_type_code = print_type(prototype->return_type);
  auto parameters_code = print_parameter_list(prototype->parameters);
  auto modifiers_suffix = print_function_modifiers_suffix(prototype->modifiers);
  return (((((((((indent_text(indent_depth) + modifiers_prefix) + return_type_code) + mlc::String(" ", 1)) + prototype->name) + mlc::String("(", 1)) + parameters_code) + mlc::String(")", 1)) + modifiers_suffix) + mlc::String(" noexcept;", 10));
}
mlc::String print_function_prototype_line(std::shared_ptr<cpp_ast::CppFunctionPrototype> prototype) noexcept{
  return ((prototype->template_prefix + print_function_prototype_core(prototype, 0)) + mlc::String("\n", 1));
}
mlc::String print_member_field_line(std::shared_ptr<cpp_ast::CppType> field_type, mlc::String field_name, mlc::String default_value) noexcept{
  if ((default_value != mlc::String("", 0)))   {
    return ((((((indent_text(1) + print_type(field_type)) + mlc::String(" ", 1)) + field_name) + mlc::String(" = ", 3)) + default_value) + mlc::String(";", 1));
  } else   {
    return ((((indent_text(1) + print_type(field_type)) + mlc::String(" ", 1)) + field_name) + mlc::String(";", 1));
  }
}
mlc::String print_class_member_line(cpp_ast::CppClassMember member) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppClassMemberAccess& cppClassMemberAccess) { auto [access] = cppClassMemberAccess; return (indent_text(1) + print_access_label(access)); },
[&](const cpp_ast::CppClassMemberField& cppClassMemberField) { auto [field_type, field_name, default_value] = cppClassMemberField; return print_member_field_line(field_type, field_name, default_value); },
[&](const cpp_ast::CppClassMemberTypeAlias& cppClassMemberTypeAlias) { auto [alias, type_node] = cppClassMemberTypeAlias; return (((((indent_text(1) + mlc::String("using ", 6)) + alias) + mlc::String(" = ", 3)) + print_type(type_node)) + mlc::String(";", 1)); },
[&](const cpp_ast::CppClassMemberFunction& cppClassMemberFunction) { auto [prototype] = cppClassMemberFunction; return print_function_prototype_core(prototype, 1); }
}, member);
}
mlc::String print_class_body_members(mlc::Array<cpp_ast::CppClassMember> members) noexcept{
  return members.map([=](cpp_ast::CppClassMember member) mutable { return print_class_member_line(member); }).join(mlc::String("\n", 1));
}
mlc::String print_class_definition(cpp_ast::CppClassDefinition definition) noexcept{
  auto kind_word = (definition.is_struct ? (mlc::String("struct", 6)) : (mlc::String("class", 5)));
  auto bases = print_base_class_list(definition.base_classes);
  auto body = print_class_body_members(definition.members);
  if ((body.length() == 0))   {
    return (((((definition.template_prefix + kind_word) + mlc::String(" ", 1)) + definition.name) + bases) + mlc::String(" {};\n", 5));
  } else   {
    return (((((((definition.template_prefix + kind_word) + mlc::String(" ", 1)) + definition.name) + bases) + mlc::String(" {\n", 3)) + body) + mlc::String("\n};\n", 4));
  }
}
mlc::String typedef_declaration(mlc::String name, mlc::String type_code) noexcept{
  return ((((mlc::String("typedef ", 8) + mlc::to_string(type_code)) + mlc::String(" ", 1)) + mlc::to_string(name)) + mlc::String(";\n", 2));
}
mlc::String extern_block_declaration(mlc::String linkage, mlc::String body_code) noexcept{
  return ((((mlc::String("extern ", 7) + linkage) + mlc::String(" {\n", 3)) + body_code) + mlc::String("\n}\n", 3));
}
mlc::String print_template_declaration(mlc::String template_parameters, std::shared_ptr<cpp_ast::CppDeclaration> inner) noexcept{
  return (((mlc::String("template<", 9) + mlc::to_string(template_parameters)) + mlc::String(">\n", 2)) + print_decl_node(inner));
}
mlc::Array<mlc::String> variant_arm_type_strings(std::shared_ptr<cpp_ast::CppVariantArm> arm) noexcept{
  auto types = cpp_ast::cpp_variant_arm_types(arm);
  if ((types.length() == 0))   {
    return mlc::Array<mlc::String>{cpp_ast::cpp_variant_arm_name(arm)};
  } else   {
    return types.map([=](std::shared_ptr<cpp_ast::CppType> type_node) mutable { return print_type(type_node); });
  }
}
mlc::String print_variant_types(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept{
  return arms.flat_map([=](std::shared_ptr<cpp_ast::CppVariantArm> arm) mutable { return variant_arm_type_strings(arm); }).join(mlc::String(", ", 2));
}
bool is_empty_cpp_block(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{
  return [&]() -> bool {
auto __match_subject = statement;
if (std::holds_alternative<cpp_ast::CppBlock>((*__match_subject))) {
const cpp_ast::CppBlock& cppBlock = std::get<cpp_ast::CppBlock>((*__match_subject));
auto [statements] = cppBlock; return (statements.length() == 0);
}
return false;
std::abort();
}();
}
mlc::String conditional_if_statement(mlc::String condition_code, mlc::String then_code, std::shared_ptr<cpp_ast::CppStatement> else_branch, int depth) noexcept{
  if (is_empty_cpp_block(else_branch))   {
    return ((((mlc::String("if (", 4) + mlc::to_string(condition_code)) + mlc::String(") ", 2)) + mlc::to_string(then_code)) + mlc::String("", 0));
  } else   {
    return if_statement(condition_code, then_code, print_statement_body(else_branch, depth));
  }
}
mlc::String print_statement_node(std::shared_ptr<cpp_ast::CppStatement> statement, int depth) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppAutoDecl& cppAutoDecl) { auto [name, initializer] = cppAutoDecl; return auto_declaration(name, print_expr(initializer)); },
[&](const cpp_ast::CppVarDecl& cppVarDecl) { auto [name, type_node, initializer] = cppVarDecl; return variable_declaration(print_type(type_node), name, print_expr(initializer)); },
[&](const cpp_ast::CppConstDecl& cppConstDecl) { auto [name, type_node, initializer] = cppConstDecl; return const_declaration(print_type(type_node), name, print_expr(initializer)); },
[&](const cpp_ast::CppConstexprAutoDecl& cppConstexprAutoDecl) { auto [name, initializer] = cppConstexprAutoDecl; return constexpr_auto_declaration(name, print_expr(initializer)); },
[&](const cpp_ast::CppReturn& cppReturn) { auto [expression] = cppReturn; return ((mlc::String("return ", 7) + print_expr(expression)) + mlc::String(";", 1)); },
[&](const cpp_ast::CppExpressionStatement& cppExpressionStatement) { auto [expression] = cppExpressionStatement; return (print_expr(expression) + mlc::String(";", 1)); },
[&](const cpp_ast::CppBlock& cppBlock) { auto [statements] = cppBlock; return formatted_block(print_statements(statements, (depth + 1)), depth); },
[&](const cpp_ast::CppIf& cppIf) { auto [condition, then_branch, else_branch] = cppIf; return conditional_if_statement(print_expr(condition), print_statement_body(then_branch, depth), else_branch, depth); },
[&](const cpp_ast::CppWhile& cppWhile) { auto [condition, body] = cppWhile; return while_statement(print_expr(condition), print_statement_body(body, depth)); },
[&](const cpp_ast::CppFor& cppFor) { auto [variable_name, range_expression, body] = cppFor; return range_for_statement(variable_name, print_expr(range_expression), formatted_block(print_statements(body, (depth + 1)), depth)); },
[&](const cpp_ast::CppStructuredBinding& cppStructuredBinding) { auto [binding_names, initializer] = cppStructuredBinding; return structured_binding_declaration(binding_names, print_expr(initializer)); },
[&](const cpp_ast::CppStatementFragment& cppStatementFragment) { auto [fragment] = cppStatementFragment; return fragment; }
}, (*statement));
}
mlc::String print_decl_sequence(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
  return declarations.map([=](std::shared_ptr<cpp_ast::CppDeclaration> declaration) mutable { return print_decl_node(declaration); }).join(mlc::String("", 0));
}
mlc::String print_decl_node(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppInclude& cppInclude) { auto [is_angle, path] = cppInclude; return (is_angle ? (include_angle(path)) : (include_quoted(path))); },
[&](const cpp_ast::CppIfndef& cppIfndef) { auto [macro] = cppIfndef; return ifndef_directive(macro); },
[&](const cpp_ast::CppDefineMacro& cppDefineMacro) { auto [macro, value] = cppDefineMacro; return define_macro_directive(macro, value); },
[&](const cpp_ast::CppEndif& cppEndif) { auto [comment] = cppEndif; return endif_directive(comment); },
[&](const cpp_ast::CppUndef& cppUndef) { auto [macro] = cppUndef; return undef_directive(macro); },
[&](const cpp_ast::CppNamespaceBegin& cppNamespaceBegin) { auto [name] = cppNamespaceBegin; return namespace_begin_directive(name); },
[&](const cpp_ast::CppNamespaceEnd& cppNamespaceEnd) { auto [name] = cppNamespaceEnd; return namespace_end_directive(name); },
[&](const cpp_ast::CppUsing& cppUsing) { auto [alias, type_code] = cppUsing; return using_alias(alias, type_code); },
[&](const cpp_ast::CppUsingNamespace& cppUsingNamespace) { auto [namespace_identifier] = cppUsingNamespace; return using_namespace_directive(namespace_identifier); },
[&](const cpp_ast::CppStruct& cppStruct) { auto [template_prefix, name, fields, forward_only] = cppStruct; return struct_definition(template_prefix, name, print_struct_fields(fields), forward_only); },
[&](const cpp_ast::CppForwardDecl& cppForwardDecl) { auto [kind, name] = cppForwardDecl; return print_forward_declaration(kind, name); },
[&](const cpp_ast::CppFnProto& cppFnProto) { auto [template_prefix, return_type, name, parameters] = cppFnProto; return function_prototype(template_prefix, return_type, name, print_comma_separated_strings(parameters)); },
[&](const cpp_ast::CppFnDef& cppFnDef) { auto [template_prefix, return_type, name, parameters, body, body_statement_depth] = cppFnDef; return print_function_definition(template_prefix, return_type, name, print_comma_separated_strings(parameters), body, body_statement_depth); },
[&](const cpp_ast::CppNamespace& cppNamespace) { auto [name, declarations] = cppNamespace; return namespace_block(name, print_decls(declarations)); },
[&](const cpp_ast::CppVariant& cppVariant) { auto [template_prefix, name, arms] = cppVariant; return variant_type_alias(template_prefix, name, print_variant_types(arms)); },
[&](const cpp_ast::CppConceptRequires& cppConceptRequires) { auto [template_prefix, trait_name, self_parameter_name, requires_expressions] = cppConceptRequires; return concept_requires_definition(template_prefix, trait_name, self_parameter_name, requires_expressions); },
[&](const cpp_ast::CppStaticAssert& cppStaticAssert) { auto [concept_cpp, type_arguments, message] = cppStaticAssert; return static_assert_declaration(concept_cpp, type_arguments, message); },
[&](const cpp_ast::CppStdHashSpecialization& cppStdHashSpecialization) { auto [type_name, body_statements] = cppStdHashSpecialization; return std_hash_specialization_definition(type_name, body_statements); },
[&](const cpp_ast::CppDeclarationSequence& cppDeclarationSequence) { auto [declarations] = cppDeclarationSequence; return print_decl_sequence(declarations); },
[&](const cpp_ast::CppDeclarationEmpty& cppDeclarationEmpty) { return mlc::String("", 0); },
[&](const cpp_ast::CppBlankLine& cppBlankLine) { return mlc::String("\n", 1); },
[&](const cpp_ast::CppHostEntryMain& cppHostEntryMain) { auto [qualified_namespace] = cppHostEntryMain; return host_entry_main_epilogue(qualified_namespace); },
[&](const cpp_ast::CppClassDeclaration& cppClassDeclaration) { auto [definition] = cppClassDeclaration; return print_class_definition(definition); },
[&](const cpp_ast::CppTypedefDeclaration& cppTypedefDeclaration) { auto [name, type_node] = cppTypedefDeclaration; return typedef_declaration(name, print_type(type_node)); },
[&](const cpp_ast::CppTemplateDeclaration& cppTemplateDeclaration) { auto [template_parameters, inner] = cppTemplateDeclaration; return print_template_declaration(template_parameters, inner); },
[&](const cpp_ast::CppFunctionPrototypeDecl& cppFunctionPrototypeDecl) { auto [prototype] = cppFunctionPrototypeDecl; return print_function_prototype_line(std::make_shared<cpp_ast::CppFunctionPrototype>(prototype)); },
[&](const cpp_ast::CppExternBlock& cppExternBlock) { auto [linkage, declarations] = cppExternBlock; return extern_block_declaration(linkage, print_decls(declarations)); },
[&](const cpp_ast::CppDeclarationFragment& cppDeclarationFragment) { auto [code] = cppDeclarationFragment; return code; }
}, (*declaration));
}
mlc::String print_decls(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
  return declarations.map([=](std::shared_ptr<cpp_ast::CppDeclaration> declaration) mutable { return print_decl_node(declaration); }).join(mlc::String("\n\n", 2));
}
mlc::String print_file_node(std::shared_ptr<cpp_ast::CppFile> file) noexcept{
  auto header_code = print_decls(cpp_ast::cpp_file_header(file));
  auto source_code = print_decls(cpp_ast::cpp_file_source(file));
  if ((header_code.length() == 0))   {
    return source_code;
  } else if ((source_code.length() == 0))   {
    return header_code;
  } else   {
    return ((header_code + mlc::String("\n\n", 2)) + source_code);
  }
}
mlc::String invoked_while_expression(std::shared_ptr<cpp_ast::CppExpression> condition, mlc::String body_statements) noexcept{
  return ((((mlc::String("[&]() {\nwhile (", 15) + mlc::to_string(print_expr(condition))) + mlc::String(") {\n", 4)) + mlc::to_string(body_statements)) + mlc::String("}\n}()", 5));
}
mlc::String invoked_for_expression(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpression> iterator, mlc::String body_statements) noexcept{
  return ((((((mlc::String("[&]() {\nfor (auto ", 18) + mlc::to_string(variable_name)) + mlc::String(" : ", 3)) + mlc::to_string(print_expr(iterator))) + mlc::String(") {\n", 4)) + mlc::to_string(body_statements)) + mlc::String("}\n}()", 5));
}
mlc::String invoked_block_expression(mlc::String body_statements) noexcept{
  return ((mlc::String("[&]() {\n", 8) + mlc::to_string(body_statements)) + mlc::String("}()", 3));
}
mlc::String invoked_block_with_return_expression(mlc::String return_type, mlc::String body_statements) noexcept{
  return ((((mlc::String("[&]() -> ", 9) + mlc::to_string(return_type)) + mlc::String(" {\n", 3)) + mlc::to_string(body_statements)) + mlc::String("}()", 3));
}
mlc::String question_try_expression(std::shared_ptr<cpp_ast::CppExpression> inner) noexcept{
  return ((mlc::String("({ auto __q = ", 14) + mlc::to_string(print_expr(inner))) + mlc::String("; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })", 85));
}
mlc::String with_block_expression(std::shared_ptr<cpp_ast::CppExpression> resource, mlc::String binder, mlc::String body_statements) noexcept{
  return ((((((((mlc::String("{\nauto ", 7) + mlc::to_string(binder)) + mlc::String(" = (", 4)) + mlc::to_string(print_expr(resource))) + mlc::String(");\n", 3)) + mlc::to_string(body_statements)) + mlc::String("", 0)) + mlc::to_string(binder)) + mlc::String(".drop();\n}\n", 11));
}
mlc::String lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String return_type_code, mlc::String body_code) noexcept{
  return ((((((((mlc::String("", 0) + mlc::to_string(capture_prefix)) + mlc::String("(", 1)) + mlc::to_string(parameters_code)) + mlc::String(") -> ", 5)) + mlc::to_string(return_type_code)) + mlc::String(" {\n", 3)) + mlc::to_string(body_code)) + mlc::String("\n}", 2));
}
mlc::String print_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body) noexcept{
  auto capture_prefix = print_capture_list(captures);
  auto parameters_code = print_parameter_list(parameters);
  auto return_type_code = print_type(return_type);
  auto body_code = print_statements(body, 1);
  return lambda_expression(capture_prefix, parameters_code, return_type_code, body_code);
}
mlc::String mutable_lambda_expression(mlc::String capture_prefix, mlc::String parameters_code, mlc::String body_expression_code) noexcept{
  return ((((((mlc::String("", 0) + mlc::to_string(capture_prefix)) + mlc::String("(", 1)) + mlc::to_string(parameters_code)) + mlc::String(") mutable { return ", 19)) + mlc::to_string(body_expression_code)) + mlc::String("; }", 3));
}
mlc::String print_mutable_lambda_expression(mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> captures, mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters, std::shared_ptr<cpp_ast::CppExpression> body) noexcept{
  auto capture_prefix = print_capture_list(captures);
  auto parameters_code = print_parameter_list(parameters);
  return mutable_lambda_expression(capture_prefix, parameters_code, print_expr(body));
}
mlc::String print_member_expression(std::shared_ptr<cpp_ast::CppExpression> object, mlc::String member_name, bool is_pointer) noexcept{
  if (is_pointer)   {
    return pointer_field_access(print_expr(object), member_name);
  } else   {
    return value_field_access(print_expr(object), member_name);
  }
}
mlc::String print_integer_literal(int integer_value) noexcept{
  return mlc::to_string(integer_value);
}
mlc::String print_string_literal(mlc::String string_value) noexcept{
  return ((mlc::String("\"", 1) + escape_cpp_string_content(string_value)) + mlc::String("\"", 1));
}
mlc::String print_boolean_literal(bool boolean_value) noexcept{
  if (boolean_value)   {
    return mlc::String("true", 4);
  } else   {
    return mlc::String("false", 5);
  }
}
mlc::String print_identifier(mlc::String name) noexcept{
  return name;
}
mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
  return std::visit(overloaded{[&](const cpp_ast::CppInt& cppInt) { auto [integer_value] = cppInt; return print_integer_literal(integer_value); },
[&](const cpp_ast::CppStr& cppStr) { auto [string_value] = cppStr; return print_string_literal(string_value); },
[&](const cpp_ast::CppCharLiteral& cppCharLiteral) { auto [character_value] = cppCharLiteral; return ((mlc::String("'", 1) + escape_cpp_character(character_value)) + mlc::String("'", 1)); },
[&](const cpp_ast::CppFloatLiteral& cppFloatLiteral) { auto [float_text] = cppFloatLiteral; return float_text; },
[&](const cpp_ast::CppBool& cppBool) { auto [boolean_value] = cppBool; return print_boolean_literal(boolean_value); },
[&](const cpp_ast::CppIdent& cppIdent) { auto [name] = cppIdent; return print_identifier(name); },
[&](const cpp_ast::CppCall& cppCall) { auto [callee, arguments] = cppCall; return render_function_call_parentheses(print_expr(callee), print_comma_separated_expressions(arguments)); },
[&](const cpp_ast::CppMember& cppMember) { auto [object, member_name, is_pointer] = cppMember; return print_member_expression(object, member_name, is_pointer); },
[&](const cpp_ast::CppIndex& cppIndex) { auto [object, index] = cppIndex; return render_index_subscript(print_expr(object), print_expr(index)); },
[&](const cpp_ast::CppBinary& cppBinary) { auto [operation, left, right] = cppBinary; return render_parenthesized_binary(print_expr(left), operation, print_expr(right)); },
[&](const cpp_ast::CppUnary& cppUnary) { auto [operation, inner] = cppUnary; return render_parenthesized_unary(operation, print_expr(inner)); },
[&](const cpp_ast::CppTernary& cppTernary) { auto [condition, then_branch, else_branch] = cppTernary; return render_ternary_conditional(print_expr(condition), print_expr(then_branch), print_expr(else_branch)); },
[&](const cpp_ast::CppInitList& cppInitList) { auto [elements] = cppInitList; return init_list_braces(print_comma_separated_expressions(elements)); },
[&](const cpp_ast::CppAggregateInit& cppAggregateInit) { auto [type_name, elements] = cppAggregateInit; return aggregate_initializer_expression(type_name, elements); },
[&](const cpp_ast::CppStdVisit& cppStdVisit) { auto [subject, handlers] = cppStdVisit; return std_visit_expression(print_comma_separated_expressions_multiline(handlers), print_expr(subject)); },
[&](const cpp_ast::CppVisitArmWild& cppVisitArmWild) { auto [void_return, return_cpp, body] = cppVisitArmWild; return print_visit_arm_wild(void_return, return_cpp, body); },
[&](const cpp_ast::CppVisitArmBinding& cppVisitArmBinding) { auto [parameter_name, void_return, return_cpp, body] = cppVisitArmBinding; return print_visit_arm_binding(parameter_name, void_return, return_cpp, body); },
[&](const cpp_ast::CppVisitArmConstructed& cppVisitArmConstructed) { auto [parameter_declaration, binding_prefix, void_return, return_cpp, body] = cppVisitArmConstructed; return print_visit_arm_constructed(parameter_declaration, binding_prefix, void_return, return_cpp, body); },
[&](const cpp_ast::CppVisitArmConstructedGeneric& cppVisitArmConstructedGeneric) { auto [type_name, binding_name, binding_prefix, void_return, return_cpp, body] = cppVisitArmConstructedGeneric; return print_visit_arm_constructed_generic(type_name, binding_name, binding_prefix, void_return, return_cpp, body); },
[&](const cpp_ast::CppCast& cppCast) { auto [kind, target_type, operand] = cppCast; return cast_expression(print_cast_kind_prefix(kind), print_type(target_type), print_expr(operand)); },
[&](const cpp_ast::CppLambda& cppLambda) { auto [captures, parameters, return_type, body] = cppLambda; return print_lambda_expression(captures, parameters, return_type, body); },
[&](const cpp_ast::CppMutableLambda& cppMutableLambda) { auto [captures, parameters, body] = cppMutableLambda; return print_mutable_lambda_expression(captures, parameters, body); },
[&](const cpp_ast::CppInvokedWhile& cppInvokedWhile) { auto [condition, body_statements] = cppInvokedWhile; return invoked_while_expression(condition, body_statements); },
[&](const cpp_ast::CppInvokedFor& cppInvokedFor) { auto [variable_name, iterator, body_statements] = cppInvokedFor; return invoked_for_expression(variable_name, iterator, body_statements); },
[&](const cpp_ast::CppInvokedBlock& cppInvokedBlock) { auto [body_statements] = cppInvokedBlock; return invoked_block_expression(body_statements); },
[&](const cpp_ast::CppInvokedBlockWithReturn& cppInvokedBlockWithReturn) { auto [return_type, body_statements] = cppInvokedBlockWithReturn; return invoked_block_with_return_expression(return_type, body_statements); },
[&](const cpp_ast::CppQuestionTry& cppQuestionTry) { auto [inner] = cppQuestionTry; return question_try_expression(inner); },
[&](const cpp_ast::CppWithBlock& cppWithBlock) { auto [resource, binder, body_statements] = cppWithBlock; return with_block_expression(resource, binder, body_statements); }
}, (*expression));
}
mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{
  return print_statement_at_depth(statement, 0);
}
mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
  return print_decl_node(declaration);
}
mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept{
  return print_file_node(file);
}

} // namespace print
